/*!
 * \file
 * <!--
 * Copyright 2003,2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2000 Bernardo Innocenti <bernie@codewiz.org>
 * All Rights Reserved.
 * -->
 *
 * \brief Buffered serial I/O driver
 *
 * The serial rx interrupt buffers incoming data in a software FIFO
 * to decouple the higher level protocols from the line speed.
 * Outgoing data is buffered as well for better performance.
 * This driver is not optimized for best performance, but it
 * has proved to be fast enough to handle transfer rates up to
 * 38400bps on a 16MHz 80196.
 *
 * MODULE CONFIGURATION
 *	\li \c CONFIG_SER_HWHANDSHAKE define this preprocessor symbol to enable
 *	support for RTS/CTS handshake. Support is incomplete/untested
 *	for 80196.
 *  \li \c CONFIG_SER_TXTIMEOUT - Enable software serial transmission timeouts
 *
 *
 * \version $Id$
 * \author Bernardo Innocenti <bernie@develer.com>
 */

/*
 * $Log$
 * Revision 1.3  2004/06/02 21:35:24  aleph
 * Serial enhancements: interruptible receive handler and 8 bit serial status for AVR; remove volatile attribute to FIFOBuffer, useless for new fifobuf routens
 *
 * Revision 1.2  2004/05/23 18:21:53  bernie
 * Trim CVS logs and cleanup header info.
 *
 */

#include <mware/formatwr.h>
#include <drv/kdebug.h>
#include "ser.h"
#include "ser_p.h"
#include "hw.h"

#ifdef CONFIG_KERNEL
	#include <kern/proc.h>
#endif
#if defined(CONFIG_SER_TXTIMEOUT) || defined(CONFIG_SER_RXTIMEOUT)
	#include <drv/timer.h>
#endif


/* Serial configuration parameters */
#define SER_CTSDELAY	    70	/*!< CTS line retry interval (ms) */
#define SER_TXPOLLDELAY	     2	/*!< Transmit buffer full retry interval (ms) */
#define SER_RXPOLLDELAY	     2	/*!< Receive buffer empty retry interval (ms) */


struct Serial ser_handles[SER_CNT];


/*!
 * Inserisce il carattere c nel buffer di trasmissione.
 * Questa funzione mette il processo chiamante in attesa
 * quando il buffer e' pieno.
 *
 * \return EOF in caso di errore o timeout, altrimenti
 *         il carattere inviato.
 */
int ser_putchar(int c, struct Serial *port)
{
	if (fifo_isfull_locked(&port->txfifo))
	{
#ifdef CONFIG_SER_TXTIMEOUT
		time_t start_time = timer_gettick();
#endif

		/* Attende finche' il buffer e' pieno... */
		do
		{
#ifdef CONFIG_KERN_SCHED
			/* Give up timeslice to other processes. */
			proc_switch();
#endif
#ifdef CONFIG_SER_TXTIMEOUT
			if (timer_gettick() - start_time >= port->txtimeout)
			{
				port->status |= SERRF_TXTIMEOUT;
				return EOF;
			}
#endif /* CONFIG_SER_TXTIMEOUT */
		}
		while (fifo_isfull_locked(&port->txfifo));
	}

	fifo_push(&port->txfifo, (unsigned char)c);

	/* (re)trigger tx interrupt */
	port->hw->table->enabletxirq(port->hw);

	/* Avoid returning signed estended char */
	return (int)((unsigned char)c);
}


/*!
 * Preleva un carattere dal buffer di ricezione.
 * Questa funzione mette il processo chiamante in attesa
 * quando il buffer e' vuoto. L'attesa ha un timeout
 * di ser_rxtimeout millisecondi.
 *
 * \return EOF in caso di errore o timeout, altrimenti
 *         il carattere ricevuto.
 */
int ser_getchar(struct Serial *port)
{
	int result;

	if (fifo_isempty_locked(&port->rxfifo))
	{
#ifdef CONFIG_SER_RXTIMEOUT
		time_t start_time = timer_gettick();
#endif
		/* Wait while buffer is empty */
		do
		{
#ifdef CONFIG_KERN_SCHED
			/* Give up timeslice to other processes. */
			proc_switch();
#endif
#ifdef CONFIG_SER_RXTIMEOUT
			if (timer_gettick() - start_time >= port->rxtimeout)
			{
				port->status |= SERRF_RXTIMEOUT;
				return EOF;
			}
#endif /* CONFIG_SER_RXTIMEOUT */
		}
		while (fifo_isempty_locked(&port->rxfifo));
	}

	/*
	 * Get a byte from the FIFO (avoiding sign-extension),
	 * re-enable RTS, then return result.
	 */
	result = (int)(unsigned char)fifo_pop(&port->rxfifo);
	return port->status ? EOF : result;
}


/*!
 * Preleva un carattere dal buffer di ricezione.
 * Se il buffer e' vuoto, ser_getchar_nowait() ritorna
 * immediatamente EOF.
 */
int ser_getchar_nowait(struct Serial *port)
{
	if (fifo_isempty_locked(&port->rxfifo))
		return EOF;

	/* NOTE: the double cast prevents unwanted sign extension */
	return (int)(unsigned char)fifo_pop(&port->rxfifo);
}


/*!
 * Read a line long at most as size and puts it
 * in buf.
 * \return number of chars read or EOF in case
 *         of error.
 */
int ser_gets(struct Serial *port, char *buf, int size)
{
	return ser_gets_echo(port, buf, size, false);
}


/*!
 * Read a line long at most as size and puts it
 * in buf, with optional echo.
 * \return number of chars read or EOF in case
 *         of error.
 */
int ser_gets_echo(struct Serial *port, char *buf, int size, bool echo)
{
	int i = 0;
	int c;

	for (;;)
	{
		if ((c = ser_getchar(port)) == EOF)
			return -1;
		/* FIXME */
		if (c == '\r' || c == '\n' || i >= size-1)
		{
			buf[i] = '\0';
			if (echo)
				ser_print(port, "\r\n");
			break;
		}
		buf[i++] = c;
		if (echo)
			ser_putchar(c, port);
	}

	return i;
}


/*!
 * Read at most size bytes and puts them
 * in buf.
 * \return number of bytes read or EOF in case
 *         of error.
 */
int ser_read(struct Serial *port, char *buf, size_t size)
{
	size_t i = 0;
	int c;

	while (i < size)
	{
		if ((c = ser_getchar(port)) == EOF)
			return EOF;
		buf[i++] = c;
	}

	return i;
}


/*!
 * Write a string to serial.
 * \return 0 if OK, EOF in case of error.
 */
int ser_print(struct Serial *port, const char *s)
{
	while (*s)
	{
		if (ser_putchar(*s++, port) == EOF)
			return EOF;
	}
	return 0;
}


/*!
 * \brief Write a buffer to serial.
 *
 * \return 0 if OK, EOF in case of error.
 */
int ser_write(struct Serial *port, const void *buf, size_t len)
{
	while (len--)
	{
		if (ser_putchar(*((const char *)buf)++, port) == EOF)
			return EOF;
	}
	return 0;
}


/*!
 * Formatted write
 */
int ser_printf(struct Serial *port, const char *format, ...)
{
	va_list ap;
	int len;

	ser_setstatus(port, 0);
	va_start(ap, format);
	len = _formatted_write(format, (void (*)(char, void *))ser_putchar, port, ap);
	va_end(ap);

	return len;
}

#if defined(CONFIG_SER_RXTIMEOUT) || defined(CONFIG_SER_TXTIMEOUT)
void ser_settimeouts(struct Serial *port, time_t rxtimeout, time_t txtimeout)
{
	port->rxtimeout = rxtimeout;
	port->txtimeout = txtimeout;
}
#endif /* defined(CONFIG_SER_RXTIMEOUT) || defined(CONFIG_SER_TXTIMEOUT) */


void ser_setbaudrate(struct Serial *port, unsigned long rate)
{
	port->hw->table->setbaudrate(port->hw, rate);
}


void ser_setparity(struct Serial *port, int parity)
{
	port->hw->table->setparity(port->hw, parity);
}


/*!
 * Flush both the RX and TX buffers.
 */
void ser_purge(struct Serial *ser)
{
	fifo_flush(&ser->rxfifo);
	fifo_flush(&ser->txfifo);
}


/*!
 * Initialize serial
 */
struct Serial *ser_open(unsigned int unit)
{
	struct Serial *port;

	ASSERT(unit < countof(ser_handles));

	port = &ser_handles[unit];

	ASSERT(!port->is_open);

	port->unit = unit;
	port->is_open = true;

	/* Initialize circular buffer */
	fifo_init(&port->rxfifo, port->rxbuffer, sizeof(port->rxbuffer));
	fifo_init(&port->txfifo, port->txbuffer, sizeof(port->txbuffer));

	port->hw = ser_hw_getdesc(unit);
	port->hw->table->init(port->hw, port);

	/* Set default values */
#if defined(CONFIG_SER_RXTIMEOUT) || defined(CONFIG_SER_TXTIMEOUT)
	ser_settimeouts(port, CONFIG_SER_RXTIMEOUT, CONFIG_SER_TXTIMEOUT);
#endif
	ser_setbaudrate(port, CONFIG_SER_DEFBAUDRATE);

	return port;
}


/*!
 * Clean up serial port, disabling the associated hardware.
 */
void ser_close(struct Serial *port)
{
	ASSERT(port->is_open);

	port->is_open = false;
	port->hw->table->cleanup(port->hw);
	port->hw = NULL;
}

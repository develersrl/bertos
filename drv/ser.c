/*!
 * \file
 * <!--
 * Copyright 2003, 2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2000 Bernardo Innocenti <bernie@codewiz.org>
 * This file is part of DevLib - See devlib/README for information.
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
 *
 *  \li \c CONFIG_SER_HWHANDSHAKE - set to 1 to enable RTS/CTS handshake.
 *         Support is incomplete/untested.
 *  \li \c CONFIG_SER_TXTIMEOUT - Enable software serial transmission timeouts
 *
 *
 * \version $Id$
 * \author Bernardo Innocenti <bernie@develer.com>
 */

/*#*
 *#* $Log$
 *#* Revision 1.16  2004/09/06 21:40:50  bernie
 *#* Move buffer handling in chip-specific driver.
 *#*
 *#* Revision 1.15  2004/08/25 14:12:08  rasky
 *#* Aggiornato il comment block dei log RCS
 *#*
 *#* Revision 1.14  2004/08/24 16:22:57  bernie
 *#* Thinkos; Doxygen fixes
 *#*
 *#* Revision 1.13  2004/08/24 16:20:48  bernie
 *#* ser_read(): Make buffer argument void *#* for consistency with ANSI C and ser_write()
 *#*
 *#* Revision 1.12  2004/08/24 13:49:39  bernie
 *#* Fix thinko.
 *#*
 *#* Revision 1.11  2004/08/15 05:32:22  bernie
 *#* ser_resync(): New function.
 *#*
 *#* Revision 1.10  2004/08/10 06:29:50  bernie
 *#* Rename timer_gettick() to timer_ticks().
 *#*
 *#* Revision 1.9  2004/08/08 06:06:20  bernie
 *#* Use new-style CONFIG_ idiom; Fix module-wide documentation.
 *#*
 *#* Revision 1.8  2004/07/29 22:57:09  bernie
 *#* ser_drain(): New function; Make Serial::is_open a debug-only feature; Switch to new-style CONFIG_* macros.
 *#*
 *#* Revision 1.7  2004/07/18 21:49:03  bernie
 *#* Make CONFIG_SER_DEFBAUDRATE optional.
 *#*
 *#* Revision 1.6  2004/06/07 15:56:28  aleph
 *#* Remove cast-as-lvalue extension abuse
 *#*
 *#* Revision 1.5  2004/06/06 16:41:44  bernie
 *#* ser_putchar(): Use fifo_push_locked() to fix potential race on 8bit processors.
 *#*
 *#* Revision 1.4  2004/06/03 11:27:09  bernie
 *#* Add dual-license information.
 *#*
 *#* Revision 1.3  2004/06/02 21:35:24  aleph
 *#* Serial enhancements: interruptible receive handler and 8 bit serial status for AVR; remove volatile attribute to FIFOBuffer, useless for new fifobuf routens
 *#*
 *#* Revision 1.2  2004/05/23 18:21:53  bernie
 *#* Trim CVS logs and cleanup header info.
 *#*
 *#*/

#include <mware/formatwr.h>
#include <drv/kdebug.h>
#include "ser.h"
#include "ser_p.h"
#include "hw.h"

#ifdef CONFIG_KERNEL
	#include <kern/proc.h>
#endif
#if CONFIG_SER_TXTIMEOUT != -1 || CONFIG_SER_RXTIMEOUT != -1
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
	//ASSERT_VALID_FIFO(&port->txfifo);
	if (fifo_isfull_locked(&port->txfifo))
	{
#if CONFIG_SER_TXTIMEOUT != -1
		time_t start_time = timer_ticks();
#endif

		/* Attende finche' il buffer e' pieno... */
		do
		{
#if defined(CONFIG_KERN_SCHED) && CONFIG_KERN_SCHED
			/* Give up timeslice to other processes. */
			proc_switch();
#endif
#if CONFIG_SER_TXTIMEOUT != -1
			if (timer_ticks() - start_time >= port->txtimeout)
			{
				port->status |= SERRF_TXTIMEOUT;
				return EOF;
			}
#endif /* CONFIG_SER_TXTIMEOUT */
		}
		while (fifo_isfull_locked(&port->txfifo));
	}

	fifo_push_locked(&port->txfifo, (unsigned char)c);

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
#if CONFIG_SER_RXTIMEOUT != -1
		time_t start_time = timer_ticks();
#endif
		/* Wait while buffer is empty */
		do
		{
#if defined(CONFIG_KERN_SCHED) && CONFIG_KERN_SCHED
			/* Give up timeslice to other processes. */
			proc_switch();
#endif
#if CONFIG_SER_RXTIMEOUT != -1
			if (timer_ticks() - start_time >= port->rxtimeout)
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
	result = (int)(unsigned char)fifo_pop_locked(&port->rxfifo);
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
	return (int)(unsigned char)fifo_pop_locked(&port->rxfifo);
}


#if CONFIG_SER_GETS
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
 * Read a line long at most as size and put it
 * in buf, with optional echo.
 *
 * \return number of chars read, or EOF in case
 *         of error.
 */
int ser_gets_echo(struct Serial *port, char *buf, int size, bool echo)
{
	int i = 0;
	int c;

	for (;;)
	{
		if ((c = ser_getchar(port)) == EOF)
		{
			buf[i] = '\0';
			return -1;
		}

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
#endif /* !CONFIG_SER_GETS */


/*!
 * Read at most \a size bytes from \a port and put them in \a buf
 *
 * \return number of bytes actually read, or EOF in
 *         case of error.
 */
int ser_read(struct Serial *port, void *buf, size_t size)
{
	size_t i = 0;
	char *_buf = (char *)buf;
	int c;

	while (i < size)
	{
		if ((c = ser_getchar(port)) == EOF)
			return EOF;
		_buf[i++] = c;
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
 *
 * \todo Optimize with fifo_pushblock()
 */
int ser_write(struct Serial *port, const void *_buf, size_t len)
{
	const char *buf = _buf;

	while (len--)
	{
		if (ser_putchar(*buf++, port) == EOF)
			return EOF;
	}
	return 0;
}


#if CONFIG_PRINTF
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
#endif /* CONFIG_PRINTF */


#if CONFIG_SER_RXTIMEOUT != -1 || CONFIG_SER_TXTIMEOUT != -1
void ser_settimeouts(struct Serial *port, time_t rxtimeout, time_t txtimeout)
{
	port->rxtimeout = rxtimeout;
	port->txtimeout = txtimeout;
}
#endif /* CONFIG_SER_RXTIMEOUT || CONFIG_SER_TXTIMEOUT */

#if CONFIG_SER_RXTIMEOUT != -1
/*!
 * Discard input to resynchronize with remote end
 *
 * Discard incoming data until the port stops receiving
 * characters for at least \a delay milliseconds.
 *
 * \note Serial errors are reset before and after executing the purge.
 */
void ser_resync(struct Serial *port, time_t delay)
{
	time_t old_rxtimeout = port->rxtimeout;

	ser_settimeouts(port, delay, port->txtimeout);
	do
	{
		ser_setstatus(port, 0);
		ser_getchar(port);
	}
	while (!(ser_getstatus(port) & SERRF_RXTIMEOUT));

	/* Restore port to an usable status */
	ser_setstatus(port, 0);
	ser_settimeouts(port, old_rxtimeout, port->txtimeout);
}
#endif /* CONFIG_SER_RXTIMEOUT */


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
void ser_purge(struct Serial *port)
{
	fifo_flush_locked(&port->rxfifo);
	fifo_flush_locked(&port->txfifo);
}


/*!
 * Wait until all pending output is completely
 * transmitted to the other end.
 *
 * \note The current implementation only checks the
 *       software transmission queue. Any hardware
 *       FIFOs are ignored.
 */
void ser_drain(struct Serial *ser)
{
	while (!fifo_isempty(&ser->txfifo))
	{
#if defined(CONFIG_KERN_SCHED) && CONFIG_KERN_SCHED
			/* Give up timeslice to other processes. */
			proc_switch();
#endif
	}
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
	DB(port->is_open = true;)

	port->unit = unit;

	port->hw = ser_hw_getdesc(unit);

	/* Initialize circular buffers */
	fifo_init(&port->txfifo, port->hw->txbuffer, port->hw->txbuffer_size);
	fifo_init(&port->rxfifo, port->hw->rxbuffer, port->hw->rxbuffer_size);

	port->hw->table->init(port->hw, port);

	/* Set default values */
#if CONFIG_SER_RXTIMEOUT != -1 || CONFIG_SER_TXTIMEOUT != -1
	ser_settimeouts(port, CONFIG_SER_RXTIMEOUT, CONFIG_SER_TXTIMEOUT);
#endif
#if CONFIG_SER_DEFBAUDRATE
	ser_setbaudrate(port, CONFIG_SER_DEFBAUDRATE);
#endif

	return port;
}


/*!
 * Clean up serial port, disabling the associated hardware.
 */
void ser_close(struct Serial *port)
{
	ASSERT(port->is_open);
	DB(port->is_open = false;)

	port->hw->table->cleanup(port->hw);
	port->hw = NULL;
}

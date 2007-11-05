/**
 * \file
 * <!--
 * This file is part of BeRTOS.
 *
 * Bertos is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * As a special exception, you may use this file as part of a free software
 * library without restriction.  Specifically, if other files instantiate
 * templates or use macros or inline functions from this file, or you compile
 * this file and link it with other files to produce an executable, this
 * file does not by itself cause the resulting executable to be covered by
 * the GNU General Public License.  This exception does not however
 * invalidate any other reasons why the executable file might be covered by
 * the GNU General Public License.
 *
 * Copyright 2003, 2004, 2006 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2000 Bernardo Innocenti <bernie@codewiz.org>
 *
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

#include "ser.h"
#include "wdt.h"
#include "ser_p.h"
#include <mware/formatwr.h>
#include <cfg/debug.h>
#include <appconfig.h>

/*
 * Sanity check for config parameters required by this module.
 */
#if !defined(CONFIG_KERNEL) || ((CONFIG_KERNEL != 0) && CONFIG_KERNEL != 1)
	#error CONFIG_KERNEL must be set to either 0 or 1 in config.h
#endif
#if !defined(CONFIG_SER_RXTIMEOUT)
	#error CONFIG_SER_TXTIMEOUT missing in config.h
#endif
#if !defined(CONFIG_SER_RXTIMEOUT)
	#error CONFIG_SER_RXTIMEOUT missing in config.h
#endif
#if !defined(CONFIG_SER_GETS) || ((CONFIG_SER_GETS != 0) && CONFIG_SER_GETS != 1)
	#error CONFIG_SER_GETS must be set to either 0 or 1 in config.h
#endif
#if !defined(CONFIG_SER_DEFBAUDRATE)
	#error CONFIG_SER_DEFBAUDRATE missing in config.h
#endif
#if !defined(CONFIG_PRINTF)
	#error CONFIG_PRINTF missing in config.h
#endif

#if CONFIG_KERNEL
	#include <kern/proc.h>
#endif

#if CONFIG_SER_TXTIMEOUT != -1 || CONFIG_SER_RXTIMEOUT != -1
	#include <drv/timer.h>
#endif


/* Serial configuration parameters */
#define SER_CTSDELAY	    70	/**< CTS line retry interval (ms) */
#define SER_TXPOLLDELAY	     2	/**< Transmit buffer full retry interval (ms) */
#define SER_RXPOLLDELAY	     2	/**< Receive buffer empty retry interval (ms) */


struct Serial ser_handles[SER_CNT];


/**
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
		ticks_t start_time = timer_clock();
#endif

		/* Attende finche' il buffer e' pieno... */
		do
		{
			wdt_reset();
#if CONFIG_KERNEL && CONFIG_KERN_SCHED
			/* Give up timeslice to other processes. */
			proc_switch();
#endif
#if CONFIG_SER_TXTIMEOUT != -1
			if (timer_clock() - start_time >= port->txtimeout)
			{
				ATOMIC(port->status |= SERRF_TXTIMEOUT);
				return EOF;
			}
#endif /* CONFIG_SER_TXTIMEOUT */
		}
		while (fifo_isfull_locked(&port->txfifo));
	}

	fifo_push_locked(&port->txfifo, (unsigned char)c);

	/* (re)trigger tx interrupt */
	port->hw->table->txStart(port->hw);

	/* Avoid returning signed extended char */
	return (int)((unsigned char)c);
}


/**
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
	if (fifo_isempty_locked(&port->rxfifo))
	{
#if CONFIG_SER_RXTIMEOUT != -1
		ticks_t start_time = timer_clock();
#endif
		/* Wait while buffer is empty */
		do
		{
			wdt_reset();
#if CONFIG_KERNEL && CONFIG_KERN_SCHED
			/* Give up timeslice to other processes. */
			proc_switch();
#endif
#if CONFIG_SER_RXTIMEOUT != -1
			if (timer_clock() - start_time >= port->rxtimeout)
			{
				ATOMIC(port->status |= SERRF_RXTIMEOUT);
				return EOF;
			}
#endif /* CONFIG_SER_RXTIMEOUT */
		}
		while (fifo_isempty_locked(&port->rxfifo) && (ser_getstatus(port) & SERRF_RX) == 0);
	}

	/*
	 * Get a byte from the FIFO (avoiding sign-extension),
	 * re-enable RTS, then return result.
	 */
	if (ser_getstatus(port) & SERRF_RX)
		return EOF;
	return (int)(unsigned char)fifo_pop_locked(&port->rxfifo);
}


/**
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
/**
 * Read a line long at most as size and put it
 * in buf.
 * \return number of chars read or EOF in case
 *         of error.
 */
int ser_gets(struct Serial *port, char *buf, int size)
{
	return ser_gets_echo(port, buf, size, false);
}


/**
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


/**
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


/**
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


/**
 * \brief Write a buffer to serial.
 *
 * \return 0 if OK, EOF in case of error.
 *
 * \todo Optimize with fifo_pushblock()
 */
int ser_write(struct Serial *port, const void *_buf, size_t len)
{
	const char *buf = (const char *)_buf;

	while (len--)
	{
		if (ser_putchar(*buf++, port) == EOF)
			return EOF;
	}
	return 0;
}


#if CONFIG_PRINTF
/**
 * Formatted write
 */
int ser_printf(struct Serial *port, const char *format, ...)
{
	va_list ap;
	int len;

	va_start(ap, format);
	len = _formatted_write(format, (void (*)(char, void *))ser_putchar, port, ap);
	va_end(ap);

	return len;
}
#endif /* CONFIG_PRINTF */


#if CONFIG_SER_RXTIMEOUT != -1 || CONFIG_SER_TXTIMEOUT != -1
void ser_settimeouts(struct Serial *port, mtime_t rxtimeout, mtime_t txtimeout)
{
	port->rxtimeout = ms_to_ticks(rxtimeout);
	port->txtimeout = ms_to_ticks(txtimeout);
}
#endif /* CONFIG_SER_RXTIMEOUT || CONFIG_SER_TXTIMEOUT */

#if CONFIG_SER_RXTIMEOUT != -1
/**
 * Discard input to resynchronize with remote end.
 *
 * Discard incoming data until the port stops receiving
 * characters for at least \a delay milliseconds.
 *
 * \note Serial errors are reset before and after executing the purge.
 */
void ser_resync(struct Serial *port, mtime_t delay)
{
	mtime_t old_rxtimeout = ticks_to_ms(port->rxtimeout);

	ser_settimeouts(port, delay, ticks_to_ms(port->txtimeout));
	do
	{
		ser_setstatus(port, 0);
		ser_getchar(port);
	}
	while (!(ser_getstatus(port) & SERRF_RXTIMEOUT));

	/* Restore port to an usable status */
	ser_setstatus(port, 0);
	ser_settimeouts(port, old_rxtimeout, ticks_to_ms(port->txtimeout));
}
#endif /* CONFIG_SER_RXTIMEOUT */


void ser_setbaudrate(struct Serial *port, unsigned long rate)
{
	port->hw->table->setBaudrate(port->hw, rate);
}


void ser_setparity(struct Serial *port, int parity)
{
	port->hw->table->setParity(port->hw, parity);
}

void ser_clearstatus(struct Serial *port)
{
       ser_setstatus(port, 0);
}

/**
 * Flush both the RX and TX buffers.
 */
void ser_purge(struct Serial *port)
{
	fifo_flush_locked(&port->rxfifo);
	fifo_flush_locked(&port->txfifo);
}


/**
 * Wait until all pending output is completely
 * transmitted to the other end.
 *
 * \note The current implementation only checks the
 *       software transmission queue. Any hardware
 *       FIFOs are ignored.
 */
void ser_drain(struct Serial *ser)
{
	/*
	 * Wait until the FIFO becomes empty, and then until the byte currently in
	 * the hardware register gets shifted out.
	 */
	while (!fifo_isempty(&ser->txfifo)
	       || ser->hw->table->txSending(ser->hw))
	{
		#if CONFIG_KERNEL && CONFIG_KERN_SCHED
			/* Give up timeslice to other processes. */
			proc_switch();
		#endif
			wdt_reset();
	}
}


/**
 * Initialize a serial port.
 *
 * \param unit  Serial unit to open. Possible values are architecture dependant.
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
	ASSERT(port->hw->txbuffer);
	ASSERT(port->hw->rxbuffer);
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

	/* Clear error flags */
	ser_setstatus(port, 0);

	return port;
}


/**
 * Clean up serial port, disabling the associated hardware.
 */
void ser_close(struct Serial *port)
{
	ASSERT(port->is_open);
	DB(port->is_open = false;)

	// Wait until we finish sending everything
	ser_drain(port);

	port->hw->table->cleanup(port->hw);
	DB(port->hw = NULL;)

	/*
	 * We purge the FIFO buffer only after the low-level cleanup, so that
	 * we are sure that there are no more interrupts.
	 */
	ser_purge(port);
}

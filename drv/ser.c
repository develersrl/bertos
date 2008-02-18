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

#include <string.h> /* memset */

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
#if !defined(CONFIG_SER_DEFBAUDRATE)
	#error CONFIG_SER_DEFBAUDRATE missing in config.h
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
static int ser_putchar(int c, struct Serial *port)
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
static int ser_getchar(struct Serial *port)
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

#if 0
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
#endif


/**
 * Read at most \a size bytes from \a port and put them in \a buf
 *
 * \return number of bytes actually read.
 */
static size_t ser_read(struct KFile *fd, void *_buf, size_t size)
{
	KFileSerial *fds = KFILESERIAL(fd);

	size_t i = 0;
	char *buf = (char *)_buf;
	int c;

	while (i < size)
	{
		if ((c = ser_getchar(fds->ser)) == EOF)
			break;
		buf[i++] = c;
	}

	return i;
}

/**
 * \brief Write a buffer to serial.
 *
 * \return 0 if OK, EOF in case of error.
 *
 * \todo Optimize with fifo_pushblock()
 */
static size_t ser_write(struct KFile *fd, const void *_buf, size_t size)
{
	KFileSerial *fds = KFILESERIAL(fd);
	const char *buf = (const char *)_buf;
	size_t i = 0;

	while (size--)
	{
		if (ser_putchar(*buf++, fds->ser) == EOF)
			break;
		i++;
	}
	return i;
}


#if CONFIG_SER_RXTIMEOUT != -1 || CONFIG_SER_TXTIMEOUT != -1
void ser_settimeouts(struct KFileSerial *fd, mtime_t rxtimeout, mtime_t txtimeout)
{
	fd->ser->rxtimeout = ms_to_ticks(rxtimeout);
	fd->ser->txtimeout = ms_to_ticks(txtimeout);
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
void ser_resync(struct KFileSerial *fd, mtime_t delay)
{
	mtime_t old_rxtimeout = ticks_to_ms(fd->ser->rxtimeout);

	ser_settimeouts(fd, delay, ticks_to_ms(fd->ser->txtimeout));
	do
	{
		ser_setstatus(fd->ser, 0);
		ser_getchar(fd->ser);
	}
	while (!(ser_getstatus(fd->ser) & SERRF_RXTIMEOUT));

	/* Restore port to an usable status */
	ser_setstatus(fd->ser, 0);
	ser_settimeouts(fd, old_rxtimeout, ticks_to_ms(fd->ser->txtimeout));
}
#endif /* CONFIG_SER_RXTIMEOUT */


void ser_setbaudrate(struct KFileSerial *fd, unsigned long rate)
{
	fd->ser->hw->table->setBaudrate(fd->ser->hw, rate);
}


void ser_setparity(struct KFileSerial *fd, int parity)
{
	fd->ser->hw->table->setParity(fd->ser->hw, parity);
}

static int ser_error(struct KFile *fd)
{
	KFileSerial *fds = KFILESERIAL(fd);
	return ser_getstatus(fds->ser);
}

static void ser_clearerr(struct KFile *fd)
{
	KFileSerial *fds = KFILESERIAL(fd);
	ser_setstatus(fds->ser, 0);
}



/**
 * Flush both the RX and TX buffers.
 */
void ser_purge(struct KFileSerial *fd)
{
	ser_purgeRx(fd);
	ser_purgeTx(fd);
}

/**
 * Flush RX buffer.
 */
void ser_purgeRx(struct KFileSerial *fd)
{
	fifo_flush_locked(&fd->ser->rxfifo);
}

/**
 * Flush TX buffer.
 */
void ser_purgeTx(struct KFileSerial *fd)
{
	fifo_flush_locked(&fd->ser->txfifo);
}


/**
 * Wait until all pending output is completely
 * transmitted to the other end.
 *
 * \note The current implementation only checks the
 *       software transmission queue. Any hardware
 *       FIFOs are ignored.
 */
static int ser_flush(struct KFile *fd)
{
	KFileSerial *fds = KFILESERIAL(fd);

	/*
	 * Wait until the FIFO becomes empty, and then until the byte currently in
	 * the hardware register gets shifted out.
	 */
	while (!fifo_isempty(&fds->ser->txfifo)
	       || fds->ser->hw->table->txSending(fds->ser->hw))
	{
		#if CONFIG_KERNEL && CONFIG_KERN_SCHED
			/* Give up timeslice to other processes. */
			proc_switch();
		#endif
			wdt_reset();
	}
	return 0;
}


/**
 * Initialize a serial port.
 *
 * \param unit  Serial unit to open. Possible values are architecture dependant.
 */
static struct Serial *ser_open(struct KFileSerial *fd, unsigned int unit)
{
	struct Serial *port;

	ASSERT(unit < countof(ser_handles));
	port = &ser_handles[unit];

	ASSERT(!port->is_open);
	DB(port->is_open = true);

	port->unit = unit;

	port->hw = ser_hw_getdesc(unit);

	/* Initialize circular buffers */
	ASSERT(port->hw->txbuffer);
	ASSERT(port->hw->rxbuffer);
	fifo_init(&port->txfifo, port->hw->txbuffer, port->hw->txbuffer_size);
	fifo_init(&port->rxfifo, port->hw->rxbuffer, port->hw->rxbuffer_size);

	port->hw->table->init(port->hw, port);

	fd->ser = port;
	/* Set default values */
#if CONFIG_SER_RXTIMEOUT != -1 || CONFIG_SER_TXTIMEOUT != -1
	ser_settimeouts(fd, CONFIG_SER_RXTIMEOUT, CONFIG_SER_TXTIMEOUT);
#endif
#if CONFIG_SER_DEFBAUDRATE
	ser_setbaudrate(fd, CONFIG_SER_DEFBAUDRATE);
#endif

	/* Clear error flags */
	ser_setstatus(port, 0);

	return port;
}


/**
 * Clean up serial port, disabling the associated hardware.
 */
static int ser_close(struct KFile *fd)
{
	KFileSerial *fds = KFILESERIAL(fd);
	Serial *port = fds->ser;

	ASSERT(port->is_open);
	DB(port->is_open = false);

	// Wait until we finish sending everything
	ser_flush(fd);

	port->hw->table->cleanup(port->hw);
	DB(port->hw = NULL);

	/*
	 * We purge the FIFO buffer only after the low-level cleanup, so that
	 * we are sure that there are no more interrupts.
	 */
	ser_purge(fds);
	return 0;
}

/**
 * Reopen serial port.
 */
static struct KFile *ser_reopen(struct KFile *fd)
{
	KFileSerial *fds = KFILESERIAL(fd);

	ser_close(fd);
	ser_open(fds, fds->ser->unit);
	return (KFile *)fds;
}

/**
 * Init serial driver for \a unit.
 */
void ser_init(struct KFileSerial *fds, unsigned int unit)
{
	memset(fds, 0, sizeof(*fds));

	DB(fds->fd._type = KFT_SERIAL);
	fds->fd.reopen = ser_reopen;
	fds->fd.close = ser_close;
	fds->fd.read = ser_read;
	fds->fd.write = ser_write;
	fds->fd.flush = ser_flush;
	fds->fd.error = ser_error;
	fds->fd.clearerr = ser_clearerr;
	ser_open(fds, unit);
}


/**
 * Read data from SPI bus.
 * Since we are master, we have to trigger slave by sending
 * fake chars on the bus.
 */
static size_t spimaster_read(struct KFile *fd, void *buf, size_t size)
{
	KFileSerial *fd_spi = KFILESERIAL(fd);

	ser_flush(&fd_spi->fd);
	ser_purgeRx(fd_spi);

	for (size_t i = 0; i < size; i++)
		ser_putchar(0, fd_spi->ser);

	return ser_read(&fd_spi->fd, buf, size);
}

/**
 * Write data to SPI bus.
 */
static size_t spimaster_write(struct KFile *fd, const void *buf, size_t size)
{
	KFileSerial *fd_spi = KFILESERIAL(fd);

	ser_purgeRx(fd_spi);

	return ser_write(&fd_spi->fd, buf, size);
}


/**
 * Init SPI serial driver \a unit in master mode.
 *
 * This interface implements the SPI master protocol over a serial SPI
 * driver. This is needed because normal serial driver send/receive data
 * at the same time. SPI slaves like memories and other peripherals
 * first receive and *then* send response back instead.
 * To achieve this, when we are master and we are *sending*,
 * we have to discard all incoming data. Then, when we want to
 * receive, we must write fake data to SPI to trigger slave devices.
 */
void spimaster_init(KFileSerial *fds, unsigned int unit)
{
	ser_init(fds, unit);
	fds->fd.read = spimaster_read;
	fds->fd.write = spimaster_write;
}

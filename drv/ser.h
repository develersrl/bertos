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
 * Copyright 2003,2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2000 Bernardo Innocenti <bernie@codewiz.org>
 *
 * -->
 *
 * \brief High level serial I/O API
 *
 * \version $Id$
 * \author Bernardo Innocenti <bernie@develer.com>
 */

#ifndef DRV_SER_H
#define DRV_SER_H


#include <mware/fifobuf.h>
#include <cfg/compiler.h>

#include CPU_HEADER(ser)

#include <appconfig.h>



/**
 * \name Masks to group TX/RX errors.
 * \{
 */
#define SERRF_RX \
	( SERRF_RXFIFOOVERRUN \
	| SERRF_RXTIMEOUT \
	| SERRF_RXSROVERRUN \
	| SERRF_PARITYERROR \
	| SERRF_FRAMEERROR \
	| SERRF_NOISEERROR)
#define SERRF_TX  (SERRF_TXTIMEOUT)
/*\}*/

/**
 * \name LSB or MSB first data order for SPI driver.
 * \{
 */
#define SER_MSB_FIRST 0
#define SER_LSB_FIRST 1
/*\}*/

/**
 * \name Parity settings for ser_setparity().
 *
 * \note Values are AVR-specific for performance reasons.
 *       Other processors should either decode them or
 *       redefine these macros.
 * \{
 */
#define SER_PARITY_NONE  0
#define SER_PARITY_EVEN  2
#define SER_PARITY_ODD   3
/*\}*/


struct SerialHardware;

/** Human-readable serial error descriptions */
extern const char * const serial_errors[8];

/** Serial handle structure */
typedef struct Serial
{
	/** Physical port number */
	unsigned int unit;

#ifdef _DEBUG
	bool is_open;
#endif

	/**
	 * \name Transmit and receive FIFOs.
	 *
	 * Declared volatile because handled asinchronously by interrupts.
	 *
	 * \{
	 */
	FIFOBuffer txfifo;
	FIFOBuffer rxfifo;
	/* \} */

#if CONFIG_SER_RXTIMEOUT != -1
	ticks_t rxtimeout;
#endif
#if CONFIG_SER_TXTIMEOUT != -1
	ticks_t txtimeout;
#endif

	/** Holds the flags defined above.  Will be 0 when no errors have occurred. */
	volatile serstatus_t status;

	/** Low-level interface to hardware. */
	struct SerialHardware* hw;
} Serial;


/* Function prototypes */
extern int ser_putchar(int c, struct Serial *port);
extern int ser_getchar(struct Serial *port);
extern int ser_getchar_nowait(struct Serial *port);

extern int ser_write(struct Serial *port, const void *buf, size_t len);
extern int ser_read(struct Serial *port, void *buf, size_t size);

extern int ser_print(struct Serial *port, const char *s);
extern int ser_printf(struct Serial *port, const char *format, ...) FORMAT(__printf__, 2, 3);

extern int ser_gets(struct Serial *port, char *buf, int size);
extern int ser_gets_echo(struct Serial *port, char *buf, int size, bool echo);

extern void ser_setbaudrate(struct Serial *port, unsigned long rate);
extern void ser_setparity(struct Serial *port, int parity);
extern void ser_settimeouts(struct Serial *port, mtime_t rxtimeout, mtime_t txtimeout);
extern void ser_resync(struct Serial *port, mtime_t delay);
extern void ser_purge(struct Serial *port);
extern void ser_drain(struct Serial *port);

extern struct Serial *ser_open(unsigned int unit);
extern void ser_close(struct Serial *port);

/**
 * \name Additional functions implemented as macros
 *
 * \{
 */
#define ser_getstatus(h)    ((h)->status)
#define ser_setstatus(h, x) ((h)->status = (x))
#define ser_clearstatus(h)  ser_setstatus(h, 0)
/* \} */

#endif /* DRV_SER_H */

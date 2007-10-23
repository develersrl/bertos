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
 * Copyright 2007 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief ARM debug support (implementation).
 *
 * \version $Id$
 * \author Francesco Sacchi <batt@develer.com>
 */

#include "kdebug_at91.h"
#include <cfg/macros.h> /* for BV() */
#include <appconfig.h>
#include <hw_cpu.h>     /* for CLOCK_FREQ */
#include <hw_ser.h>     /* Required for bus macros overrides */

#include <io/arm.h>

#if CONFIG_KDEBUG_PORT == KDEBUG_PORT_DBGU
	#define KDBG_WAIT_READY()     while (!(DBGU_SR & BV(US_TXRDY))) {}
	#define KDBG_WAIT_TXDONE()    while (!(DBGU_SR & BV(US_TXEMPTY))) {}

	#define KDBG_WRITE_CHAR(c)    do { DBGU_THR = (c); } while(0)

	/* Debug unit is used only for debug purposes so does not generate interrupts. */
	#define KDBG_MASK_IRQ(old)    do { (void)old; } while(0)

	/* Debug unit is used only for debug purposes so does not generate interrupts. */
	#define KDBG_RESTORE_IRQ(old) do { (void)old; } while(0)

	typedef uint32_t kdbg_irqsave_t;

#else
	#error CONFIG_KDEBUG_PORT should be KDEBUG_PORT_DBGU
#endif


INLINE void kdbg_hw_init(void)
{
	#if CONFIG_KDEBUG_PORT == KDEBUG_PORT_DBGU
		/* Disable all DBGU interrupts. */
		DBGU_IDR =  0xFFFFFFFF;
		/* Reset DBGU */
		DBGU_CR =  BV(US_RSTRX) | BV(US_RSTTX) | BV(US_RXDIS) | BV(US_TXDIS);
		/* Set baudrate */
		DBGU_BRGR = (CLOCK_FREQ + (16 * CONFIG_KDEBUG_BAUDRATE) / 2) / (16 * CONFIG_KDEBUG_BAUDRATE);
		/* Set DBGU mode to 8 data bits, no parity and 1 stop bit. */
		DBGU_MR =  US_CHMODE_NORMAL | US_CHRL_8 | US_PAR_NO | US_NBSTOP_1;
		/* Enable DBGU transmitter. */
		DBGU_CR = BV(US_TXEN);
		#if !CPU_ARM_AT91SAM7S256
			#warning Check Debug Unit AT91 pins on datasheet!
		#endif
		/* Disable PIO on DGBU tx pin. */
		PIOA_PDR = BV(10);
		PIOA_ASR = BV(10);
		
		#if 0 /* Disable Rx for now */
		/* Enable DBGU receiver. */
		DBGU_CR = BV(US_RXEN);
		#if !CPU_ARM_AT91SAM7S256
			#warning Check Debug Unit AT91 pins on datasheet!
		#endif
		/* Disable PIO on DGBU rx pin. */
		PIOA_PDR = BV(9);
		PIOA_ASR = BV(9);

		#endif
	#else
		#error CONFIG_KDEBUG_PORT should be KDEBUG_PORT_DBGU
	#endif /* CONFIG_KDEBUG_PORT == KDEBUG_PORT_DBGU */
}

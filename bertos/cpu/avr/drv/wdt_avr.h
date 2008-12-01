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
 * Copyright 2004 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief Watchdog interface for AVR architecture.
 *
 * \version $Id$
 *
 * \author Daniele Basile <asterix@develer.com>
 *
 */

#ifndef DRV_WDT_AVR_H
#define DRV_WDT_AVR_H

#include "cfg/cfg_wdt.h"

#include <cfg/compiler.h> // INLINE

#include <avr/io.h>

#if CPU_AVR_ATMEGA1281
	#define WDT_CRTL_REG WDTCSR
#else
	#define WDT_CRTL_REG WDTCR
#endif

/**
 * Reset the watchdog timer.
 */
INLINE void wdt_reset(void)
{
	__asm__ __volatile__ ("wdr");
}

INLINE void wdt_enable(bool flag)
{
	IRQ_DISABLE;
	if (flag)
	{
		WDT_CRTL_REG |= BV(WDE);
	}
	else
	{
		WDT_CRTL_REG |= BV(WDCE) | BV(WDE);
		WDT_CRTL_REG &= ~BV(WDE);
	}
	IRQ_ENABLE;
}

/**
 * Set watchdog timeout.
 */
INLINE void wdt_setTimeout(uint32_t timeout)
{
	IRQ_DISABLE;
	wdt_reset();
	WDT_CRTL_REG |= BV(WDCE) | BV(WDE);
	WDT_CRTL_REG = timeout;
	IRQ_ENABLE;
}

/**
 * Init watchdog timer.
 */
INLINE void wdt_init(void)
{
	WDT_CRTL_REG |= BV(WDCE) | BV(WDE);
}


#endif /* DRV_WDT_AVR_H */

/*!
 * \file
 * <!--
 * Copyright 2004 Develer S.r.l. (http://www.develer.com/)
 * This file is part of DevLib - See devlib/README for information.
 * -->
 *
 * \version $Id$
 *
 * \author Bernardo Innocenti <bernie@develer.com>
 *
 * \brief Watchdog interface
 */

/*#*
 *#* $Log$
 *#* Revision 1.1  2004/10/26 08:34:47  bernie
 *#* New DevLib module.
 *#*
 *#*/
#ifndef DRV_WDT_H
#define DRV_WDT_H

#include <avr/io.h>
#include <compiler.h>
#include <cpu.h>
#include <macros.h> // BV()

/*!
 * Reset the watchdog timer.
 */
INLINE void wdt_reset(void)
{
	__asm__ __volatile__ ("wdr");
}

/*!
 * Set watchdog timer timeout.
 *
 * \param timeout  0: 16.3ms, 7: 2.1s
 */
INLINE void wdt_init(uint8_t timeout)
{
#if CPU_AVR
	WDTCR |= BV(WDCE) | BV(WDE);
	WDTCR = timeout;
#else
	#error unknown CPU
#endif
}

INLINE void wdt_start(void)
{
#if CPU_AVR
	WDTCR |= BV(WDE);
#else
	#error unknown CPU
#endif
}

INLINE void wdt_stop(void)
{
#if CPU_AVR
	WDTCR |= BV(WDCE) | BV(WDE);
	WDTCR &= ~BV(WDE);
#else
	#error unknown CPU
#endif
}

#endif /* DRV_WDT_H */

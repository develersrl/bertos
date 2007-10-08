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
 * \version $Id$
 *
 * \author Bernardo Innocenti <bernie@develer.com>
 *
 * \brief Watchdog interface
 */

/*#*
 *#* $Log$
 *#* Revision 1.12  2007/06/07 14:35:12  batt
 *#* Merge from project_ks.
 *#*
 *#* Revision 1.11  2006/07/19 12:56:26  bernie
 *#* Convert to new Doxygen style.
 *#*
 *#* Revision 1.10  2006/05/18 00:38:42  bernie
 *#* Work around missing ARCH_FREERTOS symbol.
 *#*
 *#* Revision 1.9  2006/03/22 09:49:09  bernie
 *#* Add FreeRTOS support.
 *#*
 *#* Revision 1.8  2006/02/20 02:02:29  bernie
 *#* Port to Qt 4.1.
 *#*
 *#* Revision 1.7  2005/11/27 03:58:40  bernie
 *#* Add POSIX timer emulator.
 *#*
 *#* Revision 1.6  2005/11/27 03:03:08  bernie
 *#* Add Qt support hack.
 *#*
 *#* Revision 1.5  2005/11/04 16:20:02  bernie
 *#* Fix reference to README.devlib in header.
 *#*
 *#* Revision 1.4  2005/04/12 01:37:17  bernie
 *#* Prevent warning when watchdog is disabled.
 *#*
 *#* Revision 1.3  2005/04/11 19:10:28  bernie
 *#* Include top-level headers from cfg/ subdir.
 *#*
 *#* Revision 1.2  2004/11/16 21:02:07  bernie
 *#* Make driver optional; mark AVR specific parts as such.
 *#*
 *#* Revision 1.1  2004/10/26 08:34:47  bernie
 *#* New DevLib module.
 *#*
 *#*/
#ifndef DRV_WDT_H
#define DRV_WDT_H

#include <appconfig.h>
#include <cfg/compiler.h> // INLINE
#include <cfg/arch_config.h>

/* Configury sanity check */
#if !defined(CONFIG_WATCHDOG) || (CONFIG_WATCHDOG != 0 && CONFIG_WATCHDOG != 1)
	#error CONFIG_WATCHDOG must be defined to either 0 or 1
#endif

#if CONFIG_WATCHDOG
	#include <cfg/cpu.h>
	#include <cfg/os.h>

	#if OS_QT
		#if _QT < 4
			#include <qapplication.h>
		#else
			#include <QtGui/QApplication>
		#endif
	#elif OS_POSIX
		#include <sys/select.h>
	#elif CPU_AVR
		#include <avr/io.h>
		#include <cfg/macros.h> // BV()
		#if CPU_AVR_ATMEGA1281  // Name is different in atmega1281
			#define WDTCR WDTCSR
		#endif
	#elif defined(ARCH_FREERTOS) && (ARCH & ARCH_FREERTOS)
		#include <task.h> /* taskYIELD() */
	#else
		#error unknown CPU
	#endif
#endif /* CONFIG_WATCHDOG */

/**
 * Reset the watchdog timer.
 */
INLINE void wdt_reset(void)
{
#if CONFIG_WATCHDOG
	#if OS_QT
		// Let Qt handle events
		ASSERT(qApp);
		qApp->processEvents();
	#elif OS_POSIX
		static struct timeval tv = { 0, 0 };
		select(0, NULL, NULL, NULL, &tv);
	#elif defined(ARCH_FREERTOS) && (ARCH & ARCH_FREERTOS)
		vTaskDelay(1);
	#elif CPU_AVR
		__asm__ __volatile__ ("wdr");
	#else
		#error unknown CPU
	#endif
#endif /* CONFIG_WATCHDOG */
}

/**
 * Set watchdog timer timeout.
 *
 * \param timeout  0: 16.3ms, 7: 2.1s
 */
INLINE void wdt_init(uint8_t timeout)
{
#if CONFIG_WATCHDOG
	#if OS_QT
		// Create a dummy QApplication object
		if (!qApp)
		{
			int argc;
			new QApplication(argc, (char **)NULL);
		}
		(void)timeout;
	#elif OS_POSIX
		(void)timeout; // NOP
	#elif defined(ARCH_FREERTOS) && (ARCH & ARCH_FREERTOS)
		/* nop */
	#elif CPU_AVR
		WDTCR |= BV(WDCE) | BV(WDE);
		WDTCR = timeout;
	#else
		#error unknown CPU
	#endif
#else
	(void)timeout;
#endif /* CONFIG_WATCHDOG */
}

INLINE void wdt_start(void)
{
#if CONFIG_WATCHDOG
	#if OS_QT
		// NOP
	#elif OS_POSIX
		// NOP
	#elif defined(ARCH_FREERTOS) && (ARCH & ARCH_FREERTOS)
		/* nop */
	#elif CPU_AVR
		WDTCR |= BV(WDE);
	#else
		#error unknown CPU
	#endif
#endif /* CONFIG_WATCHDOG */
}

INLINE void wdt_stop(void)
{
#if CONFIG_WATCHDOG
	#if OS_QT
		// NOP
	#elif OS_POSIX
		// NOP
	#elif defined(ARCH_FREERTOS) && (ARCH & ARCH_FREERTOS)
		/* nop */
	#elif CPU_AVR
		WDTCR |= BV(WDCE) | BV(WDE);
		WDTCR &= ~BV(WDE);
	#else
		#error unknown CPU
	#endif
#endif /* CONFIG_WATCHDOG */
}

#endif /* DRV_WDT_H */

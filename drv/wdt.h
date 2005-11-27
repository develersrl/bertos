/*!
 * \file
 * <!--
 * Copyright 2004 Develer S.r.l. (http://www.develer.com/)
 * This file is part of DevLib - See README.devlib for information.
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

/* Configury sanity check */
#if !defined(CONFIG_WATCHDOG) || (CONFIG_WATCHDOG != 0 && CONFIG_WATCHDOG != 1)
	#error CONFIG_WATCHDOG must be defined to either 0 or 1
#endif

#if CONFIG_WATCHDOG
	#include <cfg/cpu.h>
	#include <cfg/os.h>

	#if OS_QT
		#include <qapplication.h>
	#elif OS_POSIX
		#include <sys/select.h>
	#elif CPU_AVR
		#include <avr/io.h>
		#include <cfg/macros.h> // BV()
	#else
		#error unknown CPU
	#endif
#endif /* CONFIG_WATCHDOG */

/*!
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
	#elif CPU_AVR
		__asm__ __volatile__ ("wdr");
	#else
		#error unknown CPU
	#endif
#endif /* CONFIG_WATCHDOG */
}

/*!
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
	#elif CPU_AVR
		WDTCR |= BV(WDCE) | BV(WDE);
		WDTCR &= ~BV(WDE);
	#else
		#error unknown CPU
	#endif
#endif /* CONFIG_WATCHDOG */
}

#endif /* DRV_WDT_H */

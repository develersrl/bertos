/**
 * \file
 * <!--
 * Copyright 2004 Develer S.r.l. (http://www.develer.com/)
 * All Rights Reserved.
 * -->
 *
 * \brief Some simple delay routines.
 *
 * Simple serial driver
 * \version $Id$
 * \author Francesco Sacchi <batt@develer.com>
 */

/*#*
 *#* $Log$
 *#* Revision 1.2  2006/07/19 12:56:26  bernie
 *#* Convert to new Doxygen style.
 *#*
 *#* Revision 1.1  2005/04/12 01:37:50  bernie
 *#* Import into DevLib.
 *#*
 *#* Revision 1.8  2005/04/12 01:18:09  bernie
 *#* time_t -> mtime_t.
 *#*
 *#* Revision 1.7  2005/03/20 04:18:41  bernie
 *#* Fixes for CONFIG_WATCHDOG == 0.
 *#*
 *#* Revision 1.6  2004/10/27 09:38:07  aleph
 *#* Bootloader working with watchdog enabled
 *#*
 *#* Revision 1.5  2004/10/20 10:00:37  customer_pw
 *#* Add newline at eof
 *#*
 *#* Revision 1.4  2004/10/14 14:13:09  batt
 *#* Add comment.
 *#*
 *#* Revision 1.3  2004/10/14 13:29:20  batt
 *#* Fix 0ms delay bug.
 *#*
 *#* Revision 1.2  2004/10/13 17:53:05  batt
 *#* Delay with hw timer.
 *#*
 *#* Revision 1.1  2004/10/13 16:36:32  batt
 *#* Simplified timer delay routines.
 *#*
 *#*/
#include "hw.h"
#include "timer_simple.h"
#include <drv/wdt.h>
#include <compiler.h>
#include <cpu.h>
#include <macros.h> /* BV() */

#include <avr/io.h>


#define MS_PER_SEC       1000UL
#define TIMER_PRESCALER  64UL
#define TIMER_DELAY_1MS  (255 - CLOCK_FREQ / TIMER_PRESCALER / MS_PER_SEC)

/**
 * Wait \a time ms using timer 0.
 *
 */
void timer_delay(mtime_t time)
{
	/* Set timer clock to clock_freq/64 */
	TCCR0 = BV(CS02);

	while (time--)
	{
		/* Initialize timer counter register */
		TCNT0 = TIMER_DELAY_1MS;
		/* Clear overflow bit. */
		TIFR |= BV(TOV0);
		/* Wait overflow. */
		while (!(TIFR & BV(TOV0)));
#if CONFIG_WATCHDOG
		wdt_reset();
#endif
	}
}

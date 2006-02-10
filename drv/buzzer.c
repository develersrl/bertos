/*!
 * \file
 * <!--
 * Copyright 2003, 2004, 2005 Develer S.r.l. (http://www.develer.com/)
 * Copyright 1999, 2003 Bernardo Innocenti <bernie@develer.com>
 * This file is part of DevLib - See README.devlib for information.
 * -->
 *
 * \version $Id$
 *
 * \brief Buzzer driver (implementation)
 *
 * \version $Id$
 * \author Bernardo Innocenti <bernie@develer.com>
 * \author Francesco Sacchi <batt@develer.com>
 */

/*#*
 *#* $Log$
 *#* Revision 1.17  2006/02/10 12:30:18  bernie
 *#* Push interrupt protection inside hw module.
 *#*
 *#* Revision 1.16  2005/11/04 16:19:33  bernie
 *#* buz_init(): Restore IRQ protection as in project_bko.
 *#*
 *#* Revision 1.15  2005/06/27 21:25:50  bernie
 *#* Modularize hardware access; Port to new timer interface.
 *#*
 *#* Revision 1.14  2005/04/11 19:10:27  bernie
 *#* Include top-level headers from cfg/ subdir.
 *#*
 *#* Revision 1.13  2005/02/18 11:20:15  bernie
 *#* Use mware/event.h; Update copyright info.
 *#*
 *#* Revision 1.12  2004/12/13 12:07:06  bernie
 *#* DISABLE_IRQSAVE/ENABLE_IRQRESTORE: Convert to IRQ_SAVE_DISABLE/IRQ_RESTORE.
 *#*
 *#* Revision 1.11  2004/12/08 09:11:53  bernie
 *#* Rename time_t to mtime_t.
 *#*
 *#* Revision 1.10  2004/10/03 18:38:51  bernie
 *#* Add missing AVR header; Fix header.
 *#*
 *#* Revision 1.9  2004/09/14 21:01:25  bernie
 *#* Use new AVR port pin names.
 *#*/

#include "buzzer.h"

#include <hw_buzzer.h>
#include <drv/timer.h>

#include <mware/event.h>

#include <cfg/debug.h>


/* Local vars */
static Timer buz_timer;
static bool buz_timer_running;
static mtime_t buz_repeat_interval;
static mtime_t buz_repeat_duration;


/*!
 * Turn off buzzer, called by software timer
 */
static void buz_softint(void)
{
	if (IS_BUZZER_ON)
	{
		BUZZER_OFF;
		if (buz_repeat_interval)
		{
			/* Wait for interval time */
			timer_setDelay(&buz_timer, ms_to_ticks(buz_repeat_interval));
			timer_add(&buz_timer);
		}
		else
			buz_timer_running = false;
	}
	else if (buz_repeat_interval)
	{
		/* Wait for beep time */
		BUZZER_ON;
		timer_setDelay(&buz_timer, ms_to_ticks(buz_repeat_duration));
		timer_add(&buz_timer);
	}
	else
		buz_timer_running = false;
}


/*!
 * Beep for the specified ms time
 */
void buz_beep(mtime_t time)
{
	cpuflags_t flags;
	IRQ_SAVE_DISABLE(flags);

	/* Remove the software interrupt if it was already queued */
	if (buz_timer_running)
		timer_abort(&buz_timer);

	/* Turn on buzzer */
	BUZZER_ON;

	/* Add software interrupt to turn the buzzer off later */
	buz_timer_running = true;
	timer_setDelay(&buz_timer, ms_to_ticks(time));
	timer_add(&buz_timer);

	IRQ_RESTORE(flags);
}


/*!
 * Start buzzer repetition
 */
void buz_repeat_start(mtime_t duration, mtime_t interval)
{
	buz_repeat_interval = interval;
	buz_repeat_duration = duration;
	buz_beep(duration);
}


/*!
 * Stop buzzer repetition
 */
void buz_repeat_stop(void)
{
	cpuflags_t flags;
	IRQ_SAVE_DISABLE(flags);

	/* Remove the software interrupt if it was already queued */
	if (buz_timer_running)
	{
		timer_abort(&buz_timer);
		buz_timer_running = false;
	}

	buz_repeat_interval = 0;
	BUZZER_OFF;

	IRQ_RESTORE(flags);
}


/*!
 * Initialize buzzer.
 */
void buz_init(void)
{
	BUZZER_HW_INIT;

	/* Init software interrupt. */
	timer_set_event_softint(&buz_timer, (Hook)buz_softint, 0);
}

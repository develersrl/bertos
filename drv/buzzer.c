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
 */

/*#*
 *#* $Log$
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

#include <drv/timer.h>
#include <mware/event.h>

#include <macros.h> /* BV() */
#include <debug.h>
#include <hw.h>
#include <arch_config.h>


#if (ARCH & ARCH_EMUL)

	int Emul_IsBuzzerOn(void);
	void Emul_BuzzerOn(void);
	void Emul_BuzzerOff(void);
	void Emul_BuzzerInit(void);

	#define IS_BUZZER_ON  (Emul_IsBuzzerOn())
	#define BUZZER_ON     (Emul_BuzzerOn())
	#define BUZZER_OFF    (Emul_BuzzerOff())
	#define BUZZER_INIT   (Emul_BuzzerInit())

#elif defined(__AVR__)

	#include <avr/io.h>

	#define IS_BUZZER_ON  (PORTG & BV(PG0))

	/*!
	 * \name Buzzer manipulation macros.
	 *
	 * \note Some PORTG functions are being used from
	 *       interrupt code, so we must be careful to
	 *       avoid race conditions.
	 * \{
	 */
	#define BUZZER_ON ATOMIC(PORTG |= BV(PG0))
	#define BUZZER_OFF ATOMIC(PORTG &= ~BV(PG0))
	#define BUZZER_INIT ATOMIC(PORTG &= ~BV(PG0); DDRG |= BV(PG0);)
	/*\}*/

#elif defined(__IAR_SYSTEMS_ICC) || defined(__IAR_SYSTEMS_ICC__) /* 80C196 */

	#define IS_BUZZER_ON  (cpld->Buzzer & 1)
	#define BUZZER_ON     (cpld->Buzzer = 1)
	#define BUZZER_OFF    (cpld->Buzzer = 0)
	#define BUZZER_INIT   (cpld->Buzzer = 0)

#endif /* ARCH, __AVR__, __IAR_SYSTEM_ICC */


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
			buz_timer.delay = buz_repeat_interval;
			timer_add(&buz_timer);
		}
		else
			buz_timer_running = false;
	}
	else if (buz_repeat_interval)
	{
		/* Wait for beep time */
		BUZZER_ON;
		buz_timer.delay = buz_repeat_duration;
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
	buz_timer.delay = time;
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
 * Initialize buzzer
 */
void buz_init(void)
{
	BUZZER_INIT;

	/* Inizializza software interrupt */
	event_initSoftInt(&buz_timer.expire, (Hook)buz_softint, 0);
}

/*!
 * \file
 * <!--
 * Copyright 2003,2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 1999,2003 Bernardo Innocenti <bernie@develer.com>
 * This file is part of DevLib - See devlib/README for information.
 * -->
 *
 * \version $Id$
 *
 * \author Bernardo Innocenti <bernie@develer.com>
 *
 * \brief Buzzer driver
 */

/*
 * $Log$
 * Revision 1.3  2004/06/03 11:27:09  bernie
 * Add dual-license information.
 *
 * Revision 1.2  2004/05/23 18:21:53  bernie
 * Trim CVS logs and cleanup header info.
 *
 */

#include "hw.h"
#include "kdebug.h"
#include "timer.h"
#include "buzzer.h"
#include <kern/event.h>


#if (ARCH & ARCH_EMUL)

	int Emul_IsBuzzerOn(void);
	void Emul_BuzzerOn(void);
	void Emul_BuzzerOff(void);
	void Emul_BuzzerInit(void);

#	define IS_BUZZER_ON  (Emul_IsBuzzerOn())
#	define BUZZER_ON     (Emul_BuzzerOn())
#	define BUZZER_OFF    (Emul_BuzzerOff())
#	define BUZZER_INIT   (Emul_BuzzerInit())

#elif defined(__AVR__)

#	define IS_BUZZER_ON  (PORTG & BV(PORTG0))

	/**
	 * Buzzer manipulation macros
	 *
	 * \note Some PORTG functions are being used from
	 *       interrupt code, so we must be careful to
	 *       avoid race conditions.
	 */
#	define BUZZER_ON \
	do { \
		cpuflags_t _flags; \
		DISABLE_IRQSAVE(_flags); \
		PORTG |= BV(PORTG0); \
		ENABLE_IRQRESTORE(_flags); \
	} while (0)

#	define BUZZER_OFF \
	do { \
		cpuflags_t _flags; \
		DISABLE_IRQSAVE(_flags); \
		PORTG &= ~BV(PORTG0); \
		ENABLE_IRQRESTORE(_flags); \
	} while (0)

#	define BUZZER_INIT \
	do { \
		cpuflags_t _flags; \
		DISABLE_IRQSAVE(_flags); \
		PORTG &= ~BV(PORTG0); \
		DDRG |= BV(PORTG0); \
		ENABLE_IRQRESTORE(_flags); \
	} while (0)

#elif defined(__IAR_SYSTEMS_ICC) || defined(__IAR_SYSTEMS_ICC__) /* 80C196 */

#	define IS_BUZZER_ON  (cpld->Buzzer & 1)
#	define BUZZER_ON     (cpld->Buzzer = 1)
#	define BUZZER_OFF    (cpld->Buzzer = 0)
#	define BUZZER_INIT   (cpld->Buzzer = 0)

#endif /* ARCH, __AVR__, __IAR_SYSTEM_ICC */


/* Local vars */
static Timer *buz_timer;
static bool buz_timer_running;
static time_t buz_repeat_interval;
static time_t buz_repeat_duration;


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
			buz_timer->delay = buz_repeat_interval;
			timer_add(buz_timer);
		}
		else
			buz_timer_running = false;
	}
	else if (buz_repeat_interval)
	{
		/* Wait for beep time */
		BUZZER_ON;
		buz_timer->delay = buz_repeat_duration;
		timer_add(buz_timer);
	}
	else
		buz_timer_running = false;
}


/*!
 * Beep for the specified ms time
 */
void buz_beep(time_t time)
{
	cpuflags_t flags;

	/* Remove the software interrupt if it was already queued */
	DISABLE_IRQSAVE(flags);
	if (buz_timer_running)
		timer_abort(buz_timer);

	/* Turn on buzzer */
	BUZZER_ON;

	/* Add software interrupt to turn the buzzer off later */
	buz_timer_running = true;
	buz_timer->delay = time;
	timer_add(buz_timer);

	ENABLE_IRQRESTORE(flags);
}


/*!
 * Start buzzer repetition
 */
void buz_repeat_start(time_t duration, time_t interval)
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
	DISABLE_IRQSAVE(flags);

	/* Remove the software interrupt if it was already queued */
	if (buz_timer_running)
	{
		timer_abort(buz_timer);
		buz_timer_running = false;
	}

	buz_repeat_interval = 0;
	BUZZER_OFF;

	ENABLE_IRQRESTORE(flags);
}


/*!
 * Initialize buzzer
 */
void buz_init(void)
{
	BUZZER_INIT;

	/* Inizializza software interrupt */
	buz_timer = timer_new();
	ASSERT(buz_timer != NULL);
	INITEVENT_INT(&buz_timer->expire, (Hook)buz_softint, 0);
}

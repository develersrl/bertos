/*!
 * \file
 * <!--
 * Copyright 2003, 2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2000 Bernardo Innocenti <bernie@develer.com>
 * This file is part of DevLib - See devlib/README for information.
 * -->
 *
 * \version $Id$
 *
 * \author Bernardo Innocenti <bernie@develer.com>
 *
 * \brief Hardware independent timer driver (interface)
 */

/*#*
 *#* $Log$
 *#* Revision 1.18  2004/11/16 23:09:52  bernie
 *#* Disable timer_minutes() for targets with 16bit time_t.
 *#*
 *#* Revision 1.17  2004/11/16 22:37:14  bernie
 *#* Replace IPTR with iptr_t.
 *#*
 *#* Revision 1.16  2004/08/25 14:12:08  rasky
 *#* Aggiornato il comment block dei log RCS
 *#*
 *#* Revision 1.15  2004/08/10 06:59:09  bernie
 *#* timer_gettick(): Rename to timer_ticks() and add backwards compatibility inline.
 *#*
 *#* Revision 1.12  2004/07/30 14:34:10  rasky
 *#* Vari fix per documentazione e commenti
 *#* Aggiunte PP_CATn e STATIC_ASSERT
 *#*
 *#* Revision 1.11  2004/07/29 22:40:12  bernie
 *#* Spelling fix.
 *#*
 *#* Revision 1.10  2004/07/21 00:13:57  bernie
 *#* Put timer driver on diet.
 *#*
 *#* Revision 1.9  2004/07/20 23:45:01  bernie
 *#* Finally remove redundant protos.
 *#*
 *#* Revision 1.8  2004/07/18 21:57:32  bernie
 *#* timer_gettick(): Rename to timer_tick() and document better.
 *#*
 *#* Revision 1.7  2004/06/27 15:26:17  aleph
 *#* Declaration fix for build with GCC 3.4
 *#*
 *#* Revision 1.6  2004/06/07 18:10:06  aleph
 *#* Remove free pool of timers; use user-provided Timer structure instead
 *#*
 *#* Revision 1.5  2004/06/07 15:57:12  aleph
 *#* Add function prototypes
 *#*
 *#* Revision 1.4  2004/06/06 18:25:44  bernie
 *#* Rename event macros to look like regular functions.
 *#*
 *#* Revision 1.3  2004/06/06 16:57:18  bernie
 *#* Mark some functions INLINE instead of 'extern inline'.
 *#*
 *#* Revision 1.2  2004/06/03 11:27:09  bernie
 *#* Add dual-license information.
 *#*
 *#* Revision 1.1  2004/05/23 18:23:30  bernie
 *#* Import drv/timer module.
 *#*
 *#*/
#ifndef DRV_TIMER_H
#define DRV_TIMER_H

#include "cpu.h"
#include "compiler.h"
#include <config.h>
#include <mware/list.h>

/*! Number of timer ticks per second. */
#define TICKS_PER_SEC  ((time_t)1000)

/* Function protos */
extern void timer_init(void);
extern void timer_delay(time_t time);

#ifndef CONFIG_TIMER_DISABLE_UDELAY
extern void timer_udelay(utime_t utime);
#endif


#ifndef CONFIG_TIMER_DISABLE_EVENTS

#if CONFIG_KERNEL
	#include <kern/event.h>
#else
	#include <mware/event.h>
#endif

/*!
 * The timer driver supports multiple synchronous timers
 * that can trigger an event when they expire.
 *
 * \sa timer_add()
 * \sa timer_abort()
 */
typedef struct Timer
{
	Node   link;      /*!< Link into timers queue */
	time_t delay;     /*!< Timer delay in ms */
	time_t tick;      /*!< Timer will expire at this tick */
	Event  expire;    /*!< Event to execute when the timer expires */
} Timer;

extern void timer_add(Timer *timer);
extern Timer *timer_abort(Timer *timer);

#if defined(CONFIG_KERN_SIGNALS) && CONFIG_KERN_SIGNALS

/*! Set the timer so that it sends a signal when it expires */
INLINE void timer_set_event_signal(Timer* timer, struct Process* proc, sigset_t sigs)
{
	event_initSignal(&timer->expire, proc, sigs);
}

#endif /* CONFIG_KERN_SIGNALS */

/*! Set the timer so that it calls an user hook when it expires */
INLINE void timer_set_event_softint(Timer* timer, Hook func, void* user_data)
{
	event_initSoftInt(&timer->expire, func, user_data);
}

/*! Set the timer delay (the time before the event will be triggered) */
INLINE void timer_set_delay(Timer* timer, time_t delay)
{
	timer->delay = delay;
}

#endif /* CONFIG_TIMER_DISABLE_EVENTS */

extern volatile time_t _clock;

/*!
 * \brief Return the system tick counter (expressed in ms)
 *
 * The result is guaranteed to increment monotonically,
 * but client code must be tolerant with respect to overflows.
 *
 * The following code is safe:
 *
 * \code
 *   time_t tea_start_time = get_tick();
 *
 *   boil_water();
 *
 *   if (get_tick() - tea_start_time > TEAPOT_DELAY)
 *       printf("Your tea, Sir.\n");
 * \endcode
 *
 * When the tick counter increments every millisecond and time_t
 * is 32bit wide, the tick count will overflow every 49.7 days.
 *
 * \note This function must disable interrupts on 8/16bit CPUs because the
 * clock variable is larger than the processor word size and can't
 * be copied atomically.
 */
INLINE time_t timer_ticks(void)
{
	time_t result;
	cpuflags_t flags;

	DISABLE_IRQSAVE(flags);
	result = _clock;
	ENABLE_IRQRESTORE(flags);

	return result;
}


/*!
 * Faster version of timer_ticks(), to be called only when the timer
 * interrupt is disabled (DISABLE_INTS) or overridden by a
 * higher-priority or non-nesting interrupt.
 *
 * \sa timer_ticks
 */
INLINE time_t timer_ticks_unlocked(void)
{
	return _clock;
}


/*
 * timer_minutes() makes no sense when time_t is 16bit because
 * it overflows every 65.536 seconds.
 */
#if SIZEOF_TIME_T >= 4

/*!
 * Return the minutes passed since timer start.
 *
 * The minutes uptime is computed directly from system tick counter:
 * in case of a 4 bytes time_t after 71582 minutes the value will
 * wrap around.
 */
INLINE time_t timer_minutes(void)
{
	return timer_ticks() / (TICKS_PER_SEC * 60);
}

#endif /* SIZEOF_TIME_T >= 4 */

#endif /* DRV_TIMER_H */


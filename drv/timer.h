/*!
 * \file
 * <!--
 * Copyright 2003,2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2000 Bernardo Innocenti <bernie@develer.com>
 * All Rights Reserved.
 * -->
 *
 * \version $Id$
 *
 * \author Bernardo Innocenti <bernie@develer.com>
 *
 * \brief Hardware independent timer driver (interface)
 */

/*
 * $Log$
 * Revision 1.1  2004/05/23 18:23:30  bernie
 * Import drv/timer module.
 *
 */
#ifndef DRV_TIMER_H
#define DRV_TIMER_H

#include "cpu.h"
#include "compiler.h"
#include <mware/list.h>
#include <kern/event.h>

/*! Number of timer ticks per second. */
#define TICKS_PER_SEC       1000

typedef struct Timer
{
	Node   link;      /*!< Link into timers queue */
	time_t delay;     /*!< Timer delay in ms */
	time_t tick;      /*!< Timer will expire at this tick */
	Event  expire;    /*!< Event to execute when the timer expires */
} Timer;

extern void timer_init(void);
extern Timer *timer_new(void);
extern void timer_delete(Timer *timer);
extern void timer_add(Timer *timer);
extern Timer *timer_abort(Timer *timer);
extern void timer_delay(time_t time);
extern void timer_udelay(utime_t utime);
extern inline time_t timer_gettick(void);
extern inline time_t timer_gettick_irq(void);

#ifdef CONFIG_KERN_SIGNALS
/*! Set the timer so that it sends a signal when it expires */
INLINE void timer_set_event_signal(Timer* timer, struct Process* proc, sigset_t sigs)
{
	INITEVENT_SIG(&timer->expire, proc, sigs);
}
#endif

/*! Set the timer so that it calls an user hook when it expires */
INLINE void timer_set_event_softint(Timer* timer, Hook func, void* user_data)
{
	INITEVENT_INT(&timer->expire, func, user_data);
}

/*! Set the timer delay (the time before the event will be triggered) */
INLINE void timer_set_delay(Timer* timer, time_t delay)
{
	timer->delay = delay;
}


/*!
 * Return the system tick counter (expressed in ms)
 * This function must disable interrupts on 8/16bit CPUs because the
 * clock variable is larger than the processor word size and can't
 * be copied atomically.
 */
extern inline time_t timer_gettick(void)
{
	extern volatile time_t _clock;

	time_t result;
	cpuflags_t flags;

	DISABLE_IRQSAVE(flags);
	result = _clock;
	ENABLE_IRQRESTORE(flags);

	return result;
}


/*!
 * Like \c timer_gettick, faster version to be called
 * from interrupt context only.
 */
extern inline time_t timer_gettick_irq(void)
{
	extern volatile time_t _clock;

	return _clock;
}

#endif /* DRV_TIMER_H */


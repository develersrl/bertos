/*!
 * \file
 * <!--
 * Copyright 2003,2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2000 Bernardo Innocenti <bernie@develer.com>
 * This file is part of DevLib - See devlib/README for information.
 * -->
 *
 * \version $Id$
 *
 * \author Bernardo Innocenti <bernie@develer.com>
 *
 * \brief Hardware independent timer driver (implementation)
 */

/*
 * $Log$
 * Revision 1.8  2004/07/18 21:57:07  bernie
 * Fix preprocessor warning with potentially undefined symbol.
 *
 * Revision 1.6  2004/06/07 18:10:06  aleph
 * Remove free pool of timers; use user-provided Timer structure instead
 *
 * Revision 1.5  2004/06/07 15:56:55  aleph
 * Some tabs cleanup and add timer strobe macros
 *
 * Revision 1.4  2004/06/06 18:25:44  bernie
 * Rename event macros to look like regular functions.
 *
 * Revision 1.3  2004/06/06 17:18:42  bernie
 * Fix \!CONFIG_KERN_SIGNALS code paths.
 *
 * Revision 1.2  2004/06/03 11:27:09  bernie
 * Add dual-license information.
 *
 * Revision 1.1  2004/05/23 18:23:30  bernie
 * Import drv/timer module.
 *
 */

#include "hw.h"
#include "kdebug.h"
#include "timer.h"

#if defined(CONFIG_KERN_SIGNALS) && CONFIG_KERN_SIGNALS
	#include <kern/proc.h>
#endif

#if defined(ARCH_EMUL) && (ARCH & ARCH_EMUL)
	#error To be recoded
#elif defined(__AVR__)
	#include "timer_avr.h"
#elif defined(__IAR_SYSTEMS_ICC) || defined(__IAR_SYSTEMS_ICC__) /* 80C196 */
	#include "timer_i196.h"
#elif defined (__m56800__)
	#include "timer_dsp56k.h"
#else
	#error Unknown system
#endif


//! Master system clock (1ms accuracy)
volatile time_t _clock;

REGISTER static List timers_queue;      /*!< Active timers */


/*!
 * Add the specified timer to the software timer service queue.
 * When the delay indicated by the timer expires, the timer
 * device will execute the event associated with it.
 *
 * \note Interrupt safe
 */
void timer_add(Timer *timer)
{
	Timer *node;
	cpuflags_t flags;

	DISABLE_IRQSAVE(flags);

	/* Calculate expiration time for this timer */
	timer->tick = _clock + timer->delay;

	/* Search for the first node whose expiration time is
	 * greater than the timer we want to add.
	 */
	node = (Timer *)timers_queue.head;
	while (node->link.succ)
	{
		/* Stop just after the insert point */
		if (node->tick > timer->tick)
			break;

		/* Go to next node */
		node = (Timer *)node->link.succ;
	}

	/* Enqueue timer request into the list */
	INSERTBEFORE((Node *)timer, (Node *)node);

	ENABLE_IRQRESTORE(flags);
}


/*!
 * Remove a timer from the timer queue before it has expired
 */
Timer *timer_abort(Timer *timer)
{
	cpuflags_t flags;
	DISABLE_IRQSAVE(flags);
	REMOVE((Node *)timer);
	ENABLE_IRQRESTORE(flags);

	return timer;
}


/*!
 * Wait for the specified amount of time (expressed in ms)
 */
void timer_delay(time_t time)
{
#if defined(CONFIG_KERN_SIGNALS) && CONFIG_KERN_SIGNALS
	Timer t;

	ASSERT(!sig_check(SIG_SINGLE));
	timer_set_event_signal(&t, proc_current(), SIG_SINGLE);
	timer_set_delay(&t, time);
	timer_add(&t);
	sig_wait(SIG_SINGLE);

#else /* !CONFIG_KERN_SIGNALS */

	time_t start = timer_gettick();

	/* Busy wait */
	while (timer_gettick() - start < time) { /* nop */ }

#endif /* !CONFIG_KERN_SIGNALS */
}


/*!
 * Wait for the specified amount of time (expressed in microseconds)
 *
 * \bug In AVR arch the maximum amount of time that can be used as
 *      delay could be very limited, depending on the hardware timer
 *      used. Check timer_avr.h, and what register is used as hptime_t.
 */
void timer_udelay(utime_t usec_delay)
{
	if (usec_delay > 1000)
	{
		timer_delay(usec_delay / 1000);
		usec_delay %= 1000;
	}

	// FIXME: This multiplication is too slow at run-time. We should try and move it
	//  to compile-time by exposing the TIMER_HW_HPTICKS_PER_SEC in the header
	//  file.
	hptime_t start = timer_hw_hpread();
	hptime_t delay = (uint32_t)usec_delay * TIMER_HW_HPTICKS_PER_SEC / 1000000ul;

	while (timer_hw_hpread() - start < delay)
	{}
}


/*!
 * Timer interrupt handler. Find soft timers expired and
 * trigger corresponding events.
 */
DEFINE_TIMER_ISR
{
	/* With the Metrowerks compiler, the only way to force the compiler generate
	   an interrupt service routine is to put a pragma directive within the function
	   body. */
	#ifdef __MWERKS__
	#pragma interrupt saveall
	#endif

	Timer *timer;

	TIMER_STROBE_ON;

	timer_hw_irq();

	/* Update the master ms counter */
	++_clock;

	/*
	 * Check the first timer request in the list and process
	 * it when it has expired. Repeat this check until the
	 * first node has not yet expired. Since the list is sorted
	 * by expiry time, all the following requests are guaranteed
	 * to expire later.
	 */
	while ((timer = (Timer *)timers_queue.head)->link.succ)
	{
		/* This request in list has not yet expired? */
		if (_clock < timer->tick)
			break;

		/* Retreat the expired timer */
		REMOVE((Node *)timer);

		/* Execute the associated event */
		event_doIntr(&timer->expire);
	}

	TIMER_STROBE_OFF;
}


/*!
 * Initialize timer
 */
void timer_init(void)
{
	TIMER_STROBE_INIT;

	INITLIST(&timers_queue);

	_clock = 0;

	timer_hw_init();
}

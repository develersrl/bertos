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
 * Copyright 2003, 2004, 2005, 2006 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2000 Bernardo Innocenti <bernie@develer.com>
 *
 * -->
 *
 * \brief Hardware independent timer driver (implementation)
 *
 * \version $Id$
 * \author Bernardo Innocenti <bernie@develer.com>
 */

#include "timer.h"

#include <cfg/cfg_timer.h>
#include <cfg/cfg_wdt.h>
#include <cfg/os.h>
#include <cfg/debug.h>
#include <cfg/module.h>

#include <cpu/attr.h>
#include <cpu/types.h>
#include <cpu/irq.h>

/*
 * Include platform-specific binding code if we're hosted.
 * Try the CPU specific one for bare-metal environments.
 */
#if OS_HOSTED
	#include OS_CSOURCE(timer)
#else
	#include CPU_CSOURCE(timer)
#endif

/*
 * Sanity check for config parameters required by this module.
 */
#if !defined(CONFIG_KERNEL) || ((CONFIG_KERNEL != 0) && CONFIG_KERNEL != 1)
	#error CONFIG_KERNEL must be set to either 0 or 1 in config.h
#endif
#if !defined(CONFIG_WATCHDOG) || ((CONFIG_WATCHDOG != 0) && CONFIG_WATCHDOG != 1)
	#error CONFIG_WATCHDOG must be set to either 0 or 1 in config.h
#endif

#if CONFIG_WATCHDOG
	#include <drv/wdt.h>
#endif

#if CONFIG_KERNEL
	#include <config_kern.h>
	#if CONFIG_KERN_PREEMPTIVE
		#include <hw/switch.h>
	#endif
	#if CONFIG_KERN_SIGNALS
		#include <kern/signal.h> /* sig_wait(), sig_check() */
		#include <kern/proc.h>   /* proc_current() */
		#include <cfg/macros.h>  /* BV() */
	#endif
#endif


/**
 * \def CONFIG_TIMER_STROBE
 *
 * This is a debug facility that can be used to
 * monitor timer interrupt activity on an external pin.
 *
 * To use strobes, redefine the macros TIMER_STROBE_ON,
 * TIMER_STROBE_OFF and TIMER_STROBE_INIT and set
 * CONFIG_TIMER_STROBE to 1.
 */
#if !defined(CONFIG_TIMER_STROBE) || !CONFIG_TIMER_STROBE
	#define TIMER_STROBE_ON    do {/*nop*/} while(0)
	#define TIMER_STROBE_OFF   do {/*nop*/} while(0)
	#define TIMER_STROBE_INIT  do {/*nop*/} while(0)
#endif


/// Master system clock (1 tick accuracy)
volatile ticks_t _clock;


#ifndef CONFIG_TIMER_DISABLE_EVENTS

/**
 * List of active asynchronous timers.
 */
REGISTER static List timers_queue;


/**
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


	/* Inserting timers twice causes mayhem. */
	ASSERT(timer->magic != TIMER_MAGIC_ACTIVE);
	DB(timer->magic = TIMER_MAGIC_ACTIVE;)

	IRQ_SAVE_DISABLE(flags);

	/* Calculate expiration time for this timer */
	timer->tick = _clock + timer->_delay;

	/*
	 * Search for the first node whose expiration time is
	 * greater than the timer we want to add.
	 */
	node = (Timer *)LIST_HEAD(&timers_queue);
	while (node->link.succ)
	{
		/*
		 * Stop just after the insertion point.
		 * (this fancy compare takes care of wrap-arounds).
		 */
		if (node->tick - timer->tick > 0)
			break;

		/* Go to next node */
		node = (Timer *)node->link.succ;
	}

	/* Enqueue timer request into the list */
	INSERT_BEFORE(&timer->link, &node->link);

	IRQ_RESTORE(flags);
}


/**
 * Remove a timer from the timer queue before it has expired.
 */
Timer *timer_abort(Timer *timer)
{
	ATOMIC(REMOVE(&timer->link));
	DB(timer->magic = TIMER_MAGIC_INACTIVE;)

	return timer;
}

#endif /* CONFIG_TIMER_DISABLE_EVENTS */


/**
 * Wait for the specified amount of timer ticks.
 */
void timer_delayTicks(ticks_t delay)
{
#if defined(IRQ_ENABLED)
	/* We shouldn't sleep with interrupts disabled */
	ASSERT(IRQ_ENABLED());
#endif

#if defined(CONFIG_KERN_SIGNALS) && CONFIG_KERN_SIGNALS
	Timer t;

	ASSERT(!sig_check(SIG_SINGLE));
	timer_set_event_signal(&t, proc_current(), SIG_SINGLE);
	timer_setDelay(&t, delay);
	timer_add(&t);
	sig_wait(SIG_SINGLE);

#else /* !CONFIG_KERN_SIGNALS */

	ticks_t start = timer_clock();

	/* Busy wait */
	while (timer_clock() - start < delay)
	{
#if CONFIG_WATCHDOG
		wdt_reset();
#endif
	}

#endif /* !CONFIG_KERN_SIGNALS */
}


#ifndef CONFIG_TIMER_DISABLE_UDELAY

/**
 * Busy wait until the specified amount of high-precision ticks have elapsed.
 *
 * \note This function is interrupt safe, the only
 *       requirement is a running hardware timer.
 */
void timer_busyWait(hptime_t delay)
{
	hptime_t now, prev = timer_hw_hpread();
	hptime_t delta;

	for(;;)
	{
		now = timer_hw_hpread();
		/*
		 * We rely on hptime_t being unsigned here to
		 * reduce the modulo to an AND in the common
		 * case of TIMER_HW_CNT.
		 */
		delta = (now - prev) % TIMER_HW_CNT;
		if (delta >= delay)
			break;
		delay -= delta;
		prev = now;
	}
}

/**
 * Wait for the specified amount of time (expressed in microseconds).
 *
 * \bug In AVR arch the maximum amount of time that can be used as
 *      delay could be very limited, depending on the hardware timer
 *      used. Check timer_avr.h, and what register is used as hptime_t.
 */
void timer_delayHp(hptime_t delay)
{
	if (UNLIKELY(delay > us_to_hptime(1000)))
	{
		timer_delayTicks(delay / (TIMER_HW_HPTICKS_PER_SEC / TIMER_TICKS_PER_SEC));
		delay %= (TIMER_HW_HPTICKS_PER_SEC / TIMER_TICKS_PER_SEC);
	}

	timer_busyWait(delay);
}
#endif /* CONFIG_TIMER_DISABLE_UDELAY */


/**
 * Timer interrupt handler. Find soft timers expired and
 * trigger corresponding events.
 */
DEFINE_TIMER_ISR
{
	/*
	 * With the Metrowerks compiler, the only way to force the compiler generate
	 * an interrupt service routine is to put a pragma directive within the function
	 * body.
	 */
	#ifdef __MWERKS__
	#pragma interrupt saveall
	#endif

#ifndef CONFIG_TIMER_DISABLE_EVENTS
	Timer *timer;
#endif
	/*
	 * On systems sharing IRQ line and vector, this check is needed
	 * to ensure that IRQ is generated by timer source.
	 */
	if (!timer_hw_triggered())
		return;

	TIMER_STROBE_ON;

	/* Perform hw IRQ handling */
	timer_hw_irq();

	/* Update the master ms counter */
	++_clock;

#ifndef CONFIG_TIMER_DISABLE_EVENTS
	/*
	 * Check the first timer request in the list and process
	 * it when it has expired. Repeat this check until the
	 * first node has not yet expired. Since the list is sorted
	 * by expiry time, all the following requests are guaranteed
	 * to expire later.
	 */
	while ((timer = (Timer *)LIST_HEAD(&timers_queue))->link.succ)
	{
		/* This request in list has not yet expired? */
		if (_clock - timer->tick < 0)
			break;

		/* Retreat the expired timer */
		REMOVE(&timer->link);
		DB(timer->magic = TIMER_MAGIC_INACTIVE;)

		/* Execute the associated event */
		event_do(&timer->expire);
	}
#endif /* CONFIG_TIMER_DISABLE_EVENTS */

	TIMER_STROBE_OFF;
}

MOD_DEFINE(timer)

/**
 * Initialize timer
 */
void timer_init(void)
{
	TIMER_STROBE_INIT;

#ifndef CONFIG_TIMER_DISABLE_EVENTS
	LIST_INIT(&timers_queue);
#endif

	_clock = 0;

	timer_hw_init();

	MOD_INIT(timer);
}

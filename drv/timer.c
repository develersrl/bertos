/*!
 * \file
 * <!--
 * Copyright 2003, 2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2000 Bernardo Innocenti <bernie@develer.com>
 * This file is part of DevLib - See devlib/README for information.
 * -->
 *
 * \brief Hardware independent timer driver (implementation)
 *
 * \version $Id$
 * \author Bernardo Innocenti <bernie@develer.com>
 */

/*#*
 *#* $Log$
 *#* Revision 1.25  2005/07/19 07:26:37  bernie
 *#* Refactor to decouple timer ticks from milliseconds.
 *#*
 *#* Revision 1.24  2005/04/11 19:10:28  bernie
 *#* Include top-level headers from cfg/ subdir.
 *#*
 *#* Revision 1.23  2004/12/13 12:07:06  bernie
 *#* DISABLE_IRQSAVE/ENABLE_IRQRESTORE: Convert to IRQ_SAVE_DISABLE/IRQ_RESTORE.
 *#*
 *#* Revision 1.22  2004/12/08 09:12:09  bernie
 *#* Rename time_t to mtime_t.
 *#*
 *#* Revision 1.21  2004/11/28 23:20:25  bernie
 *#* Remove obsolete INITLIST macro.
 *#*
 *#* Revision 1.20  2004/11/16 20:59:06  bernie
 *#* Add watchdog timer support.
 *#*/

#include "timer.h"
#include <cfg/cpu.h>
#include <hw.h>
#include <cfg/debug.h>
#include <appconfig.h>

#include CPU_CSOURCE(timer)

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

#if CONFIG_KERNEL && CONFIG_KERN_SIGNALS
	#include <kern/proc.h>
#endif


/*!
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


//! Master system clock (1 tick accuracy)
volatile ticks_t _clock;


#ifndef CONFIG_TIMER_DISABLE_EVENTS

/*!
 * List of active asynchronous timers.
 */
REGISTER static List timers_queue;


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
	INSERTBEFORE(&timer->link, &node->link);

	IRQ_RESTORE(flags);
}


/*!
 * Remove a timer from the timer queue before it has expired.
 */
Timer *timer_abort(Timer *timer)
{
	ATOMIC(REMOVE(&timer->link));
	DB(timer->magic = TIMER_MAGIC_INACTIVE;)

	return timer;
}

#endif /* CONFIG_TIMER_DISABLE_EVENTS */


/*!
 * Wait for the specified amount of time (expressed in ms).
 */
void timer_delayTicks(ticks_t delay)
{
#if defined(IRQ_GETSTATE)
	/* We shouldn't sleep with interrupts disabled */
	ASSERT(IRQ_GETSTATE());
#endif

#if defined(CONFIG_KERN_SIGNALS) && CONFIG_KERN_SIGNALS
	Timer t;

	ASSERT(!sig_check(SIG_SINGLE));
	timer_set_event_signal(&t, proc_current(), SIG_SINGLE);
	timer_set_delay(&t, delay);
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

/*!
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

/*!
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


/*!
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

	TIMER_STROBE_ON;

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


/*!
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
}


#if CONFIG_TEST

static void timer_test_constants(void)
{
	kprintf("TIMER_PRESCALER=%d\n", TIMER_PRESCALER);
	kprintf("TIMER_HW_HPTICKS_PER_SEC=%lu\n", TIMER_HW_HPTICKS_PER_SEC);
	#ifdef TIMER1_OVF_COUNT
		kprintf("TIMER1_OVF_COUNT=%d\n", (int)TIMER1_OVF_COUNT);
	#endif
	kprintf("TIMER_TICKS_PER_MSEC=%d\n", (int)TIMER_TICKS_PER_MSEC);
	kprintf("\n");
	kprintf("ms_to_ticks(100)=%lu\n", ms_to_ticks(100));
	kprintf("ms_to_ticks(10000)=%lu\n", ms_to_ticks(10000));
	kprintf("us_to_ticks(100)=%lu\n", us_to_ticks(100));
	kprintf("us_to_ticks(10000)=%lu\n", us_to_ticks(10000));
	kprintf("\n");
	kprintf("ticks_to_ms(100)=%lu\n", ticks_to_ms(100));
	kprintf("ticks_to_ms(10000)=%lu\n", ticks_to_ms(10000));
	kprintf("ticks_to_us(100)=%lu\n", ticks_to_us(100));
	kprintf("ticks_to_us(10000)=%lu\n", ticks_to_us(10000));
	kprintf("\n");
	kprintf("hptime_to_us(100)=%lu\n", hptime_to_us(100));
	kprintf("hptime_to_us(10000)=%lu\n", hptime_to_us(10000));
	kprintf("us_to_hptime(100)=%lu\n", us_to_hptime(100));
	kprintf("us_to_hptime(10000)=%lu\n", us_to_hptime(10000));
}

static void timer_test_delay(void)
{
	int i;

	kputs("Delay test\n");
	for (i = 0; i < 1000; i += 100)
	{
		kprintf("delay %d...", i);
		timer_delay(i);
		kputs("done\n");
	}
}

static void timer_test_hook(iptr_t _timer)
{
	Timer *timer = (Timer *)(void *)_timer;

	kprintf("Timer %ld expired\n", ticks_to_ms(timer->_delay));
	timer_add(timer);
}

static void timer_test_async(void)
{
	static Timer test_timers[5];
	static const mtime_t test_delays[5] = { 170, 50, 310, 1500, 310 };
	size_t i;

	for (i = 0; i < countof(test_timers); ++i)
	{
		Timer *timer = &test_timers[i];
		timer_setDelay(timer, ms_to_ticks(test_delays[i]));
		timer_set_event_softint(timer, timer_test_hook, (iptr_t)timer);
		timer_add(timer);
	}
}

static void timer_test_poll(void)
{
	int secs = 0;
	mtime_t start_time = ticks_to_ms(timer_clock());
	mtime_t now;

	while (secs <= 10)
	{
		now = ticks_to_ms(timer_clock());
		if (now - start_time >= 1000)
		{
			++secs;
			start_time += 1000;
			kprintf("seconds = %d, ticks=%ld\n", secs, now);
		}
	}
}

void timer_test(void)
{
	timer_test_constants();
	timer_test_delay();
	timer_test_async();
	timer_test_poll();
}

#endif /* CONFIG_TEST */

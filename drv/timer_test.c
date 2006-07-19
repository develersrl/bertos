/**
 * \file
 * <!--
 * Copyright 2005 Develer S.r.l. (http://www.develer.com/)
 * This file is part of DevLib - See README.devlib for information.
 * -->
 *
 * \brief Hardware independent timer driver (implementation)
 *
 * \version $Id$
 * \author Bernardo Innocenti <bernie@develer.com>
 */

/*#*
 *#* $Log$
 *#* Revision 1.4  2006/07/19 12:56:26  bernie
 *#* Convert to new Doxygen style.
 *#*
 *#* Revision 1.3  2006/02/21 21:28:02  bernie
 *#* New time handling based on TIMER_TICKS_PER_SEC to support slow timers with ticks longer than 1ms.
 *#*
 *#* Revision 1.2  2005/11/27 03:58:18  bernie
 *#* Add POSIX timer emulator.
 *#*
 *#* Revision 1.1  2005/11/27 03:04:08  bernie
 *#* Move test code to timer_test.c; Add OS_HOSTED support.
 *#*
 *#*/

#include "timer.c"
#include "mware/event.c"
#include "os/hptime.c"

static void timer_test_constants(void)
{
	kprintf("TIMER_HW_HPTICKS_PER_SEC=%lu\n", TIMER_HW_HPTICKS_PER_SEC);
	#ifdef TIMER_PRESCALER
		kprintf("TIMER_PRESCALER=%d\n", TIMER_PRESCALER);
	#endif
	#ifdef TIMER1_OVF_COUNT
		kprintf("TIMER1_OVF_COUNT=%d\n", (int)TIMER1_OVF_COUNT);
	#endif
	kprintf("TIMER_TICKS_PER_SEC=%d\n", (int)TIMER_TICKS_PER_SEC);
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
		wdt_reset();
	}
}

int main(void)
{
	wdt_init(7);
	timer_init();
	timer_test_constants();
	timer_test_delay();
	timer_test_async();
	timer_test_poll();
	return 0;
}

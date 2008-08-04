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
 * Copyright 2005 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief Hardware independent timer driver (implementation)
 *
 * \version $Id$
 * \author Bernie Innocenti <bernie@codewiz.org>
 */

#include <cfg/test.h>

#include <drv/timer.h>
#include <drv/wdt.h>

#include <mware/event.h>

#warning TODO:Refactor this test to comply whit BeRTOS test policy.

#ifdef _TEST

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
	kprintf("hptime_to_us(100)=%ld\n", hptime_to_us(100));
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


#include "timer.c"
#include "mware/event.c"
#include "os/hptime.c"

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
#endif


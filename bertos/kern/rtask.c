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
 * Copyright 2010 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief Recurrent task module implementation.
 *
 * \author Luca Ottaviano <lottaviano@develer.com>
 * \author Francesco Sacchi <batt@develer.com>
 *
 */

#include "rtask.h"
#include "cfg/cfg_rtask.h"
#include <cfg/module.h> // MOD_CHECK

#define LOG_LEVEL RTASK_LOG_LEVEL
#define LOG_FORMAT RTASK_LOG_FORMAT
#include <cfg/log.h>

#include <cpu/types.h>

#include <drv/timer.h>

#include <struct/pool.h>
#include <struct/list.h>

#include <kern/proc.h>
#include <kern/signal.h>
#include <kern/sem.h>

#define NEW_TASK SIG_USER0

// TODO: Mixing static and dynamic tests in kernel must be tested with care,
// until then use this workaround
#if CONFIG_KERN_HEAP
	#define PROC_NEW() proc_new(rtask_proc, NULL, CONFIG_RTASK_STACK, NULL)
#else
	PROC_DEFINE_STACK(rtask_stack, CONFIG_RTASK_STACK);
	#define PROC_NEW() proc_new(rtask_proc, NULL, sizeof(rtask_stack), rtask_stack)
#endif


struct RTask
{
	Timer t;
	rtask_cb_t callback;
	void *user_data;
};

DEFINE_POOL_STATIC(rtask_pool, RTask, CONFIG_RTASK_POOL_SIZE);
static Process *process = NULL;
static List rt_list;
static Semaphore rtask_sem;
#define RTASK_ATOMIC(code) \
	do {                         \
		sem_obtain(&rtask_sem);  \
		code;                    \
		sem_release(&rtask_sem); \
	} while (0)


//TODO: "Remove synctimer_poll and use a list directly"
static NORETURN void rtask_proc(void)
{
	while (1)
	{
		bool empty;
		RTASK_ATOMIC(
			empty = LIST_EMPTY(&rt_list);
			sig_check(NEW_TASK);
		);
		if (empty)
			sig_wait(NEW_TASK);

		ticks_t delay;
		RTASK_ATOMIC(delay = synctimer_nextTimeout(&rt_list));
		timer_delayTicks(delay);
		RTASK_ATOMIC(synctimer_poll(&rt_list));
	}
}

static void rtask_trampoline(void *_rtask)
{
	// Access the pool and the list freely since this callback is called
	// with the semaphore held in rtask_proc.
	RTask *rtask = _rtask;
	if (rtask->callback(rtask->user_data))
		synctimer_readd(&rtask->t, &rt_list);
	else
		pool_free(&rtask_pool, rtask);
}

RTask *rtask_add(rtask_cb_t cb, mtime_t delay, void *cb_data)
{
	// Beware: this function is called from a different process
	// than rtask_proc, so each access to rtask_pool and rt_list
	// must be protected with a semaphore.

	// The semaphore is not yet initialized, disable preemption
	// altogether.
	proc_forbid();
	if (UNLIKELY(process == NULL))
	{
		MOD_CHECK(proc);

		LIST_INIT(&rt_list);
		pool_init(rtask_pool, NULL);
		sem_init(&rtask_sem);
		process = PROC_NEW();
		ASSERT(process);
	}
	proc_permit();

	RTask *rt = NULL;
	RTASK_ATOMIC(rt = (RTask *)pool_alloc(&rtask_pool));
	if (rt)
	{
		rt->callback = cb;
		rt->user_data = cb_data;
		timer_setSoftint(&rt->t, rtask_trampoline, rt);
		timer_setDelay(&rt->t, ms_to_ticks(delay));
		RTASK_ATOMIC(synctimer_add(&rt->t, &rt_list));
		sig_send(process, NEW_TASK);
	}
	else
		LOG_ERR("Failed to allocate RTask\n");
	return rt;
}

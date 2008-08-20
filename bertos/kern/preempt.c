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
 * Copyright 2008 Bernie Innocenti <bernie@codewiz.org>
 * -->
 *
 * \brief Simple preemptive multitasking scheduler.
 *
 * \version $Id: proc.c 1616 2008-08-10 19:41:26Z bernie $
 * \author Bernie Innocenti <bernie@codewiz.org>
 */

#include "proc_p.h"
#include "proc.h"

#include <kern/irq.h>
#include <kern/monitor.h>
#include <cpu/frame.h> // CPU_IDLE
#include <drv/timer.h>
#include <cfg/module.h>



Timer preempt_timer;

/**
 * Disable preemptive task switching.
 *
 * The scheduler maintains a per-process nesting counter.  Task switching is
 * effectively re-enabled only when the number of calls to proc_permit()
 * matches the number of calls to proc_forbid().
 *
 * Calling functions that could sleep while task switching is disabled
 * is dangerous, although supported.  Preemptive task switching is
 * resumed while the process is sleeping and disabled again as soon as
 * it wakes up again.
 *
 * \sa proc_permit()
 */
void proc_forbid(void)
{
	/* No need to protect against interrupts here. */
	++CurrentProcess->forbid_cnt;
}

/**
 * Re-enable preemptive task switching.
 *
 * \sa proc_forbid()
 */
void proc_permit(void)
{
	/* No need to protect against interrupts here. */
	--CurrentProcess->forbid_cnt;
}


void proc_preempt(void)
{
	IRQ_DISABLE;

	LIST_ASSERT_VALID(&ProcReadyList);
	CurrentProcess = (struct Process *)list_remHead(&ProcReadyList);
	LIST_ASSERT_VALID(&ProcReadyList);
	ASSERT2(CurrentProcess, "no idle proc?");

	IRQ_ENABLE;

	TRACEMSG("new proc: %p:%s", CurrentProcess, CurrentProcess ? CurrentProcess->monitor.name : "---");
	monitor_report();
}

void proc_preempt_timer(UNUSED_ARG(void *, param))
{
	IRQ_DISABLE;
/*
	if (!CurrentProcess->forbid_cnt)
	{
		TRACEMSG("preempting %p:%s", CurrentProcess, CurrentProcess->monitor.name);
		LIST_ASSERT_VALID(&ProcReadyList);
		SCHED_ENQUEUE(CurrentProcess);
		proc_preempt();
	}
*/
	IRQ_ENABLE;

	timer_setDelay(&preempt_timer, CONFIG_KERN_QUANTUM);
	timer_add(&preempt_timer);
}

void proc_schedule(void)
{
	TRACE;

	// Will invoke proc_preempt() in interrupt context
	kill(0, SIGUSR1);
}

void proc_yield(void)
{
	TRACE;

	ASSERT_IRQ_ENABLED();
	IRQ_DISABLE;
	SCHED_ENQUEUE(CurrentProcess);
	LIST_ASSERT_VALID(&ProcReadyList);
	proc_schedule();
	IRQ_ENABLE;
}

void proc_entry(void (*user_entry)(void))
{
	user_entry();
	proc_exit();
}


static cpustack_t idle_stack[CONFIG_PROC_DEFSTACKSIZE / sizeof(cpustack_t)];

/*
 * The idle process
 *
 * This process never dies and never sleeps.  It's also quite apathic
 * and a bit antisocial.
 *
 * Having an idle process costs some stack space, but simplifies the
 * interrupt-driven preemption logic because there is always a user
 * context to which we can return.
 */
static NORETURN void idle(void)
{
	for (;;)
	{
		TRACE;
		monitor_report();
		proc_yield(); // FIXME: CPU_IDLE
	}
}

void preempt_init(void)
{
	MOD_CHECK(irq);
	MOD_CHECK(timer);

	irq_register(SIGUSR1, proc_preempt);

	timer_setSoftint(&preempt_timer, proc_preempt_timer, NULL);
	timer_setDelay(&preempt_timer, CONFIG_KERN_QUANTUM);
	timer_add(&preempt_timer);

	proc_new(idle, NULL, sizeof(idle_stack), idle_stack);
}

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
 * All voluntary and preemptive context switching happens on exit from
 * a common interrupt (signal) dispatcher.  Preemption on quantum timeout
 * is regulated by a soft-timer.  Other kinds of preemption could happen
 * if an interrupt sends a signal to a higher priority process (but this
 * is still unimplemented).
 *
 * In the POSIX implementaiton, context switching is done by the portable
 * SVR4 swapcontext() facility.
 *
 * \version $Id: proc.c 1616 2008-08-10 19:41:26Z bernie $
 * \author Bernie Innocenti <bernie@codewiz.org>
 */

#include <cfg/cfg_kern.h>

#if CONFIG_KERN_PREEMPT

#include "proc_p.h"
#include "proc.h"

#include <kern/irq.h>
#include <kern/monitor.h>
#include <cpu/frame.h> // CPU_IDLE
#include <cpu/irq.h>   // IRQ_DISABLE()...
#include <drv/timer.h>
#include <cfg/module.h>
#include <cfg/depend.h>    // CONFIG_DEPEND()

// Check config dependencies
CONFIG_DEPEND(CONFIG_KERN_PREEMPT,    CONFIG_KERN_SCHED && CONFIG_TIMER_EVENTS && CONFIG_KERN_IRQ);

MOD_DEFINE(preempt)

int preempt_forbid_cnt;

static Timer preempt_timer;


// fwd decl from idle.c
void idle_init(void);


void proc_schedule(void)
{
	IRQ_DISABLE;

	ASSERT(preempt_forbid_cnt == 0);
	LIST_ASSERT_VALID(&ProcReadyList);
	CurrentProcess = (struct Process *)list_remHead(&ProcReadyList);
	ASSERT2(CurrentProcess, "no idle proc?");

	IRQ_ENABLE;

	TRACEMSG("launching %p:%s", CurrentProcess, proc_currentName());
}

void proc_preempt(UNUSED_ARG(void *, param)
{
	if (!preempt_forbid_cnt)
	{
		IRQ_DISABLE;

		#if CONFIG_KERN_PRI
			Process *rival = (Process *)LIST_HEAD(&ProcReadyList);
			if (rival && rival->link.pri >= CurrentProcess->link.pri)
			{
		#endif

		TRACEMSG("preempting %p:%s", CurrentProcess, proc_currentName());

// FIXME: this still breaks havoc, probably because of some reentrancy issue
#if 0
		SCHED_ENQUEUE(CurrentProcess);
		proc_schedule();
#endif
		#if CONFIG_KERN_PRI
			}
		#endif

		IRQ_ENABLE;
	}

	timer_setDelay(&preempt_timer, CONFIG_KERN_QUANTUM);
	timer_add(&preempt_timer);
}

void proc_switch(void)
{
	ATOMIC(LIST_ASSERT_VALID(&ProcReadyList));
	TRACEMSG("%p:%s", CurrentProcess, proc_currentName());
	ATOMIC(LIST_ASSERT_VALID(&ProcReadyList));

	/* Sleeping with IRQs disabled or preemption forbidden is illegal */
	IRQ_ASSERT_ENABLED();
	ASSERT(preempt_forbid_cnt == 0);

	// Will invoke proc_switch() in interrupt context
	kill(0, SIGUSR1);
}

void proc_yield(void)
{
	TRACEMSG("%p:%s", CurrentProcess, proc_currentName());

	IRQ_DISABLE;
	SCHED_ENQUEUE(CurrentProcess);
	IRQ_ENABLE;

	proc_switch();
}

void proc_entry(void (*user_entry)(void))
{
	user_entry();
	proc_exit();
}

void preempt_init(void)
{
	MOD_CHECK(irq);
	MOD_CHECK(timer);

	irq_register(SIGUSR1, proc_schedule);

	timer_setSoftint(&preempt_timer, proc_preempt_timer, NULL);
	timer_setDelay(&preempt_timer, CONFIG_KERN_QUANTUM);
	timer_add(&preempt_timer);

	idle_init();

	MOD_INIT(preempt);
}

#endif // CONFIG_KERN_PREEMPT

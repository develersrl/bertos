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
 * Copyright 2009 Andrea Righi <arighi@develer.com>
 * -->
 *
 * \brief Simple preemptive multitasking scheduler.
 *
 * Preemption is explicitly regulated at the exit of each interrupt service
 * routine (ISR). Each task obtains a time quantum as soon as it is scheduled
 * on the CPU and its quantum is decremented at each clock tick. The frequency
 * of the timer determines the system tick granularity and CONFIG_KERN_QUANTUM
 * the time sharing interval.
 *
 * When the quantum expires the handler proc_needPreempt() checks if the
 * preemption is enabled and in this case preempt_schedule() is called, that
 * possibly replaces the current running thread with a different one.
 *
 * The preemption can be disabled or enabled via proc_forbid() and
 * proc_permit() primitives. This is implemented using a global atomic counter.
 * When the counter is greater than 0 the task cannot be preempted; only when
 * the counter reaches 0 the task can be preempted again.
 *
 * Preemption-disabled sections may be nested. The preemption will be
 * re-enabled when the outermost preemption-disabled section completes.
 *
 * The voluntary preemption still happens via proc_switch() or proc_yield().
 * The first one assumes the current process has been already added to a
 * private wait queue (e.g., on a semaphore or a signal), while the second one
 * takes care of adding the process into the ready queue.
 *
 * Context switch is done by CPU-dependent support routines. In case of a
 * voluntary preemption the context switch routine must take care of
 * saving/restoring only the callee-save registers (the voluntary-preemption is
 * actually a function call). The kernel-preemption always happens inside a
 * signal/interrupt context and it must take care of saving all registers. For
 * this, in the entry point of each ISR the caller-save registers must be
 * saved. In the ISR exit point, if the context switch must happen, we switch
 * to user-context and call the same voluntary context switch routine that take
 * care of saving/restoring also the callee-save registers. On resume from the
 * switch, the interrupt exit point moves back to interrupt-context, resumes
 * the caller-save registers (saved in the ISR entry point) and return from the
 * interrupt-context.
 *
 * \note Thread priority (if enabled by CONFIG_KERN_PRI) defines the order in
 * the \p proc_ready_list and the capability to deschedule a running process. A
 * low-priority thread can't preempt a high-priority thread.
 *
 * A high-priority process can preempt a low-priority process immediately (it
 * will be descheduled and replaced in the interrupt exit point). Processes
 * running at the same priority can be descheduled when they expire the time
 * quantum.
 *
 * \note Sleeping while preemption is disabled fallbacks to a busy-wait sleep.
 * Voluntary preemption when preemption is disabled raises a kernel bug.
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 * \author Andrea Righi <arighi@develer.com>
 */

#include "cfg/cfg_proc.h"

#if CONFIG_KERN_PREEMPT

#include "proc_p.h"
#include "proc.h"

#include <kern/irq.h>
#include <kern/monitor.h>
#include <kern/idle.h> // idle_proc
#include <cpu/frame.h> // CPU_IDLE
#include <cpu/irq.h>   // IRQ_DISABLE()...
#include <cfg/log.h>
#include <cfg/module.h>
#include <cfg/depend.h>    // CONFIG_DEPEND()

// Check config dependencies
CONFIG_DEPEND(CONFIG_KERN_PREEMPT, CONFIG_KERN);

MOD_DEFINE(preempt)

/**
 * CPU dependent context switching routines.
 *
 * Saving and restoring the context on the stack is done by a CPU-dependent
 * support routine which usually needs to be written in assembly.
 */
EXTERN_C void asm_switch_context(cpu_stack_t **new_sp, cpu_stack_t **save_sp);

/* Global preemption nesting counter */
cpu_atomic_t preempt_count;

/*
 * The time sharing interval: when a process is scheduled on a CPU it gets an
 * amount of CONFIG_KERN_QUANTUM clock ticks. When these ticks expires and
 * preemption is enabled a new process is selected to run.
 */
int _proc_quantum;

/**
 * Call the scheduler and eventually replace the current running process.
 */
static void preempt_schedule(void)
{
	Process *old_process = current_process;

	IRQ_ASSERT_DISABLED();

	/* Poll on the ready queue for the first ready process */
	LIST_ASSERT_VALID(&proc_ready_list);
	current_process = (Process *)list_remHead(&proc_ready_list);
	if (UNLIKELY(!current_process))
		current_process = idle_proc;
	_proc_quantum = CONFIG_KERN_QUANTUM;
	/*
	 * Optimization: don't switch contexts when the active process has not
	 * changed.
	 */
	if (LIKELY(old_process != current_process))
	{
		cpu_stack_t *dummy;

		/*
		 * Save context of old process and switch to new process. If
		 * there is no old process, we save the old stack pointer into
		 * a dummy variable that we ignore. In fact, this happens only
		 * when the old process has just exited.
		 *
		 * \todo Instead of physically clearing the process at exit
		 * time, a zombie list should be created.
		 */
		asm_switch_context(&current_process->stack,
				old_process ? &old_process->stack : &dummy);
	}

	/* This RET resumes the execution on the new process */
	LOG_INFO("resuming %p:%s\n", current_process, proc_currentName());
}

/**
 * Check if we need to schedule another task
 */
int preempt_needPreempt(void)
{
	if (UNLIKELY(current_process == NULL))
		return 0;
	if (!proc_preemptAllowed())
		return 0;
	return _proc_quantum ? prio_next() > prio_curr() :
			prio_next() >= prio_curr();
}

/**
 * Preempt the current task.
 */
void preempt_preempt(void)
{
	IRQ_ASSERT_DISABLED();
	ASSERT(current_process);

	/* Perform the kernel preemption */
	LOG_INFO("preempting %p:%s\n", current_process, proc_currentName());
	/* We are inside a IRQ context, so ATOMIC is not needed here */
	if (current_process != idle_proc)
		SCHED_ENQUEUE(current_process);
	preempt_schedule();
}

/**
 * Give the control of the CPU to another process.
 *
 * \note Assume the current process has been already added to a wait queue.
 *
 * \warning This should be considered an internal kernel function, even if it
 * is allowed, usage from application code is strongly discouraged.
 */
void preempt_switch(void)
{
	ASSERT(proc_preemptAllowed());
	IRQ_ASSERT_ENABLED();

	ATOMIC(preempt_schedule());
}

/**
 * Voluntarily release the CPU.
 */
void preempt_yield(void)
{
	/*
	 * Voluntary preemption while preemption is disabled is considered
	 * illegal, as not very useful in practice.
	 *
	 * ASSERT if it happens.
	 */
	ASSERT(proc_preemptAllowed());
	IRQ_ASSERT_ENABLED();

	ATOMIC(
		SCHED_ENQUEUE(current_process);
		preempt_schedule();
	);
}

void preempt_init(void)
{
	idle_init();
	MOD_INIT(preempt);
}

#endif // CONFIG_KERN_PREEMPT

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
 * Copyright 2001, 2004, 2008 Develer S.r.l. (http://www.develer.com/)
 * Copyright 1999, 2000, 2001, 2008 Bernie Innocenti <bernie@codewiz.org>
 * -->
 *
 * \brief Simple cooperative multitasking scheduler.
 *
 * \version $Id$
 * \author Bernie Innocenti <bernie@codewiz.org>
 * \author Stefano Fedrigo <aleph@develer.com>
 */

#include "proc_p.h"
#include "proc.h"

// Log settings for cfg/log.h.
#define LOG_LEVEL   KERN_LOG_LEVEL
#define LOG_FORMAT  KERN_LOG_FORMAT
#include <cfg/log.h>

#include <cpu/irq.h>
#include <cpu/types.h>
#include <cpu/attr.h>
#include <cpu/frame.h>

/**
 * CPU dependent context switching routines.
 *
 * Saving and restoring the context on the stack is done by a CPU-dependent
 * support routine which usually needs to be written in assembly.
 */
EXTERN_C void asm_switch_context(cpu_stack_t **new_sp, cpu_stack_t **save_sp);


/**
 * System scheduler: pass CPU control to the next process in
 * the ready queue.
 */
static void proc_schedule(void)
{
	cpu_flags_t flags;

	ATOMIC(LIST_ASSERT_VALID(&proc_ready_list));
	ASSERT_USER_CONTEXT();
	IRQ_ASSERT_ENABLED();

	/* Poll on the ready queue for the first ready process */
	IRQ_SAVE_DISABLE(flags);
	while (!(current_process = (struct Process *)list_remHead(&proc_ready_list)))
	{
		/*
		 * Make sure we physically reenable interrupts here, no matter what
		 * the current task status is. This is important because if we
		 * are idle-spinning, we must allow interrupts, otherwise no
		 * process will ever wake up.
		 *
		 * During idle-spinning, an interrupt can occur and it may
		 * modify \p proc_ready_list. To ensure that compiler reload this
		 * variable every while cycle we call CPU_MEMORY_BARRIER.
		 * The memory barrier ensure that all variables used in this context
		 * are reloaded.
		 * \todo If there was a way to write sig_wait() so that it does not
		 * disable interrupts while waiting, there would not be any
		 * reason to do this.
		 */
		IRQ_ENABLE;
		CPU_IDLE;
		MEMORY_BARRIER;
		IRQ_DISABLE;
	}
	IRQ_RESTORE(flags);
}

void proc_switch(void)
{
	/* Remember old process to save its context later */
	Process * const old_process = current_process;

	proc_schedule();

	/*
	 * Optimization: don't switch contexts when the active
	 * process has not changed.
	 */
	if (current_process != old_process)
	{
		cpu_stack_t *dummy;

		#if CONFIG_KERN_MONITOR
			LOG_INFO("Switch from %p(%s) to %p(%s)\n",
				old_process,    proc_name(old_process),
				current_process, proc_currentName());
		#endif

		/* Save context of old process and switch to new process. If there is no
		 * old process, we save the old stack pointer into a dummy variable that
		 * we ignore. In fact, this happens only when the old process has just
		 * exited.
		 * TODO: Instead of physically clearing the process at exit time, a zombie
		 * list should be created.
		 */
		asm_switch_context(&current_process->stack, old_process ? &old_process->stack : &dummy);
	}

	/* This RET resumes the execution on the new process */
}

/**
 * Co-operative context switch
 */
void proc_yield(void)
{
	ATOMIC(SCHED_ENQUEUE(current_process));
	proc_switch();
}

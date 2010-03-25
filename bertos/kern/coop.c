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
 * Define function prototypes exported outside.
 *
 * Required to silent gcc "no previous prototype" warnings.
 */
void coop_yield(void);
void coop_switch(void);
void coop_wakeup(Process *proc);

/**
 * Give the control of the CPU to another process.
 *
 * \note Assume the current process has been already added to a wait queue.
 *
 * \warning This should be considered an internal kernel function, even if it
 * is allowed, usage from application code is strongly discouraged.
 */
void coop_switch(void)
{
	ATOMIC(proc_schedule());
}

/**
 * Immediately wakeup a process, dispatching it to the CPU.
 */
void coop_wakeup(Process *proc)
{
	ASSERT(proc_preemptAllowed());
	ASSERT(current_process);
	IRQ_ASSERT_DISABLED();

	if (prio_proc(proc) >= prio_curr())
	{
		Process *old_process = current_process;

		SCHED_ENQUEUE(current_process);
		current_process = proc;
		proc_switchTo(current_process, old_process);
	}
	else
		SCHED_ENQUEUE_HEAD(proc);
}

/**
 * Co-operative context switch
 */
void coop_yield(void)
{
	ATOMIC(SCHED_ENQUEUE(current_process));
	coop_switch();
}

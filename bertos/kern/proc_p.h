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
 * Copyright 2001, 2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 1999, 2000, 2001 Bernie Innocenti <bernie@codewiz.org>
 *
 * -->
 *
 * \brief Internal scheduler structures and definitions for processes.
 *
 * \version $Id$
 * \author Bernie Innocenti <bernie@codewiz.org>
 */

#ifndef KERN_PROC_P_H
#define KERN_PROC_P_H

#include "cfg/cfg_kern.h"
#include <cfg/compiler.h>

#include <cpu/types.h>        /* for cpu_stack_t */

#include <struct/list.h>

#if CONFIG_KERN_PREEMPTIVE
	#include <ucontext.h> // XXX
#endif

typedef struct Process
{
#if CONFIG_KERN_PRI
	PriNode      link;        /**< Link Process into scheduler lists */
#else
	Node         link;        /**< Link Process into scheduler lists */
#endif
	cpustack_t  *stack;       /**< Per-process SP */
	iptr_t       user_data;   /**< Custom data passed to the process */

#if CONFIG_KERN_SIGNALS
	sigmask_t    sig_wait;    /**< Signals the process is waiting for */
	sigmask_t    sig_recv;    /**< Received signals */
#endif

#if CONFIG_KERN_HEAP
	uint16_t     flags;       /**< Flags */
#endif

#if CONFIG_KERN_HEAP | CONFIG_KERN_MONITOR | (ARCH & ARCH_EMUL)
	cpustack_t  *stack_base;  /**< Base of process stack */
	size_t       stack_size;  /**< Size of process stack */
#endif

#if CONFIG_KERN_PREEMPTIVE
	ucontext_t   context;
#endif

#if CONFIG_KERN_MONITOR
	struct ProcMonitor
	{
		Node        link;
		const char *name;
	} monitor;
#endif

} Process;


/**
 * \name Flags for Process.flags.
 * \{
 */
#define PF_FREESTACK  BV(0)  /**< Free the stack when process dies */
/*\}*/


/** Track running processes. */
extern REGISTER Process	*CurrentProcess;

/**
 * Track ready processes.
 *
 * Access to this list must be protected with a proc_forbid() / proc_premit()
 * pair, or with SCHED_ATOMIC()
 */
extern REGISTER List     ProcReadyList;

#if CONFIG_KERN_PRI
	/**
	 * Enqueue a task in the ready list.
	 *
	 * Always use this macro to instert a process in the ready list, as its
	 * might vary to implement a different scheduling algorithms.
	 *
	 * \note This macro is *NOT* protected against the scheduler.  Access to
	 *       this list must be performed with interrupts disabled.
	 */
	#define SCHED_ENQUEUE(proc)  do { \
			LIST_ASSERT_VALID(&ProcReadyList); \
			LIST_ENQUEUE(&ProcReadyList, &(proc)->link); \
		} while (0)

#else // !CONFIG_KERN_PRI

	#define SCHED_ENQUEUE(proc)  do { \
			LIST_ASSERT_VALID(&ProcReadyList); \
			ADDTAIL(&ProcReadyList, &(proc)->link); \
		} while (0)

#endif // !CONFIG_KERN_PRI

/** Schedule to another process *without* adding the current to the ready list. */
void proc_schedule(void);

#if CONFIG_KERN_PREEMPT
void proc_entry(void (*user_entry)(void));
void preempt_init(void);
#endif

#if CONFIG_KERN_MONITOR
	/** Initialize the monitor */
	void monitor_init(void);

	/** Register a process into the monitor */
	void monitor_add(Process *proc, const char *name);

	/** Unregister a process from the monitor */
	void monitor_remove(Process *proc);

	/** Rename a process */
	void monitor_rename(Process *proc, const char *name);
#endif /* CONFIG_KERN_MONITOR */

#endif /* KERN_PROC_P_H */

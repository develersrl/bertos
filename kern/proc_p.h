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
 * Copyright 1999, 2000, 2001 Bernardo Innocenti <bernie@develer.com>
 *
 * -->
 *
 * \brief Internal scheduler structures and definitions for processes.
 *
 * \version $Id$
 *
 * \author Bernardo Innocenti <bernie@develer.com>
 */

/*#*
 *#* $Log$
 *#* Revision 1.16  2006/07/19 12:56:27  bernie
 *#* Convert to new Doxygen style.
 *#*
 *#* Revision 1.15  2005/11/27 23:36:19  bernie
 *#* Use appconfig.h instead of cfg/config.h.
 *#*
 *#* Revision 1.14  2005/11/04 16:20:02  bernie
 *#* Fix reference to README.devlib in header.
 *#*
 *#* Revision 1.13  2005/04/11 19:10:28  bernie
 *#* Include top-level headers from cfg/ subdir.
 *#*
 *#* Revision 1.12  2004/12/08 08:57:35  bernie
 *#* Rename sigset_t to sigmask_t.
 *#*
 *#* Revision 1.11  2004/11/16 22:37:14  bernie
 *#* Replace IPTR with iptr_t.
 *#*
 *#* Revision 1.10  2004/10/19 11:47:07  bernie
 *#* Add missing #endif.
 *#*
 *#* Revision 1.9  2004/10/19 08:55:31  bernie
 *#* Define forbid_cnt.
 *#*
 *#* Revision 1.8  2004/10/03 20:39:28  bernie
 *#* Import changes from sc/firmware.
 *#*
 *#* Revision 1.7  2004/08/25 14:12:09  rasky
 *#* Aggiornato il comment block dei log RCS
 *#*
 *#* Revision 1.6  2004/08/24 16:05:15  bernie
 *#* Add missing headers; Reformat.
 *#*
 *#* Revision 1.5  2004/08/14 19:37:57  rasky
 *#* Merge da SC: macros.h, pool.h, BIT_CHANGE, nome dei processi, etc.
 *#*
 *#* Revision 1.4  2004/07/30 14:24:16  rasky
 *#* Task switching con salvataggio perfetto stato di interrupt (SR)
 *#* Kernel monitor per dump informazioni su stack dei processi
 *#*
 *#* Revision 1.3  2004/07/14 14:18:09  rasky
 *#* Merge da SC: Rimosso timer dentro il task, che è uno spreco di memoria per troppi task
 *#*
 *#* Revision 1.2  2004/06/03 11:27:09  bernie
 *#* Add dual-license information.
 *#*
 *#* Revision 1.1  2004/05/23 17:27:00  bernie
 *#* Import kern/ subdirectory.
 *#*
 *#* Revision 1.3  2004/05/14 12:52:13  rasky
 *#* Importato supporto kernel per AVR da Stefano
 *#*
 *#* Revision 1.2  2004/04/28 16:13:49  rasky
 *#* proc_schedule() is now semi-private (used only within the kernel)
 *#*
 *#* Revision 1.1  2004/04/26 18:02:40  rasky
 *#* Importato microkernel
 *#*
 *#* Revision 1.1  2004/04/04 17:40:26  aleph
 *#* Add multithreading kernel
 *#*
 *#*/
#ifndef KERN_PROC_P_H
#define KERN_PROC_P_H

#include <cfg/compiler.h>
#include <cpu/cpu.h>        /* for cpu_stack_t */
#include <mware/list.h>
#include <config_kern.h>
#include <appconfig.h>

typedef struct Process
{
	Node         link;        /**< Link Process into scheduler lists */
	cpustack_t  *stack;       /**< Per-process SP */
	iptr_t       user_data;   /**< Custom data passed to the process */

#if CONFIG_KERN_SIGNALS
	sigmask_t    sig_wait;    /**< Signals the process is waiting for */
	sigmask_t    sig_recv;    /**< Received signals */
#endif

#if CONFIG_KERN_PREEMPTIVE
	int          forbid_cnt;  /**< Nesting count for proc_forbid()/proc_permit(). */
#endif

#if CONFIG_KERN_HEAP
	uint16_t     flags;       /**< Flags */
	cpustack_t  *stack_base;  /**< Base of process stack */
	size_t       stack_size;  /**< Size of process stack */
#endif

#if CONFIG_KERN_MONITOR
	struct ProcMonitor
	{
		Node        link;
		const char *name;
		cpustack_t *stack_base;
		size_t      stack_size;
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

/** Track ready processes. */
extern REGISTER List     ProcReadyList;


/** Enqueue a task in the ready list. */
#define SCHED_ENQUEUE(proc)  ADDTAIL(&ProcReadyList, &(proc)->link)

/** Schedule to another process *without* adding the current to the ready list. */
void proc_schedule(void);

#if CONFIG_KERN_MONITOR
	/** Initialize the monitor */
	void monitor_init(void);

	/** Register a process into the monitor */
	void monitor_add(Process *proc, const char *name, cpustack_t *stack, size_t stacksize);

	/** Unregister a process from the monitor */
	void monitor_remove(Process *proc);

	/** Rename a process */
	void monitor_rename(Process *proc, const char* name);
#endif /* CONFIG_KERN_MONITOR */

#endif /* KERN_PROC_P_H */


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
 * Copyright 2001,2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 1999,2000,2001 Bernardo Innocenti <bernie@develer.com>
 *
 * -->
 *
 * \brief Simple realtime multitasking scheduler.
 *        Context switching is only done cooperatively.
 *
 * \version $Id$
 *
 * \author Bernardo Innocenti <bernie@develer.com>
 * \author Stefano Fedrigo <aleph@develer.com>
 */

/*#*
 *#* $Log$
 *#* Revision 1.32  2006/09/20 14:19:23  marco
 *#* Restored test.
 *#*
 *#* Revision 1.31  2006/07/19 12:56:27  bernie
 *#* Convert to new Doxygen style.
 *#*
 *#* Revision 1.30  2006/03/27 04:49:23  bernie
 *#* CPU_IDLE(): Fix for new emulator.
 *#*
 *#* Revision 1.29  2006/02/24 01:17:05  bernie
 *#* Update for new emulator.
 *#*
 *#* Revision 1.28  2006/02/21 16:06:55  bernie
 *#* Cleanup/update process scheduling.
 *#*
 *#* Revision 1.27  2005/11/04 16:20:02  bernie
 *#* Fix reference to README.devlib in header.
 *#*
 *#* Revision 1.26  2005/04/11 19:10:28  bernie
 *#* Include top-level headers from cfg/ subdir.
 *#*
 *#* Revision 1.25  2005/03/15 00:20:54  bernie
 *#* proc_schedule(): New sanity check.
 *#*
 *#* Revision 1.24  2005/01/08 09:20:54  bernie
 *#* Remove unused variable.
 *#*
 *#* Revision 1.23  2004/12/13 12:07:06  bernie
 *#* DISABLE_IRQSAVE/ENABLE_IRQRESTORE: Convert to IRQ_SAVE_DISABLE/IRQ_RESTORE.
 *#*
 *#* Revision 1.22  2004/12/13 11:51:08  bernie
 *#* DISABLE_INTS/ENABLE_INTS: Convert to IRQ_DISABLE/IRQ_ENABLE.
 *#*
 *#* Revision 1.21  2004/11/28 23:20:25  bernie
 *#* Remove obsolete INITLIST macro.
 *#*
 *#* Revision 1.20  2004/11/16 22:37:14  bernie
 *#* Replace IPTR with iptr_t.
 *#*
 *#* Revision 1.19  2004/10/19 11:47:39  bernie
 *#* Kill warnings when !CONFIG_PROC_MONITOR.
 *#*
 *#* Revision 1.18  2004/10/19 08:54:43  bernie
 *#* Initialize forbid_cnt; Formatting/comments fixes.
 *#*
 *#* Revision 1.17  2004/10/19 08:47:13  bernie
 *#* proc_rename(), proc_forbid(), proc_permit(): New functions.
 *#*
 *#* Revision 1.16  2004/10/03 20:39:28  bernie
 *#* Import changes from sc/firmware.
 *#*
 *#* Revision 1.15  2004/09/20 03:29:39  bernie
 *#* C++ fixes.
 *#*
 *#* Revision 1.14  2004/09/14 21:06:44  bernie
 *#* Use debug.h instead of kdebug.h.
 *#*
 *#* Revision 1.13  2004/08/29 21:58:53  bernie
 *#* Include macros.h explicityl.
 *#*
 *#* Revision 1.11  2004/08/24 16:09:08  bernie
 *#* Add missing header.
 *#*
 *#* Revision 1.10  2004/08/24 16:07:01  bernie
 *#* Use kputs()/kputchar() when possible.
 *#*
 *#* Revision 1.9  2004/08/24 14:26:57  bernie
 *#* monitor_debug_stacks(): Conditionally compile on CONFIG_KERN_MONITOR.
 *#*
 *#* Revision 1.8  2004/08/14 19:37:57  rasky
 *#* Merge da SC: macros.h, pool.h, BIT_CHANGE, nome dei processi, etc.
 *#*
 *#* Revision 1.7  2004/08/02 20:20:29  aleph
 *#* Merge from project_ks
 *#*
 *#* Revision 1.6  2004/07/30 14:24:16  rasky
 *#* Task switching con salvataggio perfetto stato di interrupt (SR)
 *#* Kernel monitor per dump informazioni su stack dei processi
 *#*
 *#* Revision 1.5  2004/07/14 14:18:09  rasky
 *#* Merge da SC: Rimosso timer dentro il task, che è uno spreco di memoria per troppi task
 *#*
 *#* Revision 1.4  2004/07/13 19:21:28  aleph
 *#* Avoid warning for unused arg when compiled without some CONFIG_KERN_xx options
 *#*
 *#* Revision 1.3  2004/06/06 18:37:57  bernie
 *#* Rename event macros to look like regular functions.
 *#*
 *#* Revision 1.2  2004/06/03 11:27:09  bernie
 *#* Add dual-license information.
 *#*
 *#* Revision 1.1  2004/05/23 17:27:00  bernie
 *#* Import kern/ subdirectory.
 *#*
 *#*/

#include "proc_p.h"
#include "proc.h"
//#include "hw.h"
#include <mware/event.h>
#include <cfg/cpu.h>
#include <cfg/debug.h>
#include <cfg/arch_config.h>  /* ARCH_EMUL */
#include <cfg/macros.h>  /* ABS() */

#include <string.h> /* memset() */

/**
 * CPU dependent context switching routines.
 *
 * \note This function *MUST* preserve also the status of the interrupts.
 */
EXTERN_C void asm_switch_context(cpustack_t **new_sp, cpustack_t **save_sp);
EXTERN_C int asm_switch_version(void);

/*
 * The scheduer tracks ready and waiting processes
 * by enqueuing them in these lists. A pointer to the currently
 * running process is stored in the CurrentProcess pointer.
 *
 * NOTE: these variables are protected by DI/EI locking
 */
REGISTER Process *CurrentProcess;
REGISTER List     ProcReadyList;


#if CONFIG_KERN_PREEMPTIVE
/*
 * The time sharing scheduler forces a task switch when
 * the current process has consumed its quantum.
 */
uint16_t Quantum;
#endif


/* In Win32 we must emulate stack on the real process stack */
#if (ARCH & ARCH_EMUL)
extern List StackFreeList;
#endif

/** The main process (the one that executes main()). */
struct Process MainProcess;


static void proc_init_struct(Process *proc)
{
	/* Avoid warning for unused argument. */
	(void)proc;

#if CONFIG_KERN_SIGNALS
	proc->sig_recv = 0;
#endif

#if CONFIG_KERN_PREEMPTIVE
	proc->forbid_cnt = 0;
#endif

#if CONFIG_KERN_HEAP
	proc->flags = 0;
#endif
}


void proc_init(void)
{
	LIST_INIT(&ProcReadyList);

#if CONFIG_KERN_MONITOR
	monitor_init();
#endif

	/* We "promote" the current context into a real process. The only thing we have
	 * to do is create a PCB and make it current. We don't need to setup the stack
	 * pointer because it will be written the first time we switch to another process.
	 */
	proc_init_struct(&MainProcess);
	CurrentProcess = &MainProcess;

	/* Make sure the assembly routine is up-to-date with us */
	ASSERT(asm_switch_version() == 1);
}


/**
 * Create a new process, starting at the provided entry point.
 *
 * \return Process structure of new created process
 *         if successful, NULL otherwise.
 */
struct Process *proc_new_with_name(UNUSED(const char *, name), void (*entry)(void), iptr_t data, size_t stacksize, cpustack_t *stack_base)
{
	Process *proc;
	size_t i;
	size_t proc_size_words = ROUND2(sizeof(Process), sizeof(cpustack_t)) / sizeof(cpustack_t);
#if CONFIG_KERN_HEAP
	bool free_stack = false;
#endif

#if (ARCH & ARCH_EMUL)
	/* Ignore stack provided by caller and use the large enough default instead. */
	stack_base = (cpustack_t *)LIST_HEAD(&StackFreeList);
	REMOVE(LIST_HEAD(&StackFreeList));
	stacksize = CONFIG_KERN_DEFSTACKSIZE;
#elif CONFIG_KERN_HEAP
	/* Did the caller provide a stack for us? */
	if (!stack_base)
	{
		/* Did the caller specify the desired stack size? */
		if (!stacksize)
			stacksize = CONFIG_KERN_DEFSTACKSIZE + sizeof(Process);

		/* Allocate stack dinamically */
		if (!(stack_base = heap_alloc(stacksize)))
			return NULL;

		free_stack = true;
	}
#else
	/* Stack must have been provided by the user */
	ASSERT(stack_base);
	ASSERT(stacksize);
#endif

#if CONFIG_KERN_MONITOR
	/* Fill-in the stack with a special marker to help debugging */
	memset(stack_base, CONFIG_KERN_STACKFILLCODE, stacksize / sizeof(cpustack_t));
#endif

	/* Initialize the process control block */
	if (CPU_STACK_GROWS_UPWARD)
	{
		proc = (Process*)stack_base;
		proc->stack = stack_base + proc_size_words;
		if (CPU_SP_ON_EMPTY_SLOT)
			proc->stack++;
	}
	else
	{
		proc = (Process*)(stack_base + stacksize / sizeof(cpustack_t) - proc_size_words);
		proc->stack = (cpustack_t*)proc;
		if (CPU_SP_ON_EMPTY_SLOT)
			proc->stack--;
	}

	proc_init_struct(proc);
	proc->user_data = data;

#if CONFIG_KERN_HEAP
	proc->stack_base = stack_base;
	proc->stack_size = stack_size;
	if (free_stack)
		proc->flags |= PF_FREESTACK;
#endif

	/* Initialize process stack frame */
	CPU_PUSH_CALL_CONTEXT(proc->stack, proc_exit);
	CPU_PUSH_CALL_CONTEXT(proc->stack, entry);

	/* Push a clean set of CPU registers for asm_switch_context() */
	for (i = 0; i < CPU_SAVED_REGS_CNT; i++)
		CPU_PUSH_WORD(proc->stack, CPU_REG_INIT_VALUE(i));

	/* Add to ready list */
	ATOMIC(SCHED_ENQUEUE(proc));

#if CONFIG_KERN_MONITOR
	monitor_add(proc, name, stack_base, stacksize);
#endif

	return proc;
}

/** Rename a process */
void proc_rename(struct Process *proc, const char *name)
{
#if CONFIG_KERN_MONITOR
	monitor_rename(proc, name);
#else
	(void)proc; (void)name;
#endif
}


/**
 * System scheduler: pass CPU control to the next process in
 * the ready queue.
 *
 * Saving and restoring the context on the stack is done
 * by a CPU-dependent support routine which must usually be
 * written in assembly.
 */
void proc_schedule(void)
{
	/* This function must not have any "auto" variables, otherwise
	 * the compiler might put them on the stack of the process
	 * being switched out.
	 */
	static struct Process *old_process;
	static cpuflags_t flags;

	/* Remember old process to save its context later */
	old_process = CurrentProcess;

#ifdef IRQ_RUNNING
	/* Scheduling in interrupts is a nono. */
	ASSERT(!IRQ_RUNNING());
#endif

	/* Poll on the ready queue for the first ready process */
	IRQ_SAVE_DISABLE(flags);
	while (!(CurrentProcess = (struct Process *)list_remHead(&ProcReadyList)))
	{
		/*
		 * Make sure we physically reenable interrupts here, no matter what
		 * the current task status is. This is important because if we
		 * are idle-spinning, we must allow interrupts, otherwise no
		 * process will ever wake up.
		 *
		 * \todo If there was a way to write sig_wait() so that it does not
		 * disable interrupts while waiting, there would not be any
		 * reason to do this.
		 */
		IRQ_ENABLE;
		CPU_IDLE;
		IRQ_DISABLE;
	}
	IRQ_RESTORE(flags);

	/*
	 * Optimization: don't switch contexts when the active
	 * process has not changed.
	 */
	if (CurrentProcess != old_process)
	{
		static cpustack_t *dummy;

#if CONFIG_KERN_PREEMPTIVE
		/* Reset quantum for this process */
		Quantum = CONFIG_KERN_QUANTUM;
#endif

		/* Save context of old process and switch to new process. If there is no
		 * old process, we save the old stack pointer into a dummy variable that
		 * we ignore. In fact, this happens only when the old process has just
		 * exited.
		 * TODO: Instead of physically clearing the process at exit time, a zombie
		 * list should be created.
		 */
		asm_switch_context(&CurrentProcess->stack, old_process ? &old_process->stack : &dummy);
	}

	/* This RET resumes the execution on the new process */
}


/**
 * Terminate the current process
 */
void proc_exit(void)
{
#if CONFIG_KERN_MONITOR
	monitor_remove(CurrentProcess);
#endif

#if CONFIG_KERN_HEAP
	/*
	 * The following code is BROKEN.
	 * We are freeing our own stack before entering proc_schedule()
	 * BAJO: A correct fix would be to rearrange the scheduler with
	 *  an additional parameter which frees the old stack/process
	 *  after a context switch.
	 */
	if (CurrentProcess->flags & PF_FREESTACK)
		heap_free(CurrentProcess->stack_base, CurrentProcess->stack_size);
	heap_free(CurrentProcess);
#endif

#if (ARCH & ARCH_EMUL)
#warning This is wrong
	/* Reinsert process stack in free list */
	ADDHEAD(&StackFreeList, (Node *)(CurrentProcess->stack
		- (CONFIG_KERN_DEFSTACKSIZE / sizeof(cpustack_t))));

	/*
	 * NOTE: At this point the first two words of what used
	 * to be our stack contain a list node. From now on, we
	 * rely on the compiler not reading/writing the stack.
	 */
#endif /* ARCH_EMUL */

	CurrentProcess = NULL;
	proc_schedule();
	/* not reached */
}


/**
 * Co-operative context switch
 */
void proc_switch(void)
{
	/* Just like proc_schedule, this function must not have auto variables. */
	static cpuflags_t flags;

	IRQ_SAVE_DISABLE(flags);
	SCHED_ENQUEUE(CurrentProcess);
	IRQ_RESTORE(flags);

	proc_schedule();
}


/**
 * Get the pointer to the current process
 */
struct Process *proc_current(void)
{
	return CurrentProcess;
}

/**
 * Get the pointer to the user data of the current process
 */
iptr_t proc_current_user_data(void)
{
	return CurrentProcess->user_data;
}


#if CONFIG_KERN_PREEMPTIVE

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

#endif /* CONFIG_KERN_PREEMPTIVE */

/*!
 * \file
 * <!--
 * Copyright 2001,2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 1999,2000,2001 Bernardo Innocenti <bernie@develer.com>
 * This file is part of DevLib - See devlib/README for information.
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
#include "cpu.h"
#include "event.h"
#include "hw.h"
#include <debug.h>
#include <arch_config.h>  /* ARCH_EMUL */
#include <macros.h>  /* ABS() */

#include <string.h> /* memset() */

/*! CPU dependent context switching routines
 *  \note This function *MUST* preserve also the status of the interrupts.
 */
extern void asm_switch_context(cpustack_t **new_sp, cpustack_t **save_sp);
extern int asm_switch_version(void);

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

/* The main process (the one that executes main()) */
struct Process MainProcess;


#if CONFIG_KERN_MONITOR
List MonitorProcs;

static void monitor_init(void)
{
	INITLIST(&MonitorProcs);
}

static void monitor_add(Process* proc, const char* name, cpustack_t* stack_base, size_t stack_size)
{
	proc->monitor.name = name;
	proc->monitor.stack_base = stack_base;
	proc->monitor.stack_size = stack_size;

	ADDTAIL(&MonitorProcs, &proc->monitor.link);
}

static void monitor_remove(Process* proc)
{
	REMOVE(&proc->monitor.link);
}

#define MONITOR_NODE_TO_PROCESS(node) \
	(struct Process*)((char*)(node) - offsetof(struct Process, monitor.link))

size_t monitor_check_stack(cpustack_t* stack_base, size_t stack_size)
{
	cpustack_t* beg;
	cpustack_t* cur;
	cpustack_t* end;
	size_t sp_free;

	beg = stack_base;
	end = stack_base + stack_size / sizeof(cpustack_t) - 1;

	if (CPU_STACK_GROWS_UPWARD)
	{
		cur = beg;
		beg = end;
		end = cur;
	}

	cur = beg;
	while (cur != end)
	{
		if (*cur != CONFIG_KERN_STACKFILLCODE)
			break;

		if (CPU_STACK_GROWS_UPWARD)
			cur--;
		else
			cur++;
	}

	sp_free = ABS(cur - beg) * sizeof(cpustack_t);
	return sp_free;
}

#if CONFIG_KERN_MONITOR

void monitor_debug_stacks(void)
{
	struct Process* p;
	int i;

	if (ISLISTEMPTY(&MonitorProcs))
	{
		kputs("No stacks registered in the monitor\n");
		return;
	}

	kprintf("%-24s    %-6s%-8s%-8s%-8s\n", "Process name", "TCB", "SPbase", "SPsize", "SPfree");
	for (i=0;i<56;i++)
		kputchar('-');
	kputchar('\n');

	for (p = MONITOR_NODE_TO_PROCESS(MonitorProcs.head);
		 p->monitor.link.succ;
		 p = MONITOR_NODE_TO_PROCESS(p->monitor.link.succ))
	{
		size_t free = monitor_check_stack(p->monitor.stack_base, p->monitor.stack_size);
		kprintf("%-24s    %04x    %04x    %4x    %4x\n", p->monitor.name, (uint16_t)p, (uint16_t)p->monitor.stack_base, (uint16_t)p->monitor.stack_size, (uint16_t)free);
	}
}

#endif /* CONFIG_KERN_MONITOR */

#endif


static void proc_init_struct(Process* proc)
{
	/* Avoid warning for unused argument */
	(void)proc;

#if CONFIG_KERN_SIGNALS
	proc->sig_recv = 0;
#endif

#if CONFIG_KERN_HEAP
	proc->flags = 0;
#endif
}


void proc_init(void)
{
	INITLIST(&ProcReadyList);

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


/*!
 * Create a new process, starting at the provided entry point.
 *
 * \return Process structure of new created process
 *         if successful, NULL otherwise.
 */
struct Process *proc_new_with_name(UNUSED(const char*, name), void (*entry)(void), IPTR data, size_t stacksize, cpustack_t *stack_base)
{
	Process *proc;
	cpuflags_t flags;
	size_t i;
	size_t proc_size_words = ROUND2(sizeof(Process), sizeof(cpustack_t)) / sizeof(cpustack_t);
#if CONFIG_KERN_HEAP
	bool free_stack = false;
#endif

#if (ARCH & ARCH_EMUL)
	/* Ignore stack provided by caller
	* and use the large enough default instead
	*/
	stack_base = (cpustack_t *)StackFreeList.head;
	REMOVE((Node *)stack_base);
	stacksize = DEF_STACKSIZE;
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
	DISABLE_IRQSAVE(flags);
	SCHED_ENQUEUE(proc);
	ENABLE_IRQRESTORE(flags);

#if CONFIG_KERN_MONITOR
	monitor_add(proc, name, stack_base, stacksize);
#endif

	return proc;
}


/*!
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
	static Process *old_process;
	static cpuflags_t flags;

	/* Remember old process to save its context later */
	old_process = CurrentProcess;

	/* Poll on the ready queue for the first ready process */
	DISABLE_IRQSAVE(flags);
	while (!(CurrentProcess = (struct Process*)REMHEAD(&ProcReadyList)))
	{
		/*
		 * Make sure we physically reenable interrupts here, no matter what
		 * the current task status is. This is important because if we
		 * are idle-spinning, we must allow interrupts, otherwise no
		 * process will ever wake up.
		 *
		 * \todo If there was a way to code sig_wait so that it does not
		 * disable interrupts while waiting, there would not be any
		 * reason to do this.
		 */
		ENABLE_INTS;
		SCHEDULER_IDLE;
		DISABLE_INTS;
	}
	ENABLE_IRQRESTORE(flags);

	/* Optimization: don't switch contexts when the active
	 * process has not changed.
	 */
	if (CurrentProcess != old_process)
	{
		static cpustack_t* dummy;

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


/*!
 * Terminate the current process
 */
void proc_exit(void)
{
#if CONFIG_KERN_HEAP
	/* The following code is BROKEN.
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
#error This is wrong
	/* Reinsert process stack in free list */
	ADDHEAD(&StackFreeList, (Node *)(CurrentProcess->stack
		- (DEF_STACKSIZE / sizeof(cpustack_t))));

	/* NOTE: At this point the first two words of what used
	 * to be our stack contain a list node. From now on, we
	 * rely on the compiler not reading/writing the stack.
	 */
#endif /* ARCH_EMUL */

#if CONFIG_KERN_MONITOR
	monitor_remove(CurrentProcess);
#endif

	CurrentProcess = NULL;
	proc_schedule();
	/* not reached */
}


/*!
 * Co-operative context switch
 */
void proc_switch(void)
{
	/* Just like proc_schedule, this function must not have auto variables. */
	static cpuflags_t flags;

	DISABLE_IRQSAVE(flags);
	SCHED_ENQUEUE(CurrentProcess);
	ENABLE_IRQRESTORE(flags);

	proc_schedule();
}


/*!
 * Get the pointer to the current process
 */
struct Process *proc_current(void)
{
	return CurrentProcess;
}

/*!
 * Get the pointer to the user data of the current process
 */
IPTR proc_current_user_data(void)
{
	return CurrentProcess->user_data;
}

#if 0 /* Simple testcase for the scheduler */

/*!
 * Proc scheduling test subthread 1
 */
static void NORETURN proc_test_thread1(void)
{
	for (;;)
	{
		kputs(">task 1\n");
		timer_delay(50);
		proc_switch();
	}
}

/*!
 * Proc scheduling test subthread 2
 */
static void NORETURN proc_test_thread2(void)
{
	for (;;)
	{
		kputs(">task 2\n");
		timer_delay(75);
		proc_switch();
	}
}

static cpustack_t proc_test_stack1[CONFIG_KERN_DEFSTACKSIZE/sizeof(cpustack_t)];
static cpustack_t proc_test_stack2[CONFIG_KERN_DEFSTACKSIZE/sizeof(cpustack_t)];

/*!
 * Proc scheduling test
 */
void NORETURN proc_test(void)
{
	proc_new(proc_test_thread1, NULL, sizeof(proc_test_stack1), proc_test_stack1);
	proc_new(proc_test_thread2, NULL, sizeof(proc_test_stack2), proc_test_stack2);
	kputs("Created tasks\n");

	kputs("stack1:\n");
	kdump(proc_test_stack1+sizeof(proc_test_stack1)-64, 64);
	kputs("stack2:\n");
	kdump(proc_test_stack2+sizeof(proc_test_stack1)-64, 64);

	for (;;)
	{
		kputs(">main task\n");
		timer_delay(93);
		proc_switch();
	}

	ASSERT(false);
}
#endif

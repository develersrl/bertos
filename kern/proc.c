/**
 * \file
 * <!--
 * Copyright 2001,2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 1999,2000,2001 Bernardo Innocenti <bernie@develer.com>
 * All Rights Reserved.
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

/*
 * $Log$
 * Revision 1.1  2004/05/23 17:27:00  bernie
 * Import kern/ subdirectory.
 *
 */

#include "cpu.h"
#include "proc_p.h"
#include "proc.h"
#include "event.h"
#include "hw.h"
#include <drv/kdebug.h>

#include <string.h> /* memset() */

/* CPU dependent context switching routines */
extern void asm_switch_context(cpustack_t **new_sp, cpustack_t **save_sp);

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

static void proc_init_struct(Process* proc)
{
#if CONFIG_KERN_TIMER
	INITEVENT_SIG(&proc->proc_timer.expire, proc, SIG_SINGLE);
#endif

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

	/* We "promote" the current context into a real process. The only thing we have
	   to do is create a PCB and make it current. We don't need to setup the stack
	   pointer because it will be written the first time we switch to another process. */
	proc_init_struct(&MainProcess);
	CurrentProcess = &MainProcess;
}


/*!
 * Create a new process, starting at the provided entry point.
 *
 * \return Process structure of new created process
 *         if successful, NULL otherwise.
 */
Process *proc_new(void (*entry)(void), size_t stacksize, cpustack_t *stack_base)
{
	Process *proc;
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

#ifdef _DEBUG
	/* Fill-in the stack with a special marker to help debugging */
	memset(stack_base, CONFIG_KERN_STACKFILLCODE, stacksize / sizeof(cpustack_t));
#endif /* _DEBUG */

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
	DISABLE_INTS;
	SCHED_ENQUEUE(proc);
	ENABLE_INTS;

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

	/* Remember old process to save its context later */
	old_process = CurrentProcess;
	CurrentProcess = NULL;

	/* Poll on the ready queue for the first ready process
	 */
	for(;;) /* forever */
	{
		/* Do CPU specific idle processing (ARGH, should be moved to the end of the loop!) */
		SCHEDULER_IDLE;

		DISABLE_INTS;
		if (!ISLISTEMPTY(&ProcReadyList))
		{
			/* Get process from ready list */
			CurrentProcess = (Process *)ProcReadyList.head;
			REMOVE((Node *)CurrentProcess);
			ENABLE_INTS;
			break;
		}
		ENABLE_INTS;
	}

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

	CurrentProcess = NULL;
	proc_schedule();
	/* not reached */
}


/*!
 * Co-operative context switch
 */
void proc_switch(void)
{
	DISABLE_INTS;
	SCHED_ENQUEUE(CurrentProcess);
	ENABLE_INTS;
	proc_schedule();
}


/*!
 * Get the pointer to the current process
 */
struct Process *proc_current(void)
{
	return CurrentProcess;
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
	proc_new(proc_test_thread1, sizeof(proc_test_stack1), proc_test_stack1);
	proc_new(proc_test_thread2, sizeof(proc_test_stack2), proc_test_stack2);
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

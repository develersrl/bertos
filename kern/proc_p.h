/*!
 * \file
 * <!--
 * Copyright 2001,2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 1999,2000,2001 Bernardo Innocenti <bernie@develer.com>
 * This file is part of DevLib - See devlib/README for information.
 * -->
 *
 * \brief Internal scheduler structures and definitions for processes.
 *
 * \version $Id$
 *
 * \author Bernardo Innocenti <bernie@develer.com>
 */

/*
 * $Log$
 * Revision 1.2  2004/06/03 11:27:09  bernie
 * Add dual-license information.
 *
 * Revision 1.1  2004/05/23 17:27:00  bernie
 * Import kern/ subdirectory.
 *
 * Revision 1.3  2004/05/14 12:52:13  rasky
 * Importato supporto kernel per AVR da Stefano
 *
 * Revision 1.2  2004/04/28 16:13:49  rasky
 * proc_schedule() is now semi-private (used only within the kernel)
 *
 * Revision 1.1  2004/04/26 18:02:40  rasky
 * Importato microkernel
 *
 * Revision 1.1  2004/04/04 17:40:26  aleph
 * Add multithreading kernel
 *
 */

#ifndef KERN_PROC_P_H
#define KERN_PROC_P_H

#include "compiler.h"
#include "config.h"
#include "config_kern.h"
#include <mware/list.h>
#include <drv/timer.h>


typedef struct Process
{
	Node         link;        /*!< Link Process into scheduler lists */
	cpustack_t  *stack;       /*!< Per-process SP */

#if CONFIG_KERN_SIGNALS
	sigset_t     sig_wait;    /*!< Signals the process is waiting for */
	sigset_t     sig_recv;    /*!< Received signals */
#endif

#if CONFIG_KERN_TIMER
	struct Timer proc_timer;  /*!< Process own timer */
#endif

#if CONFIG_KERN_HEAP
	uint16_t     flags;       /*!< Flags */
	cpustack_t  *stack_base;  /*!< Base of process stack */
	size_t       stack_size;  /*!< Size of process stack */
#endif
} Process;


/*!
 * \name Flags for Process.flags
 * \{
 */
#define PF_FREESTACK  BV(0)  /*!< Free the stack when process dies */
/*\}*/


/*! Track running processes */
extern REGISTER Process	*CurrentProcess;

/*! Track ready processes */
extern REGISTER List     ProcReadyList;


/*!
 * Enqueue a task in the ready list
 */
#define SCHED_ENQUEUE(proc)  ADDTAIL(&ProcReadyList, &(proc)->link)

/*! Schedule to another process *without* adding the current to the ready list */
void proc_schedule(void);

#endif /* KERN_PROC_P_H */


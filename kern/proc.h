/**
 * \file
 * <!--
 * Copyright 2001,2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 1999,2000,2001 Bernardo Innocenti <bernie@develer.com>
 * All Rights Reserved.
 * -->
 *
 * \brief Process scheduler (public interface).
 *
 * \version $Id$
 *
 * \author Bernardo Innocenti <bernie@develer.com>
 */

/*
 * $Log$
 * Revision 1.1  2004/05/23 17:27:00  bernie
 * Import kern/ subdirectory.
 *
 */

#ifndef KERN_PROC_H
#define KERN_PROC_H

#include "compiler.h"
#include "cpu.h"
#include "config_kern.h"

/* Fwd decl */
struct Process;

/* Task scheduling services */
void proc_init(void);
struct Process *proc_new(void (*entry)(void), size_t stacksize, cpustack_t *stack);
void proc_exit(void);
void proc_switch(void);
void proc_test(void);
struct Process* proc_current(void);

#if CONFIG_KERN_PREEMPTIVE
	#define	FORBID proc_forbid()
	#define	PERMIT proc_permit()
#else
	#define FORBID
	#define PERMIT
#endif

#endif /* KERN_PROC_H */


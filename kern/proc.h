/*!
 * \file
 * <!--
 * Copyright 2001,2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 1999,2000,2001 Bernardo Innocenti <bernie@develer.com>
 * This file is part of DevLib - See devlib/README for information.
 * -->
 *
 * \brief Process scheduler (public interface).
 *
 * \version $Id$
 *
 * \author Bernardo Innocenti <bernie@develer.com>
 */

/*#*
 *#* $Log$
 *#* Revision 1.7  2004/10/19 08:54:55  bernie
 *#* Define forbid_cnt.
 *#*
 *#* Revision 1.6  2004/10/03 20:44:18  bernie
 *#* Remove stale declarations (moved to monitor.h).
 *#*
 *#* Revision 1.2  2004/06/03 11:27:09  bernie
 *#* Add dual-license information.
 *#*
 *#* Revision 1.1  2004/05/23 17:27:00  bernie
 *#* Import kern/ subdirectory.
 *#*
 *#*/

#ifndef KERN_PROC_H
#define KERN_PROC_H

#include "compiler.h"
#include "cpu.h"
#include "config_kern.h"

/* Fwd decl */
struct Process;

/* Task scheduling services */
void proc_init(void);
struct Process *proc_new_with_name(const char* name, void (*entry)(void), IPTR data, size_t stacksize, cpustack_t *stack);

#if !CONFIG_KERN_MONITOR
	#define proc_new(entry,data,size,stack) proc_new_with_name(NULL,(entry),(data),(size),(stack))
#else
	#define proc_new(entry,data,size,stack) proc_new_with_name(#entry,(entry),(data),(size),(stack))
#endif

void proc_exit(void);
void proc_switch(void);
void proc_test(void);
struct Process* proc_current(void);
IPTR proc_current_user_data(void);
void proc_rename(struct Process* proc, const char* name);

#if CONFIG_KERN_PREEMPTIVE
	void proc_forbid(void);
	void proc_permit(void);
#else
	INLINE void proc_forbid(void) { /* nop */ }
	INLINE void proc_permit(void) { /* nop */ }
#endif

/*!
 * Execute a block of \a CODE atomically with respect to task scheduling.
 */
#define PROC_ATOMIC(CODE) \
	do { \
		proc_forbid(); \
		CODE; \
		proc_permit(); \
	} while(0)

#endif /* KERN_PROC_H */


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
 * \brief Process scheduler (public interface).
 *
 * \version $Id$
 *
 * \author Bernardo Innocenti <bernie@develer.com>
 */
#ifndef KERN_PROC_H
#define KERN_PROC_H

#include <cfg/cfg_kern.h>
#include <cfg/compiler.h>

#include <cpu/irq.h>

/* Fwd decl */
struct Process;

/* Task scheduling services */
void proc_init(void);
struct Process *proc_new_with_name(const char* name, void (*entry)(void), iptr_t data, size_t stacksize, cpustack_t *stack);

#if !CONFIG_KERN_MONITOR
	#define proc_new(entry,data,size,stack) proc_new_with_name(NULL,(entry),(data),(size),(stack))
#else
	#define proc_new(entry,data,size,stack) proc_new_with_name(#entry,(entry),(data),(size),(stack))
#endif

void proc_exit(void);
void proc_switch(void);
void proc_test(void);
struct Process *proc_current(void);
iptr_t proc_current_user_data(void);
void proc_rename(struct Process *proc, const char* name);

#if CONFIG_KERN_PREEMPTIVE
	void proc_forbid(void);
	void proc_permit(void);
#else
	INLINE void proc_forbid(void) { /* nop */ }
	INLINE void proc_permit(void) { /* nop */ }
#endif

/**
 * Execute a block of \a CODE atomically with respect to task scheduling.
 */
#define PROC_ATOMIC(CODE) \
	do { \
		proc_forbid(); \
		CODE; \
		proc_permit(); \
	} while(0)

#endif /* KERN_PROC_H */

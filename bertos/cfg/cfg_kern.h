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
 * \brief Kernel configuration parameters
 *
 * \version $Id$
 *
 * \author Bernardo Innocenti <bernie@develer.com>
 */

#ifndef CFG_KERN_H
#define CFG_KERN_H

#include <cfg/cfg_arch.h>  /* ARCH_EMUL */

/**
 * Multithreading kernel.
 */
#define CONFIG_KERNEL 0

/**
 * \name Modules activation
 *
 * \{
 */
/*      Module/option          Active    Dependencies */
#define CONFIG_KERN_SCHED       (1)
#define CONFIG_KERN_SIGNALS     (1    && CONFIG_KERN_SCHED)
#define CONFIG_KERN_TIMER       (1)
#define CONFIG_KERN_HEAP        (0)
#define CONFIG_KERN_SEMAPHORES  (0    && CONFIG_KERN_SIGNALS)
#define CONFIG_KERN_MONITOR     (1    && CONFIG_KERN_SCHED)
/*\}*/

/* EXPERIMENTAL */
#define CONFIG_KERN_PREEMPTIVE  (0    && CONFIG_KERN_SCHED && CONFIG_KERN_TIMER)

#define CONFIG_KERN_QUANTUM     50    /**< Time sharing quantum in timer ticks. */

#if (ARCH & ARCH_EMUL)
	/* We need a large stack because system libraries are bloated */
	#define CONFIG_PROC_DEFSTACKSIZE  65536
#else
	/**
	 * Default stack size for each thread, in bytes.
	 *
	 * The goal here is to allow a minimal task to save all of its
	 * registers twice, plus push a maximum of 32 variables on the
	 * stack.
	 *
	 * The actual size computed by the default formula is:
	 *   AVR:    102
	 *   i386:   156
	 *   ARM:    164
	 *   x86_64: 184
	 *
	 * Note that on most 16bit architectures, interrupts will also
	 * run on the stack of the currently running process.  Nested
	 * interrupts will greatly increases the amount of stack space
	 * required per process.  Use irqmanager to minimize stack
	 * usage.
	 */
	#define CONFIG_PROC_DEFSTACKSIZE  \
	    (CPU_SAVED_REGS_CNT * 2 * sizeof(cpustack_t) \
	    + 32 * sizeof(int))
#endif

/* OBSOLETE */
#define CONFIG_KERN_DEFSTACKSIZE CONFIG_PROC_DEFSTACKSIZE

/* Memory fill codes to help debugging */
#if CONFIG_KERN_MONITOR
	#include <cpu/types.h>
	#if (SIZEOF_CPUSTACK_T == 1)
		/* 8bit cpustack_t */
		#define CONFIG_KERN_STACKFILLCODE  0xA5
		#define CONFIG_KERN_MEMFILLCODE    0xDB
	#elif (SIZEOF_CPUSTACK_T == 2)
		/* 16bit cpustack_t */
		#define CONFIG_KERN_STACKFILLCODE  0xA5A5
		#define CONFIG_KERN_MEMFILLCODE    0xDBDB
	#elif (SIZEOF_CPUSTACK_T == 4)
		/* 16bit cpustack_t */
		#define CONFIG_KERN_STACKFILLCODE  0xA5A5A5A5UL
		#define CONFIG_KERN_MEMFILLCODE    0xDBDBDBDBUL
	#elif (SIZEOF_CPUSTACK_T == 8)
		/* 16bit cpustack_t */
		#define CONFIG_KERN_STACKFILLCODE  0xA5A5A5A5A5A5A5A5UL
		#define CONFIG_KERN_MEMFILLCODE    0xDBDBDBDBDBDBDBDBUL
	#else
		#error No cpustack_t size supported!
	#endif
#endif

#endif /*  CFG_KERN_H */

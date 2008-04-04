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
 * Copyright 2008 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief Kernel scheduler macros.
 *
 * \version $Id$
 *
 * \author Francesco Sacchi <batt@develer.com>
 * \author Stefano Fedrigo <aleph@develer.com>
 */

#ifndef CPU_ARM_HW_SWITCH_H
#define CPU_ARM_HW_SWITCH_H

#include <kern/proc_p.h>

/**
 * Interrupt entry point.
 * Needed because AT91 uses an Interrupt Controller with auto-vectoring.
 */
#define SCHEDULER_IRQ_ENTRY \
	asm volatile("sub   lr, lr, #4          \n\t"  /* Adjust LR */ \
	             "stmfd sp!, {r0}           \n\t"  /* Save r0 */ \
	             "stmfd sp, {sp}^           \n\t"  /* Save user SP */ \
	             "sub   sp, sp, #4          \n\t"  /* Decrement irq SP, writeback is illegal */ \
	             "ldmfd sp!, {r0}           \n\t"  /* Restore user SP immedately in r0 */ \
	             "stmfd r0!, {lr}		\n\t"  /* Store system LR in user stack */ \
	             "stmfd r0, {r1-r12,lr}^    \n\t"  /* Store registers on user stack (user LR too) */ \
	             "sub   r0, r0, #52         \n\t"  /* Decrement r0, writeback is illegal */ \
	             "ldmfd sp!, {r1}           \n\t"  /* Restore r0 */ \
	             "stmfd r0!, {r1}           \n\t"  /* Store r0 in user stack too */ \
	             "mrs   r1, spsr            \n\t"  /* Save SPSR... */ \
	             "stmfd r0!, {r1}           \n\t"  /*  ... in user stack */ \
	             "ldr   r1, =CurrentProcess \n\t"  /* Load in r1 &CurrentProcess->stack */ \
	             "ldr   r1, [r1, %0]        \n\t"  \
	             "str   r0, [r1]            \n\t"  /* Store the process SP */ \
	             "sub   fp, sp, #4          \n\t"  /* Store the process SP */ \
	             : /* no output */ \
	             : "n" (offsetof(Process, stack)) \
	)


#define SCHEDULER_IRQ_EXIT \
	asm volatile("ldr   lr, =CurrentProcess \n\t"  /* Load &CurrentProcess->stack */ \
	             "ldr   lr, [lr, %0]        \n\t"  \
	             "ldr   lr, [lr]            \n\t"  /* Load current process SP */ \
	             "ldr   r0, =0xFFFFF000     \n\t"  /* End of interrupt for AT91 */ \
	             "str   r0, [r0, #0x130]    \n\t"  /* */ \
	             "ldmfd lr!, {r0}           \n\t"  /* Pop status reg */ \
	             "msr   spsr, r0            \n\t"  /* ... */ \
	             "ldmfd lr, {r0-r12,lr}^    \n\t"  /* Restore user regs */ \
	             "add   lr, lr, #56         \n\t"  /* 52 + irq link register (extracted below) */ \
		     "stmfd sp!, {lr}           \n\t"  /* Push user stack pointer in irq stack */ \
		     "ldmfd sp,  {sp}^          \n\t"  /* Restore user SP */ \
		     "sub   sp, sp, #4          \n\t"  /* Align irq SP */ \
		     "ldmdb lr, {pc}^           \n\t"  /* And return to user space (We use ldmdb cause lr is sp+4) */ \
	             : /* no output */ \
	             : "n" (offsetof(Process, stack))  \
	)

#endif /* CPU_ARM_HW_SWITCH_H */

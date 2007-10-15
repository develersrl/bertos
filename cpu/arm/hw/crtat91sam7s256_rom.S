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
 * Copyright 2007 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \version $Id: $
 *
 * \author Francesco Sacchi <batt@develer.com>
 *
 * \brief AT91SAM7S256 CRT, adapted from NUt/OS, see license below.
 */

/*
 * Copyright (C) 2005-2007 by egnite Software GmbH. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holders nor the names of
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY EGNITE SOFTWARE GMBH AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL EGNITE
 * SOFTWARE GMBH OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * For additional information see http://www.ethernut.de/
 *
 */

#include <cfg/clock.h>
#include <cfg/memory.h>

#include <arch/arm.h>

#ifndef PLL_MUL_VAL
#define PLL_MUL_VAL 72
#endif

#ifndef PLL_DIV_VAL
#define PLL_DIV_VAL 14
#endif

#if MASTER_CLOCK_PRES == 1
#define AT91MCK_PRES    PMC_PRES_CLK
#elif MASTER_CLOCK_PRES == 4
#define AT91MCK_PRES    PMC_PRES_CLK_4
#elif MASTER_CLOCK_PRES == 8
#define AT91MCK_PRES    PMC_PRES_CLK_8
#elif MASTER_CLOCK_PRES == 16
#define AT91MCK_PRES    PMC_PRES_CLK_16
#elif MASTER_CLOCK_PRES == 32
#define AT91MCK_PRES    PMC_PRES_CLK_32
#elif MASTER_CLOCK_PRES == 64
#define AT91MCK_PRES    PMC_PRES_CLK_64
#else
#define AT91MCK_PRES    PMC_PRES_CLK_2
#endif

#ifndef IRQ_STACK_SIZE
#define IRQ_STACK_SIZE  512
#endif

#ifndef FIQ_STACK_SIZE
#define FIQ_STACK_SIZE  256
#endif

#ifndef ABT_STACK_SIZE
#define ABT_STACK_SIZE  128
#endif

#ifndef UND_STACK_SIZE
#define UND_STACK_SIZE  128
#endif


/*
 * Section 0: Vector table and reset entry.
 */
        .section .init0,"ax",%progbits

        .global __vectors
__vectors:
        ldr     pc, [pc, #24]   /* Reset */
        ldr     pc, [pc, #24]   /* Undefined instruction */
        ldr     pc, [pc, #24]   /* Software interrupt */
        ldr     pc, [pc, #24]   /* Prefetch abort */
        ldr     pc, [pc, #24]   /* Data abort */
        ldr     pc, [pc, #24]   /* Reserved */

        /*
         * On IRQ the PC will be loaded from AIC_IVR, which
         * provides the address previously set in AIC_SVR.
         * The interrupt routine will be called in ARM_MODE_IRQ
         * with IRQ disabled and FIQ unchanged.
         */
        ldr     pc, [pc, #-0xF20]   /* Interrupt request, auto vectoring. */
        ldr     pc, [pc, #-0xF20]   /* Fast interrupt request, auto vectoring. */

        .word   _start
        .word   __undef
        .word   __swi
        .word   __prefetch_abort
        .word   __data_abort

        .weak   __undef
        .set    __undef, __xcpt_dummy
        .weak   __swi
        .set    __swi, __xcpt_dummy
        .weak   __prefetch_abort
        .set    __prefetch_abort, __xcpt_dummy
        .weak   __data_abort
        .set    __data_abort, __xcpt_dummy

        .global __xcpt_dummy
__xcpt_dummy:
        b       __xcpt_dummy

        .ltorg
/*
 * Section 1: Hardware initialization.
 */
        .section .init1, "ax", %progbits
        .globl	_start
_start:

        /*
         * Use 2 cycles for flash access.
         */
        ldr     r1, =MC_BASE
        mov     r0, #MC_FWS_2R3W
        str     r0, [r1, #MC_FMR_OFF]

        /*
         * Disable all interrupts. Useful for debugging w/o target reset.
         */
        ldr     r1, =AIC_BASE
        mvn     r0, #0
        str     r0, [r1, #AIC_EOICR_OFF]
        str     r0, [r1, #AIC_IDCR_OFF]

        /*
         * The watchdog is enabled after processor reset. Disable it.
         */
        ldr     r1, =WDT_BASE
        ldr     r0, =WDT_WDDIS
        str     r0, [r1, #WDT_MR_OFF]

        /*
         * Enable the main oscillator. Set startup time of 6 * 8 slow 
         * clock cycles and wait until oscillator is stabilized.
         */
        ldr     r1, =PMC_BASE
        mov     r0, #(6 << 8)
        orr     r0, r0, #CKGR_MOSCEN
        str     r0, [r1, #CKGR_MOR_OFF]
wait_moscs:
        ldr     r0, [r1, #PMC_SR_OFF]
        tst     r0, #PMC_MOSCS
        beq     wait_moscs

        /*
         * Set PLL:
         * PLLfreq = crystal / divider * (multiplier + 1)
         * Wait 28 clock cycles until PLL is locked.
         */
        ldr     r0, =((PLL_MUL_VAL << CKGR_MUL_LSB) | (28 << CKGR_PLLCOUNT_LSB) | (PLL_DIV_VAL << CKGR_DIV_LSB))

        str     r0, [r1, #CKGR_PLLR_OFF]
wait_lock:
        ldr     r0, [r1, #PMC_SR_OFF]
        tst     r0, #PMC_LOCK
        beq     wait_lock

        /*
         * Set master clock prescaler.
         */
        mov     r0, #AT91MCK_PRES
        str     r0, [r1, #PMC_MCKR_OFF]
wait_presrdy:
        ldr     r0, [r1, #PMC_SR_OFF]
        tst     r0, #PMC_MCKRDY
        beq     wait_presrdy

        /*
         * Switch to PLL clock. Trying to set this together with the
         * prescaler fails for unknown reason.
         */
        ldr     r0, [r1, #PMC_MCKR_OFF]
        orr     r0, r0, #PMC_CSS_PLL_CLK
        str     r0, [r1, #PMC_MCKR_OFF]
wait_pllsel:
        ldr     r0, [r1, #PMC_SR_OFF]
        tst     r0, #PMC_MCKRDY
        beq     wait_pllsel

        /*
         * Enable external reset key.
         */
        ldr     r0, =(RSTC_KEY | RSTC_URSTEN)
        ldr     r1, =RSTC_MR
        str     r0, [r1, #0]
        
        b       __set_stacks

        .ltorg


/*
 * Section 2: Set stack pointers.
 */
        .section .init2,"ax",%progbits        
        .global __set_stacks
__set_stacks:

        /*
         * Set exception stack pointers and enable interrupts.
         */
        ldr     r0, =__exp_stack
        msr     CPSR_c, #ARM_MODE_FIQ | 0xC0
        mov     r13, r0
        sub     r0, r0, #FIQ_STACK_SIZE
        msr     CPSR_c, #ARM_MODE_IRQ | 0xC0
        mov     r13, r0
        sub     r0, r0, #IRQ_STACK_SIZE
        msr     CPSR_c, #ARM_MODE_ABORT | 0xC0
        mov     r13, r0
        sub     r0, r0, #ABT_STACK_SIZE
        msr     CPSR_c, #ARM_MODE_UNDEF | 0xC0
        mov     r13, r0
        sub     r0, r0, #UND_STACK_SIZE
        msr     CPSR_c, #ARM_MODE_SVC | 0xC0
        mov     r13, r0
        b       __enter_mode
        
        .ltorg

/*
 * Section 3: Enter user mode.
 */
        .section .init3,"ax",%progbits

        .global __enter_mode
__enter_mode:

        b       __clear_bss

        .ltorg

/*
 * Section 4: Clear bss and copy data.
 */
        .section .init4,"ax",%progbits
        .global __clear_bss
__clear_bss:

        ldr     r1, =__bss_start
        ldr     r2, =__bss_end
        ldr     r3, =0

_40:
        cmp     r1, r2
        strne   r3, [r1], #+4
        bne     _40

        /*
         * Relocate .data section (Copy from ROM to RAM).
         */
        ldr     r1, =__etext
        ldr     r2, =__data_start
        ldr     r3, =__data_end

_41:
        cmp     r2, r3
        ldrlo   r0, [r1], #4
        strlo   r0, [r2], #4
        blo     _41

        /*
         * Initialize user stack pointer.
         */
        ldr     r13, =__stack
        b       __call_rtos

        .ltorg

/*
 * Section 5: Call RTOS
 */
        .section .init5,"ax",%progbits
        .global __call_rtos
__call_rtos:

        /*
         * Jump to Nut/OS initialization.
         */
        ldr     r0, =NutInit
        bx      r0

End:
        b       End

        .ltorg

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
 * Copyright 2008 Bernie Innocenti <bernie@codewiz.org>
 * Copyright 2004, 2005, 2006, 2007, 2008 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2004 Giovanni Bajo
 *
 * -->
 *
 * \brief CPU-specific stack frame handling macros.
 *
 * These are mainly used by the portable part of the scheduler
 * to work with the process stack frames.
 *
 * \author Giovanni Bajo <rasky@develer.com>
 * \author Bernie Innocenti <bernie@codewiz.org>
 * \author Stefano Fedrigo <aleph@develer.com>
 * \author Francesco Sacchi <batt@develer.com>
 */
#ifndef CPU_FRAME_H
#define CPU_FRAME_H

#include <cpu/detect.h>

#include "cfg/cfg_arch.h"      /* ARCH_EMUL */
#include <cfg/compiler.h>      /* for uintXX_t */

#if CPU_X86

	#define CPU_SAVED_REGS_CNT      7
	#define CPU_STACK_GROWS_UPWARD  0
	#define CPU_SP_ON_EMPTY_SLOT	0

#elif CPU_ARM

	#define CPU_SAVED_REGS_CNT     9
	#define CPU_STACK_GROWS_UPWARD 0
	#define CPU_SP_ON_EMPTY_SLOT   0

	/**
	 * Initialization value for registers in stack frame.
	 * The register index is not directly corrispondent to CPU
	 * register numbers, but is related to how are pushed to
	 * stack (\see asm_switch_context).
	 * Index (CPU_SAVED_REGS_CNT - 1) is the CPSR register,
	 * the initial value is set to:
	 * - All flags (N, Z, C, V) set to 0.
	 * - IRQ and FIQ enabled.
	 * - ARM state.
	 * - CPU in Supervisor Mode (SVC).
	 */
	#define CPU_REG_INIT_VALUE(reg) (reg == (CPU_SAVED_REGS_CNT - 1) ? 0x13 : 0)

#elif CPU_PPC

	#define CPU_SAVED_REGS_CNT     1
	#define CPU_STACK_GROWS_UPWARD 0
	#define CPU_SP_ON_EMPTY_SLOT   0

#elif CPU_DSP56K

	#define CPU_SAVED_REGS_CNT      8
	#define CPU_STACK_GROWS_UPWARD  1
	#define CPU_SP_ON_EMPTY_SLOT	0

#elif CPU_AVR

	#define CPU_SAVED_REGS_CNT     19
	#define CPU_STACK_GROWS_UPWARD  0
	#define CPU_SP_ON_EMPTY_SLOT    1

	/**
	 * Initialization value for registers in stack frame.
	 * The register index is not directly corrispondent to CPU
	 * register numbers. Index 0 is the SREG register: the initial
	 * value is all 0 but the interrupt bit (bit 7).
	 */
	#define CPU_REG_INIT_VALUE(reg) (reg == 0 ? 0x80 : 0)

#else
	#error No CPU_... defined.
#endif

#ifndef CPU_STACK_GROWS_UPWARD
	#error CPU_STACK_GROWS_UPWARD should have been defined to either 0 or 1
#endif

#ifndef CPU_SP_ON_EMPTY_SLOT
	#error CPU_SP_ON_EMPTY_SLOT should have been defined to either 0 or 1
#endif

/// Default for macro not defined in the right arch section
#ifndef CPU_REG_INIT_VALUE
	#define CPU_REG_INIT_VALUE(reg)     0
#endif

/*
 * Support stack handling peculiarities of a few CPUs.
 *
 * Most processors let their stack grow downward and
 * keep SP pointing at the last pushed value.
 */
#if !CPU_STACK_GROWS_UPWARD
	#if !CPU_SP_ON_EMPTY_SLOT
		/* Most microprocessors (x86, m68k...) */
		#define CPU_PUSH_WORD(sp, data) \
			do { *--(sp) = (data); } while (0)
		#define CPU_POP_WORD(sp) \
			(*(sp)++)
	#else
		/* AVR insanity */
		#define CPU_PUSH_WORD(sp, data) \
			do { *(sp)-- = (data); } while (0)
		#define CPU_POP_WORD(sp) \
			(*++(sp))
	#endif

#else /* CPU_STACK_GROWS_UPWARD */

	#if !CPU_SP_ON_EMPTY_SLOT
		/* DSP56K and other weirdos */
		#define CPU_PUSH_WORD(sp, data) \
			do { *++(sp) = (cpustack_t)(data); } while (0)
		#define CPU_POP_WORD(sp) \
			(*(sp)--)
	#else
		#error I bet you cannot find a CPU like this
	#endif
#endif


#if CPU_DSP56K
	/*
	 * DSP56k pushes both PC and SR to the stack in the JSR instruction, but
	 * RTS discards SR while returning (it does not restore it). So we push
	 * 0 to fake the same context.
	 */
	#define CPU_PUSH_CALL_FRAME(sp, func) \
		do { \
			CPU_PUSH_WORD((sp), (func)); \
			CPU_PUSH_WORD((sp), 0x100); \
		} while (0);

#elif CPU_AVR
	/*
	 * On AVR, addresses are pushed into the stack as little-endian, while
	 * memory accesses are big-endian (actually, it's a 8-bit CPU, so there is
	 * no natural endianess).
	 */
	#define CPU_PUSH_CALL_FRAME(sp, func) \
		do { \
			uint16_t funcaddr = (uint16_t)(func); \
			CPU_PUSH_WORD((sp), funcaddr); \
			CPU_PUSH_WORD((sp), funcaddr>>8); \
		} while (0)

	/*
	 * If the kernel is in idle-spinning, the processor executes:
	 *
	 * IRQ_ENABLE;
	 * CPU_IDLE;
	 * IRQ_DISABLE;
	 *
	 * IRQ_ENABLE is translated in asm as "sei" and IRQ_DISABLE as "cli".
	 * We could define CPU_IDLE to expand to none, so the resulting
	 * asm code would be:
	 *
	 * sei;
	 * cli;
	 *
	 * But Atmel datasheet states:
	 * "When using the SEI instruction to enable interrupts,
	 * the instruction following SEI will be executed *before*
	 * any pending interrupts", so "cli" is executed before any
	 * pending interrupt with the result that IRQs will *NOT*
	 * be enabled!
	 * To ensure that IRQ will run a NOP is required.
	 */
	#define CPU_IDLE NOP

#elif CPU_PPC

	#define CPU_PUSH_CALL_FRAME(sp, func) \
		do { \
			CPU_PUSH_WORD((sp), (cpustack_t)(func)); /* LR -> 8(SP) */ \
			CPU_PUSH_WORD((sp), 0);                  /* CR -> 4(SP) */ \
		} while (0)

#else
	#define CPU_PUSH_CALL_FRAME(sp, func) \
		CPU_PUSH_WORD((sp), (cpustack_t)(func))
#endif

/**
 * \def CPU_IDLE
 *
 * \brief Invoked by the scheduler to stop the CPU when idle.
 *
 * This hook can be redefined to put the CPU in low-power mode, or to
 * profile system load with an external strobe, or to save CPU cycles
 * in hosted environments such as emulators.
 */
#ifndef CPU_IDLE
	#if defined(ARCH_EMUL) && (ARCH & ARCH_EMUL)
		/* This emulator hook should yield the CPU to the host.  */
		EXTERN_C_BEGIN
		void emul_idle(void);
		EXTERN_C_END
		#define CPU_IDLE emul_idle()
	#else /* !ARCH_EMUL */
		#define CPU_IDLE do { /* nothing */ } while (0)
	#endif /* !ARCH_EMUL */
#endif /* !CPU_IDLE */

#endif /* CPU_ATTR_H */

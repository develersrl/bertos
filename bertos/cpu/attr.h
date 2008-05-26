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
 * Copyright 2004, 2005, 2006, 2007 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2004 Giovanni Bajo
 *
 * -->
 *
 * \brief CPU-specific attributes.
 *
 * \author Giovanni Bajo <rasky@develer.com>
 * \author Bernardo Innocenti <bernie@develer.com>
 * \author Stefano Fedrigo <aleph@develer.com>
 * \author Francesco Sacchi <batt@develer.com>
 */
#ifndef CPU_ATTR_H
#define CPU_ATTR_H

#include "detect.h"
#include <cfg/compiler.h> /* for uintXX_t */
#include <cfg/arch_config.h>  /* ARCH_EMUL */

#include "appconfig.h" // CONFIG_FAST_MEM

/**
 * \name Macros for determining CPU endianness.
 * \{
 */
#define CPU_BIG_ENDIAN    0x1234
#define CPU_LITTLE_ENDIAN 0x3412 /* Look twice, pal. This is not a bug. */
/*\}*/

/** Macro to include cpu-specific versions of the headers. */
#define CPU_HEADER(module)          PP_STRINGIZE(drv/PP_CAT3(module, _, CPU_ID).h)

/** Macro to include cpu-specific versions of implementation files. */
#define CPU_CSOURCE(module)         PP_STRINGIZE(drv/PP_CAT3(module, _, CPU_ID).c)


#if CPU_I196

	#define NOP                     nop_instruction()

	#define CPU_REG_BITS            16
	#define CPU_REGS_CNT            16
	#define CPU_STACK_GROWS_UPWARD  0
	#define CPU_SP_ON_EMPTY_SLOT	0
	#define CPU_BYTE_ORDER          CPU_LITTLE_ENDIAN
	#define CPU_HARVARD		0

#elif CPU_X86

	#define NOP                     asm volatile ("nop")

	#define CPU_REGS_CNT            7
	#define CPU_SAVED_REGS_CNT      7
	#define CPU_STACK_GROWS_UPWARD  0
	#define CPU_SP_ON_EMPTY_SLOT	0
	#define CPU_BYTE_ORDER          CPU_LITTLE_ENDIAN
	#define CPU_HARVARD		0

	#if CPU_X86_64
		#define CPU_REG_BITS    64

		#ifdef __WIN64__
			/* WIN64 is an IL32-P64 weirdo. */
			#define SIZEOF_LONG  4
		#endif
	#else
		#define CPU_REG_BITS    32
	#endif

#elif CPU_ARM

	/* Register counts include SREG too */
	#define CPU_REG_BITS           32
	#define CPU_REGS_CNT           16
	#define CPU_SAVED_REGS_CNT     9
	#define CPU_STACK_GROWS_UPWARD 0
	#define CPU_SP_ON_EMPTY_SLOT   0
	#define CPU_HARVARD            0

	#ifdef __IAR_SYSTEMS_ICC__
		#warning Check CPU_BYTE_ORDER
		#define CPU_BYTE_ORDER (__BIG_ENDIAN__ ? CPU_BIG_ENDIAN : CPU_LITTLE_ENDIAN)

		#define NOP            __no_operation()

	#else /* GCC and compatibles */

		#if defined(__ARMEB__)
  			#define CPU_BYTE_ORDER CPU_BIG_ENDIAN
  		#elif defined(__ARMEL__)
  			#define CPU_BYTE_ORDER CPU_LITTLE_ENDIAN
		#else
			#error Unable to detect ARM endianness!
  		#endif

		#define NOP            asm volatile ("mov r0,r0" ::)

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

		#if CONFIG_FAST_MEM
			/**
			 * Function attribute for use with performance critical code.
			 *
			 * On the AT91 family, code residing in flash has wait states.
			 * Moving functions to the data section is a quick & dirty way
			 * to get them transparently copied to SRAM for zero-wait-state
			 * operation.
			 */
			#define FAST_FUNC __attribute__((section(".data")))

			/**
			 * Data attribute to move constant data to fast memory storage.
			 *
			 * \see FAST_FUNC
			 */
			#define FAST_RODATA __attribute__((section(".data")))

		#else // !CONFIG_FAST_MEM
			#define FAST_RODATA /**/
			#define FAST_FUNC /**/
		#endif

		/**
		 * Function attribute to declare an interrupt service routine.
		 */
		#define ISR_FUNC __attribute__((interrupt))

	#endif /* !__IAR_SYSTEMS_ICC_ */

#elif CPU_PPC
	#define NOP                 asm volatile ("nop" ::)

	/* Register counts include SREG too */
	#define CPU_REG_BITS           (CPU_PPC32 ? 32 : 64)
	#define CPU_REGS_CNT           FIXME
	#define CPU_SAVED_REGS_CNT     FIXME
	#define CPU_STACK_GROWS_UPWARD 0  //FIXME
	#define CPU_SP_ON_EMPTY_SLOT   0  //FIXME
	#define CPU_BYTE_ORDER         (__BIG_ENDIAN__ ? CPU_BIG_ENDIAN : CPU_LITTLE_ENDIAN)
	#define CPU_HARVARD            0

#elif CPU_DSP56K

	#define NOP                     asm(nop)

	#define CPU_REG_BITS            16
	#define CPU_REGS_CNT            FIXME
	#define CPU_SAVED_REGS_CNT      8
	#define CPU_STACK_GROWS_UPWARD  1
	#define CPU_SP_ON_EMPTY_SLOT	0
	#define CPU_BYTE_ORDER          CPU_BIG_ENDIAN
	#define CPU_HARVARD		1

	/* Memory is word-addessed in the DSP56K */
	#define CPU_BITS_PER_CHAR  16
	#define SIZEOF_SHORT        1
	#define SIZEOF_INT          1
	#define SIZEOF_LONG         2
	#define SIZEOF_PTR          1

#elif CPU_AVR

	#define NOP           asm volatile ("nop" ::)

	/* Register counts include SREG too */
	#define CPU_REG_BITS            8
	#define CPU_REGS_CNT           33
	#define CPU_SAVED_REGS_CNT     19
	#define CPU_STACK_GROWS_UPWARD  0
	#define CPU_SP_ON_EMPTY_SLOT    1
	#define CPU_BYTE_ORDER          CPU_LITTLE_ENDIAN
	#define CPU_HARVARD             1

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

/// Default for macro not defined in the right arch section
#ifndef CPU_REG_INIT_VALUE
	#define CPU_REG_INIT_VALUE(reg)     0
#endif

#ifndef CPU_STACK_GROWS_UPWARD
	#error CPU_STACK_GROWS_UPWARD should have been defined to either 0 or 1
#endif

#ifndef CPU_SP_ON_EMPTY_SLOT
	#error CPU_SP_ON_EMPTY_SLOT should have been defined to either 0 or 1
#endif

#ifndef FAST_FUNC
	/**
	 * Function attribute for use with performance critical code.
	 */
	#define FAST_FUNC /* */
#endif

#ifndef FAST_RODATA
	/**
	 * Data attribute to move constant data to fast memory storage.
	 */
	#define FAST_RODATA /* */
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
	#define CPU_PUSH_CALL_CONTEXT(sp, func) \
		do { \
			CPU_PUSH_WORD((sp), (func)); \
			CPU_PUSH_WORD((sp), 0x100); \
		} while (0);

#elif CPU_AVR
	/*
	 * In AVR, the addresses are pushed into the stack as little-endian, while
	 * memory accesses are big-endian (actually, it's a 8-bit CPU, so there is
	 * no natural endianess).
	 */
	#define CPU_PUSH_CALL_CONTEXT(sp, func) \
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

#else
	#define CPU_PUSH_CALL_CONTEXT(sp, func) \
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

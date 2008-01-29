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
 * \brief CPU-specific IRQ definitions.
 *
 * \author Giovanni Bajo <rasky@develer.com>
 * \author Bernardo Innocenti <bernie@develer.com>
 * \author Stefano Fedrigo <aleph@develer.com>
 * \author Francesco Sacchi <batt@develer.com>
 */
#ifndef CPU_IRQ_H
#define CPU_IRQ_H

#include "detect.h"
#include "types.h"

#include <cfg/compiler.h> /* for uintXX_t */

#if CPU_I196
	#define IRQ_DISABLE             disable_interrupt()
	#define IRQ_ENABLE              enable_interrupt()
#elif CPU_X86

	/* Get IRQ_* definitions from the hosting environment. */
	#include <cfg/os.h>
	#if OS_EMBEDDED
		#define IRQ_DISABLE             FIXME
		#define IRQ_ENABLE              FIXME
		#define IRQ_SAVE_DISABLE(x)     FIXME
		#define IRQ_RESTORE(x)          FIXME
	#endif /* OS_EMBEDDED */

#elif CPU_ARM


	#ifdef __IAR_SYSTEMS_ICC__

		#include <inarm.h>

		#if __CPU_MODE__ == 1 /* Thumb */
			/* Use stubs */
			extern cpuflags_t get_CPSR(void);
			extern void set_CPSR(cpuflags_t flags);
		#else
			#define get_CPSR __get_CPSR
			#define set_CPSR __set_CPSR
		#endif

		#define IRQ_DISABLE __disable_interrupt()
		#define IRQ_ENABLE  __enable_interrupt()

		#define IRQ_SAVE_DISABLE(x) \
		do { \
			(x) = get_CPSR(); \
			__disable_interrupt(); \
		} while (0)

		#define IRQ_RESTORE(x) \
		do { \
			set_CPSR(x); \
		} while (0)

		#define IRQ_ENABLED() \
			((bool)(get_CPSR() & 0xb0))

		#define BREAKPOINT  /* asm("bkpt 0") DOES NOT WORK */

	#else /* !__IAR_SYSTEMS_ICC__ */

		#define IRQ_DISABLE \
		do { \
			asm volatile ( \
				"mrs r0, cpsr\n\t" \
				"orr r0, r0, #0xc0\n\t" \
				"msr cpsr_c, r0" \
				::: "r0" \
			); \
		} while (0)

		#define IRQ_ENABLE \
		do { \
			asm volatile ( \
				"mrs r0, cpsr\n\t" \
				"bic r0, r0, #0xc0\n\t" \
				"msr cpsr_c, r0" \
				::: "r0" \
			); \
		} while (0)

		#define IRQ_SAVE_DISABLE(x) \
		do { \
			asm volatile ( \
				"mrs %0, cpsr\n\t" \
				"orr r0, %0, #0xc0\n\t" \
				"msr cpsr_c, r0" \
				: "=r" (x) \
				: /* no inputs */ \
				: "r0" \
			); \
		} while (0)

		#define IRQ_RESTORE(x) \
		do { \
			asm volatile ( \
				"msr cpsr_c, %0" \
				: /* no outputs */ \
				: "r" (x) \
			); \
		} while (0)

		#define CPU_READ_FLAGS() \
		({ \
			cpuflags_t sreg; \
			asm volatile ( \
				"mrs %0, cpsr\n\t" \
				: "=r" (sreg) \
				: /* no inputs */ \
			); \
			sreg; \
		})

		#define IRQ_ENABLED() ((CPU_READ_FLAGS() & 0xc0) != 0xc0)

	#endif /* !__IAR_SYSTEMS_ICC_ */

#elif CPU_PPC
	#define IRQ_DISABLE         FIXME
	#define IRQ_ENABLE          FIXME
	#define IRQ_SAVE_DISABLE(x) FIXME
	#define IRQ_RESTORE(x)      FIXME
	#define IRQ_ENABLED()       FIXME

#elif CPU_DSP56K

	#define BREAKPOINT              asm(debug)
	#define IRQ_DISABLE             do { asm(bfset #0x0200,SR); asm(nop); } while (0)
	#define IRQ_ENABLE              do { asm(bfclr #0x0200,SR); asm(nop); } while (0)

	#define IRQ_SAVE_DISABLE(x)  \
		do { (void)x; asm(move SR,x); asm(bfset #0x0200,SR); } while (0)
	#define IRQ_RESTORE(x)  \
		do { (void)x; asm(move x,SR); } while (0)

	static inline bool irq_running(void)
	{
		extern void *user_sp;
		return !!user_sp;
	}
	#define IRQ_RUNNING() irq_running()

	static inline bool irq_enabled(void)
	{
		uint16_t x;
		asm(move SR,x);
		return !(x & 0x0200);
	}
	#define IRQ_ENABLED() irq_enabled()

#elif CPU_AVR

	#define IRQ_DISABLE   asm volatile ("cli" ::)
	#define IRQ_ENABLE    asm volatile ("sei" ::)

	#define IRQ_SAVE_DISABLE(x) \
	do { \
		__asm__ __volatile__( \
			"in %0,__SREG__\n\t" \
			"cli" \
			: "=r" (x) : /* no inputs */ : "cc" \
		); \
	} while (0)

	#define IRQ_RESTORE(x) \
	do { \
		__asm__ __volatile__( \
			"out __SREG__,%0" : /* no outputs */ : "r" (x) : "cc" \
		); \
	} while (0)

	#define IRQ_ENABLED() \
	({ \
		uint8_t sreg; \
		__asm__ __volatile__( \
			"in %0,__SREG__\n\t" \
			: "=r" (sreg)  /* no inputs & no clobbers */ \
		); \
		(bool)(sreg & 0x80); \
	})
#else
	#error No CPU_... defined.
#endif

#ifndef IRQ_ENTRY
	#define IRQ_ENTRY() /* NOP */
#endif

#ifndef IRQ_EXIT
	#define IRQ_EXIT() /* NOP */
#endif


/**
 * Execute \a CODE atomically with respect to interrupts.
 *
 * \see IRQ_SAVE_DISABLE IRQ_RESTORE
 */
#define ATOMIC(CODE) \
	do { \
		cpuflags_t __flags; \
		IRQ_SAVE_DISABLE(__flags); \
		CODE; \
		IRQ_RESTORE(__flags); \
	} while (0)


#ifndef BREAKPOINT
#define BREAKPOINT /* nop */
#endif


#endif /* CPU_IRQ_H */

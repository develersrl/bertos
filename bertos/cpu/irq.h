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
 * \author Bernie Innocenti <bernie@codewiz.org>
 * \author Stefano Fedrigo <aleph@develer.com>
 * \author Francesco Sacchi <batt@develer.com>
 */
#ifndef CPU_IRQ_H
#define CPU_IRQ_H

#include "detect.h"
#include "types.h"

#include <kern/proc.h> /* proc_needPreempt() / proc_preempt() */

#include <cfg/compiler.h> /* for uintXX_t */
#include "cfg/cfg_proc.h" /* CONFIG_KERN_PREEMPT */

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

#elif CPU_ARM_LM3S1968

	#define IRQ_DISABLE asm volatile ("cpsid i" : : : "memory", "cc")
	#define IRQ_ENABLE asm volatile ("cpsie i" : : : "memory", "cc")

	#define IRQ_SAVE_DISABLE(x)					\
	({								\
		asm volatile (						\
			"mrs %0, PRIMASK\n"				\
			"cpsid i"					\
			: "=r" (x) : : "memory", "cc");			\
	})

	#define IRQ_RESTORE(x)						\
	({								\
		if (x)							\
			IRQ_DISABLE;					\
		else							\
			IRQ_ENABLE;					\
	})

	#define CPU_READ_FLAGS()					\
	({								\
		cpu_flags_t sreg;					\
		asm volatile (						\
			"mrs %0, PRIMASK\n\t"				\
			: "=r" (sreg) : : "memory", "cc");		\
		sreg;							\
	})

	#define IRQ_ENABLED() (!CPU_READ_FLAGS())

#elif CPU_ARM

	#ifdef __IAR_SYSTEMS_ICC__

		#include <inarm.h>

		#if __CPU_MODE__ == 1 /* Thumb */
			/* Use stubs */
			extern cpu_flags_t get_CPSR(void);
			extern void set_CPSR(cpu_flags_t flags);
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
			cpu_flags_t sreg; \
			asm volatile ( \
				"mrs %0, cpsr\n\t" \
				: "=r" (sreg) \
				: /* no inputs */ \
			); \
			sreg; \
		})

		#define IRQ_ENABLED() ((CPU_READ_FLAGS() & 0xc0) != 0xc0)

		#if CONFIG_KERN_PREEMPT
			EXTERN_C void asm_irq_switch_context(void);

			/**
			 * At the beginning of any ISR immediately ajust the
			 * return address and store all the caller-save
			 * registers (the ISR may change these registers that
			 * are shared with the user-context).
			 */
			#define IRQ_ENTRY() asm volatile ( \
						"sub	lr, lr, #4\n\t" \
						"stmfd	sp!, {r0-r3, ip, lr}\n\t")
			#define IRQ_EXIT()  asm volatile ( \
						"b	asm_irq_switch_context\n\t")
			/**
			 * Function attribute to declare an interrupt service
			 * routine.
			 *
			 * An ISR function must be declared as naked because we
			 * want to add our IRQ_ENTRY() prologue and IRQ_EXIT()
			 * epilogue code to handle the context switch and save
			 * all the registers (not only the callee-save).
			 *
			 */
			#define ISR_FUNC __attribute__((naked))

			/**
			 * The compiler cannot establish which
			 * registers actually need to be saved, because
			 * the interrupt can happen at any time, so the
			 * "normal" prologue and epilogue used for a
			 * generic function call are not suitable for
			 * the ISR.
			 *
			 * Using a naked function has the drawback that
			 * the stack is not automatically adjusted at
			 * this point, like a "normal" function call.
			 *
			 * So, an ISR can _only_ contain other function
			 * calls and they can't use the stack in any
			 * other way.
			 *
			 * NOTE: we need to explicitly disable IRQs after
			 * IRQ_ENTRY(), because the IRQ status flag is not
			 * masked by the hardware and an IRQ ack inside the ISR
			 * may cause the triggering of another IRQ before
			 * exiting from the current ISR.
			 *
			 * The respective IRQ_ENABLE is not necessary, because
			 * IRQs will be automatically re-enabled when restoring
			 * the context of the user task.
			 */
			#define DECLARE_ISR_CONTEXT_SWITCH(func)	\
				void ISR_FUNC func(void);		\
				static void __isr_##func(void);		\
				void ISR_FUNC func(void)		\
				{					\
					IRQ_ENTRY();			\
					IRQ_DISABLE;			\
					__isr_##func();			\
					IRQ_EXIT();			\
				}					\
				static void __isr_##func(void)
			/**
			 * Interrupt service routine prototype: can be used for
			 * forward declarations.
			 */
			#define ISR_PROTO_CONTEXT_SWITCH(func)	\
				void ISR_FUNC func(void)
			/**
			 * With task priorities enabled each ISR is used a point to
			 * check if we need to perform a context switch.
			 *
			 * Instead, without priorities a context switch can occur only
			 * when the running task expires its time quantum. In this last
			 * case, the context switch can only occur in the timer
			 * ISR, that must be always declared with the
			 * DECLARE_ISR_CONTEXT_SWITCH() macro.
			 */
			#if CONFIG_KERN_PRI
				#define DECLARE_ISR(func) \
					DECLARE_ISR_CONTEXT_SWITCH(func)

				#define ISR_PROTO(func) \
					ISR_PROTO_CONTEXT_SWITCH(func)
			#endif /* !CONFIG_KERN_PRI */
		#endif /* CONFIG_KERN_PREEMPT */

		#ifndef DECLARE_ISR
			#define DECLARE_ISR(func) \
				void __attribute__((interrupt)) func(void)
		#endif
		#ifndef DECLARE_ISR_CONTEXT_SWITCH
			#define DECLARE_ISR_CONTEXT_SWITCH(func) \
				void __attribute__((interrupt)) func(void)
		#endif
		#ifndef ISR_PROTO
			#define ISR_PROTO(func) \
				void __attribute__((interrupt)) func(void)
		#endif
		#ifndef ISR_PROTO_CONTEXT_SWITCH
			#define ISR_PROTO_CONTEXT_SWITCH(func)	\
				void __attribute__((interrupt)) func(void)
		#endif

	#endif /* !__IAR_SYSTEMS_ICC_ */

#elif CPU_PPC

	/* Get IRQ_* definitions from the hosting environment. */
	#include <cfg/os.h>
	#if OS_EMBEDDED
		#define IRQ_DISABLE         FIXME
		#define IRQ_ENABLE          FIXME
		#define IRQ_SAVE_DISABLE(x) FIXME
		#define IRQ_RESTORE(x)      FIXME
		#define IRQ_ENABLED()       FIXME
	#endif /* OS_EMBEDDED */

#elif CPU_DSP56K

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
	#if CONFIG_KERN_PREEMPT
		#define DECLARE_ISR_CONTEXT_SWITCH(vect)		\
			INLINE void __isr_##vect(void);			\
			ISR(vect)					\
			{						\
				__isr_##vect();				\
				IRQ_PREEMPT_HANDLER();			\
			}						\
			INLINE void __isr_##vect(void)

		/**
		 * With task priorities enabled each ISR is used a point to
		 * check if we need to perform a context switch.
		 *
		 * Instead, without priorities a context switch can occur only
		 * when the running task expires its time quantum. In this last
		 * case, the context switch can only occur in the timer ISR,
		 * that must be always declared with the
		 * DECLARE_ISR_CONTEXT_SWITCH() macro.
		 */
		#if CONFIG_KERN_PRI
			#define DECLARE_ISR(func) \
				DECLARE_ISR_CONTEXT_SWITCH(func)
			/**
			 * Interrupt service routine prototype: can be used for
			 * forward declarations.
			 */
			#define ISR_PROTO(func) \
				ISR_PROTO_CONTEXT_SWITCH(func)
		#endif /* !CONFIG_KERN_PRI */
	#endif

	#ifndef ISR_PROTO
		#define ISR_PROTO(vect)	ISR(vect)
	#endif
	#ifndef DECLARE_ISR
		#define DECLARE_ISR(vect) ISR(vect)
	#endif
	#ifndef DECLARE_ISR_CONTEXT_SWITCH
		#define DECLARE_ISR_CONTEXT_SWITCH(vect) ISR(vect)
	#endif
	#ifndef ISR_PROTO_CONTEXT_SWITCH
		#define ISR_PROTO_CONTEXT_SWITCH(func) ISR(vect)
	#endif

#else
	#error No CPU_... defined.
#endif

#ifdef IRQ_RUNNING
	/// Ensure callee is running within an interrupt
	#define ASSERT_IRQ_CONTEXT()  ASSERT(IRQ_RUNNING())

	/// Ensure callee is not running within an interrupt
	#define ASSERT_USER_CONTEXT() ASSERT(!IRQ_RUNNING())
#else
	#define ASSERT_USER_CONTEXT()  do {} while(0)
	#define ASSERT_IRQ_CONTEXT()   do {} while(0)
#endif

#ifdef IRQ_ENABLED
	/// Ensure interrupts are enabled
	#define IRQ_ASSERT_ENABLED()  ASSERT(IRQ_ENABLED())

	/// Ensure interrupts are not enabled
	#define IRQ_ASSERT_DISABLED() ASSERT(!IRQ_ENABLED())
#else
	#define IRQ_ASSERT_ENABLED() do {} while(0)
	#define IRQ_ASSERT_DISABLED() do {} while(0)
#endif


#ifndef IRQ_PREEMPT_HANDLER
	#if CONFIG_KERN_PREEMPT
		/**
		 * Handle preemptive context switch inside timer IRQ.
		 */
		INLINE void IRQ_PREEMPT_HANDLER(void)
		{
			if (proc_needPreempt())
				proc_preempt();
		}
	#else
		#define IRQ_PREEMPT_HANDLER() /* Nothing */
	#endif
#endif

/**
 * Execute \a CODE atomically with respect to interrupts.
 *
 * \see IRQ_SAVE_DISABLE IRQ_RESTORE
 */
#define ATOMIC(CODE) \
	do { \
		cpu_flags_t __flags; \
		IRQ_SAVE_DISABLE(__flags); \
		CODE; \
		IRQ_RESTORE(__flags); \
	} while (0)

#endif /* CPU_IRQ_H */

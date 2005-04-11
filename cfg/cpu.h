/*!
 * \file
 * <!--
 * Copyright 2004, 2005 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2004 Giovanni Bajo
 * This file is part of DevLib - See README.devlib for information.
 * -->
 *
 * \brief CPU-specific definitions
 *
 * \version $Id$
 *
 * \author Giovanni Bajo <rasky@develer.com>
 * \author Bernardo Innocenti <bernie@develer.com>
 * \author Stefano Fedrigo <aleph@develer.com>
 */

/*#*
 *#* $Log$
 *#* Revision 1.2  2005/04/11 19:10:27  bernie
 *#* Include top-level headers from cfg/ subdir.
 *#*
 *#* Revision 1.1  2005/04/11 19:04:13  bernie
 *#* Move top-level headers to cfg/ subdir.
 *#*
 *#* Revision 1.30  2005/03/15 00:20:09  bernie
 *#* BREAKPOINT, IRQ_RUNNING(), IRQ_GETSTATE(): New DSP56K macros.
 *#*
 *#* Revision 1.29  2005/02/16 20:33:24  bernie
 *#* Preliminary PPC support.
 *#*
 *#* Revision 1.28  2004/12/31 17:39:41  bernie
 *#* Fix documentation.
 *#*
 *#* Revision 1.27  2004/12/31 17:02:47  bernie
 *#* IRQ_SAVE_DISABLE(), IRQ_RESTORE(): Add null stubs for x86.
 *#*
 *#* Revision 1.26  2004/12/13 12:08:12  bernie
 *#* DISABLE_IRQSAVE, ENABLE_IRQRESTORE, DISABLE_INTS, ENABLE_INTS: Remove obsolete macros.
 *#*
 *#* Revision 1.25  2004/12/08 08:31:02  bernie
 *#* CPU_HARVARD: Define to 1 for AVR and DSP56K.
 *#*/
#ifndef DEVLIB_CPU_H
#define DEVLIB_CPU_H

#include <cfg/compiler.h> /* for uintXX_t */


/*!
 * \name Macros for determining CPU endianness.
 * \{
 */
#define CPU_BIG_ENDIAN    0x1234
#define CPU_LITTLE_ENDIAN 0x3412
/*\}*/

/*! Macro to include cpu-specific versions of the headers. */
#define CPU_HEADER(module)          PP_STRINGIZE(PP_CAT3(module, _, CPU_ID).h)


#if CPU_I196

	#define NOP                     nop_instruction()
	#define IRQ_DISABLE             disable_interrupt()
	#define IRQ_ENABLE              enable_interrupt()

	typedef uint16_t cpuflags_t; // FIXME
	typedef unsigned int cpustack_t;

	#define CPU_REG_BITS            16
	#define CPU_REGS_CNT            16
	#define CPU_STACK_GROWS_UPWARD  0
	#define CPU_SP_ON_EMPTY_SLOT	0
	#define CPU_BYTE_ORDER          CPU_LITTLE_ENDIAN
	#define CPU_HARVARD		0

#elif CPU_X86

	#define NOP                     asm volatile ("nop")
	#define IRQ_DISABLE             /* nothing */
	#define IRQ_ENABLE              /* nothing */
	#define IRQ_SAVE_DISABLE(x)     /* nothing */
	#define IRQ_RESTORE(x)          /* nothing */

	typedef uint32_t cpuflags_t; // FIXME
	typedef uint32_t cpustack_t;

	#define CPU_REG_BITS            32
	#define CPU_REGS_CNT            7
	#define CPU_STACK_GROWS_UPWARD  0
	#define CPU_SP_ON_EMPTY_SLOT	0
	#define CPU_BYTE_ORDER          CPU_LITTLE_ENDIAN
	#define CPU_HARVARD		0

#elif CPU_PPC
	#define NOP                 asm volatile ("nop" ::)
	#define IRQ_DISABLE         FIXME
	#define IRQ_ENABLE          FIXME
	#define IRQ_SAVE_DISABLE(x) FIXME
	#define IRQ_RESTORE(x)      FIXME
	#define IRQ_GETSTATE()      FIXME

	typedef uint32_t cpuflags_t; // FIXME
	typedef uint32_t cpustack_t; // FIXME

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

	static inline bool irq_getstate(void)
	{
		uint16_t x;
		asm(move SR,x);
		return !(x & 0x0200);
	}
	#define IRQ_GETSTATE() irq_getstate()



	typedef uint16_t cpuflags_t;
	typedef unsigned int cpustack_t;

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

	#define IRQ_GETSTATE() \
	({ \
		uint8_t sreg; \
		__asm__ __volatile__( \
			"in %0,__SREG__\n\t" \
			: "=r" (sreg)  /* no inputs & no clobbers */ \
		); \
		(bool)(sreg & 0x80); \
	})

	typedef uint8_t cpuflags_t;
	typedef uint8_t cpustack_t;

	/* Register counts include SREG too */
	#define CPU_REG_BITS            8
	#define CPU_REGS_CNT           33
	#define CPU_SAVED_REGS_CNT     19
	#define CPU_STACK_GROWS_UPWARD  0
	#define CPU_SP_ON_EMPTY_SLOT    1
	#define CPU_BYTE_ORDER          CPU_LITTLE_ENDIAN
	#define CPU_HARVARD             1

	/*!
	 * Initialization value for registers in stack frame.
	 * The register index is not directly corrispondent to CPU
	 * register numbers. Index 0 is the SREG register: the initial
	 * value is all 0 but the interrupt bit (bit 7).
	 */
	#define CPU_REG_INIT_VALUE(reg) (reg == 0 ? 0x80 : 0)

#endif

/*!
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


//! Default for macro not defined in the right arch section
#ifndef CPU_REG_INIT_VALUE
	#define CPU_REG_INIT_VALUE(reg)     0
#endif


#ifndef CPU_STACK_GROWS_UPWARD
	#error CPU_STACK_GROWS_UPWARD should have been defined to either 0 or 1
#endif

#ifndef CPU_SP_ON_EMPTY_SLOT
	#error CPU_SP_ON_EMPTY_SLOT should have been defined to either 0 or 1
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

#else
	#define CPU_PUSH_CALL_CONTEXT(sp, func) \
		CPU_PUSH_WORD((sp), (func))
#endif


/*!
 * \name Default type sizes.
 *
 * These defaults are reasonable for most 16/32bit machines.
 * Some of these macros may be overridden by CPU-specific code above.
 *
 * ANSI C requires that the following equations be true:
 * \code
 *   sizeof(char) <= sizeof(short) <= sizeof(int) <= sizeof(long)
 *   sizeof(float) <= sizeof(double)
 *   CPU_BITS_PER_CHAR  >= 8
 *   CPU_BITS_PER_SHORT >= 8
 *   CPU_BITS_PER_INT   >= 16
 *   CPU_BITS_PER_LONG  >= 32
 * \endcode
 * \{
 */
#ifndef SIZEOF_CHAR
#define SIZEOF_CHAR  1
#endif

#ifndef SIZEOF_SHORT
#define SIZEOF_SHORT  2
#endif

#ifndef SIZEOF_INT
#if CPU_REG_BITS < 32
	#define SIZEOF_INT  2
#else
	#define SIZEOF_INT  4
#endif
#endif /* !SIZEOF_INT */

#ifndef SIZEOF_LONG
#if CPU_REG_BITS > 32
	#define SIZEOF_LONG  8
#else
	#define SIZEOF_LONG  4
#endif
#endif

#ifndef SIZEOF_PTR
#define SIZEOF_PTR   SIZEOF_INT
#endif

#ifndef CPU_BITS_PER_CHAR
#define CPU_BITS_PER_CHAR   (SIZEOF_CHAR * 8)
#endif

#ifndef CPU_BITS_PER_SHORT
#define CPU_BITS_PER_SHORT  (SIZEOF_SHORT * CPU_BITS_PER_CHAR)
#endif

#ifndef CPU_BITS_PER_INT
#define CPU_BITS_PER_INT    (SIZEOF_INT * CPU_BITS_PER_CHAR)
#endif

#ifndef CPU_BITS_PER_LONG
#define CPU_BITS_PER_LONG   (SIZEOF_LONG * CPU_BITS_PER_CHAR)
#endif

#ifndef CPU_BITS_PER_PTR
#define CPU_BITS_PER_PTR    (SIZEOF_PTR * CPU_BITS_PER_CHAR)
#endif

#ifndef BREAKPOINT
#define BREAKPOINT /* nop */
#endif

/*\}*/

/* Sanity checks for the above definitions */
STATIC_ASSERT(sizeof(char) == SIZEOF_CHAR);
STATIC_ASSERT(sizeof(short) == SIZEOF_SHORT);
STATIC_ASSERT(sizeof(long) == SIZEOF_LONG);
STATIC_ASSERT(sizeof(int) == SIZEOF_INT);


/*!
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
		void SchedulerIdle(void);
		EXTERN_C_END
		#define CPU_IDLE SchedulerIdle()
	#else /* !ARCH_EMUL */
		#define CPU_IDLE do { /* nothing */ } while (0)
	#endif /* !ARCH_EMUL */
#endif /* !CPU_IDLE */

/* OBSOLETE */
#define SCHEDULER_IDLE CPU_IDLE

#endif /* DEVLIB_CPU_H */

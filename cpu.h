/*!
 * \file
 * <!--
 * Copyright 2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2004 Giovanni Bajo
 * This file is part of DevLib - See devlib/README for information.
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

/*
 * $Log$
 * Revision 1.5  2004/07/20 16:20:35  bernie
 * Move byte-order macros to mware/byteorder.h; Add missing author names.
 *
 * Revision 1.4  2004/07/20 16:06:04  bernie
 * Add macros to handle endianess issues.
 *
 * Revision 1.3  2004/07/18 21:49:51  bernie
 * Fixes for GCC 3.5.
 *
 * Revision 1.2  2004/06/03 11:27:09  bernie
 * Add dual-license information.
 *
 * Revision 1.1  2004/05/23 17:48:35  bernie
 * Add top-level files.
 *
 */
#ifndef CPU_H
#define CPU_H

#include "compiler.h"

//! Initialization value for registers in stack frame
#define CPU_REG_INIT_VALUE(reg)     0

// Macros for determining CPU endianness
#define CPU_BIG_ENDIAN    0x1234
#define CPU_LITTLE_ENDIAN 0x3412


#if defined(__IAR_SYSTEMS_ICC) || defined(__IAR_SYSTEMS_ICC__)  /* 80C196 */

	#define DISABLE_INTS            disable_interrupt()
	#define ENABLE_INTS             enable_interrupt()
	#define NOP                     nop_instruction()
	#define SCHEDULER_IDLE          /* Hmmm... could we go in STOP mode? */

	typedef uint16_t cpuflags_t; // FIXME
	typedef unsigned int cpustack_t;

	#define CPU_REGS_CNT            16
	#define CPU_STACK_GROWS_UPWARD  0
	#define CPU_SP_ON_EMPTY_SLOT	0
	#define CPU_BYTE_ORDER          CPU_LITTLE_ENDIAN

#elif defined(__i386__) || defined(_MSC_VER) /* x86 */

	#define NOP                     asm volatile ("nop")
	#define DISABLE_INTS            /* nothing */
	#define ENABLE_INTS             /* nothing */
	#define SCHEDULER_IDLE          SchedulerIdle()

	typedef uint32_t cpuflags_t; // FIXME
	typedef uint32_t cpustack_t;

	#define CPU_REGS_CNT            7
	#define CPU_STACK_GROWS_UPWARD  0
	#define CPU_SP_ON_EMPTY_SLOT	0
	#define CPU_BYTE_ORDER          CPU_LITTLE_ENDIAN

#elif defined(__m56800E__) || defined(__m56800__) /* DSP56K */

	#define NOP                     asm(nop)
	#define DISABLE_INTS            do { asm(bfset #0x0200,SR); asm(nop); } while (0)
	#define ENABLE_INTS             do { asm(bfclr #0x0200,SR); asm(nop); } while (0)
	#define SCHEDULER_IDLE          /* nothing */

	#define DISABLE_IRQSAVE(x)  \
		do { asm(move SR,x); asm(bfset #0x0200,SR); } while (0)
	#define ENABLE_IRQRESTORE(x)  \
		do { asm(move x,SR); } while (0)

	typedef uint16_t cpuflags_t;
	typedef unsigned int cpustack_t;

	#define CPU_REGS_CNT            FIXME
	#define CPU_SAVED_REGS_CNT      28
	#define CPU_STACK_GROWS_UPWARD  1
	#define CPU_SP_ON_EMPTY_SLOT	0
	#define CPU_BYTE_ORDER          CPU_BIG_ENDIAN

	#undef CPU_REG_INIT_VALUE
	INLINE uint16_t CPU_REG_INIT_VALUE(int reg)
	{
		if (reg == 14)
		{
			uint16_t omr_img;
			asm(move OMR, omr_img);
			return omr_img & (BV(3)/*EX*/ | BV(1)/*MB*/ | BV(0)/*MA*/);
		}
		else if (reg == 16)/*M01*/
			return 0xFFFF;
		return 0;
	}

#elif defined (__AVR__)

	#define NOP                     asm volatile ("nop" ::)
	#define DISABLE_INTS            asm volatile ("cli" ::)
	#define ENABLE_INTS             asm volatile ("sei" ::)
	#define SCHEDULER_IDLE          /* nothing */

	#define DISABLE_IRQSAVE(x) \
	do { \
		__asm__ __volatile__( \
			"in %0,__SREG__\n\t" \
			"cli" \
			: "=r" (x) : /* no inputs */ : "cc" \
		); \
	} while (0)

	#define ENABLE_IRQRESTORE(x) \
	do { \
		__asm__ __volatile__( \
			"out __SREG__,%0" : /* no outputs */ : "r" (x) : "cc" \
		); \
	} while (0)

	typedef uint8_t cpuflags_t;
	typedef uint8_t cpustack_t;

	#define CPU_REGS_CNT            32
	#define CPU_SAVED_REGS_CNT      18
	#define CPU_STACK_GROWS_UPWARD  0
	#define CPU_SP_ON_EMPTY_SLOT	1
	#define CPU_BYTE_ORDER          CPU_LITTLE_ENDIAN
#else
	#error Unknown CPU
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


#if defined(__m56800E__) || defined(__m56800__)
	/* DSP56k pushes both PC and SR to the stack in the JSR instruction, but
	 * RTS discards SR while returning (it does not restore it). So we push
	 * 0 to fake the same context.
	 */
	#define CPU_PUSH_CALL_CONTEXT(sp, func) \
		do { \
			CPU_PUSH_WORD((sp), (func)); \
			CPU_PUSH_WORD((sp), 0); \
		} while (0);

#elif defined (__AVR__)
	/* In AVR, the addresses are pushed into the stack as little-endian, while
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

#endif /* CPU_H */

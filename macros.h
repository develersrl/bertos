/*!
 * \file
 * <!--
 * Copyright 2003, 2004 Develer S.r.l. (http://www.develer.com/)
 * This file is part of DevLib - See devlib/README for information.
 * -->
 *
 * \version $Id$
 *
 * \author Bernardo Innocenti <bernie@develer.com>
 * \author Giovanni Bajo <rasky@develer.com>
 *
 * \brief Common and handy function macros
 */

/*
 * $Log$
 * Revision 1.1  2004/08/14 19:37:57  rasky
 * Merge da SC: macros.h, pool.h, BIT_CHANGE, nome dei processi, etc.
 *
 * Revision 1.4  2004/08/14 18:36:50  rasky
 * Doxygen fix e un livello di parentesi aggiuntivi per la macro
 *
 * Revision 1.3  2004/08/12 20:01:32  rasky
 * Aggiunte macro BIT_CHANGE e BIT_CHANGE_BV
 *
 * Revision 1.2  2004/08/10 21:36:14  rasky
 * Aggiunto include macros.h dove serve
 * Aggiunta dipendenza da compiler.h in macros.h
 *
 * Revision 1.1  2004/08/10 21:30:00  rasky
 * Estratte le funzioni macro in macros.h
 *
 */

#ifndef MACROS_H
#define MACROS_H

#include <compiler.h>

/* Quasi-ANSI macros */
#ifndef offsetof
	/*! offsetof(s,m) - Return the byte offset of the member \a m in struct \a s */
	#define offsetof(s,m)   (size_t)&(((s *)0)->m)
#endif
#ifndef countof
	/*! Count the number of elements in the static array \a a */
	#define countof(a)      (sizeof(a) / sizeof(*(a)))
#endif


/* Simple macros */
#define ABS(a)      (((a) < 0) ? -(a) : (a))
#define MIN(a,b)    (((a) < (b)) ? (a) : (b))
#define MAX(a,b)    (((a) > (b)) ? (a) : (b))

#ifndef BV
/*! Convert a bit value to a binary flag */
#define BV(x)       (1<<(x))
#endif

/*! Round up \a x to an even multiple of the 2's power \a pad */
#define ROUND2(x, pad) (((x) + ((pad) - 1)) & ~((pad) - 1))

//! Check if \a x is an integer power of 2
#define IS_POW2(x)     (!(bool)((x) & ((x)-1)))

/*! Calculate a compile-time log2 for a uint8_t */
#define UINT8_LOG2(x) \
	((x) < 2 ? 0 : \
	 ((x) < 4 ? 1 : \
	  ((x) < 8 ? 2 : \
	   ((x) < 16 ? 3 : \
	    ((x) < 32 ? 4 : \
	     ((x) < 64 ? 5 : \
	      ((x) < 128 ? 6 : 7)))))))

/*! Calculate a compile-time log2 for a uint16_t */
#define UINT16_LOG2(x) \
	((x < 256) ? UINT8_LOG2(x) : UINT8_LOG2((x) >> 8) + 8)

/*! Calculate a compile-time log2 for a uint32_t */
#define UINT32_LOG2(x) \
	((x < 65536UL) ? UINT16_LOG2(x) : UINT16_LOG2((x) >> 16) + 16)

/*! Concatenate two different preprocessor tokens (allowing macros to expand) */
#define PP_CAT(x,y)         PP_CAT__(x,y)
#define PP_CAT__(x,y)       x ## y
#define PP_CAT3(x,y,z)      PP_CAT(PP_CAT(x,y),z)
#define PP_CAT4(x,y,z,w)    PP_CAT(PP_CAT3(x,y,z),w)
#define PP_CAT5(x,y,z,w,j)  PP_CAT(PP_CAT4(x,y,z,w),j)

/*! String-ize a token (allowing macros to expand) */
#define PP_STRINGIZE(x)     PP_STRINGIZE__(x)
#define PP_STRINGIZE__(x)   #x

#if COMPILER_C99
	/*! Count the number of arguments (up to 16) */
	#define PP_COUNT(...) \
		PP_COUNT__(__VA_ARGS__,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0)
	#define PP_COUNT__(a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15,count,...) \
		count
#endif

/*! Issue a compilation error if the \a condition is false */
#define STATIC_ASSERT(condition)  \
	extern char CT_ASSERT___[(condition) ? 1 : -1]


#if COMPILER_C99
	/*!
	 * \def BIT_CHANGE(reg, (mask, value), ...)
	 *
	 * This macro allows for efficient and compact bit toggling in a hardware
	 * register. It is meant to replace hand-coded cruft which toggles bits
	 * in sequence.
	 *
	 * It is possible to specify an unlimited pair of (mask, value) parameters.
	 * For instance:
	 *
	 * \code
	 * void set_timer(bool start)
	 * {
	 *     BIT_CHANGE(REG_CTRL_TIMER,
	 *        (TIMER_MODE, MODE_COUNT),
	 *        (OVL_IRQ, 1),
	 *        (CMP_IRQ, 1),
	 *        (START, start)
	 *     );
	 * }
	 * \endcode
	 *
	 * The macro expansion will be roughly the following:
	 *
	 * \code
	 * REG_CTRL_TIMER = (REG_CTRL_TIMER & ~(TIMER_MODE|OVL_IRQ|CMP_IRQ|START)
	 *                  | (MODE_COUNT|OVL_IRQ|CMP_IRQ|(start ? START : 0));
	 * \endcode
	 *
	 * It is up to the compiler to produce the optimal code. We checked that GCC produces
	 * the best code in most cases. We preferred this expansion over the use of a block
	 * with a local variable because CodeWarrior 6.1 was not able to remove completely the
	 * allocation of the local from the stack.
	 *
	 * \note This macro is available only in C99 because it makes use of variadic macros.
	 * It would be possible to make up an implementation with a slightly different syntax
	 * for use with C90 compilers, through Boost Preprocessor.
	 *
	 */

	/*!
	 * \def BIT_CHANGE_BV(reg, (bit, value), ...)
	 *
	 * Similar to BIT_CHANGE(), but get bits instead of masks (and applies BV() to convert
	 * them to masks).
	 *
	 */

	#define BIT_EXTRACT_FLAG_0(bit, value)  bit
	#define BIT_EXTRACT_FLAG_1(bit, value)  BV(bit)
	#define BIT_EXTRACT_VALUE__(bit, value) value

	#define BIT_MASK_SINGLE__(use_bv, index, max, arg) \
		((index < max) ? (PP_CAT(BIT_EXTRACT_FLAG_, use_bv) arg) : 0) \
		/**/

	#define BIT_MASK_IF_SINGLE__(use_bv, index, max, arg) \
		(((index < max) && (BIT_EXTRACT_VALUE__ arg)) ? (PP_CAT(BIT_EXTRACT_FLAG_, use_bv) arg) : 0) \
		/**/

	#define BIT_ITER__2(macro, use_bv, max, a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15, ...) \
		(macro(use_bv, 0, max, a0) | \
		macro(use_bv, 1, max, a1) | \
		macro(use_bv, 2, max, a2) | \
		macro(use_bv, 3, max, a3) | \
		macro(use_bv, 4, max, a4) | \
		macro(use_bv, 5, max, a5) | \
		macro(use_bv, 6, max, a6) | \
		macro(use_bv, 7, max, a7) | \
		macro(use_bv, 8, max, a8) | \
		macro(use_bv, 9, max, a9) | \
		macro(use_bv, 10, max, a10) | \
		macro(use_bv, 11, max, a11) | \
		macro(use_bv, 12, max, a12) | \
		macro(use_bv, 13, max, a13) | \
		macro(use_bv, 14, max, a14) | \
		macro(use_bv, 15, max, a15)) \
		/**/

	#define BIT_ITER__(macro, use_bv, ...) \
		BIT_ITER__2(macro, use_bv, PP_COUNT(__VA_ARGS__), __VA_ARGS__, (0,1),(0,1),(0,1),(0,1),(0,1),(0,1),(0,1),(0,1),(0,1),(0,1),(0,1),(0,1),(0,1),(0,1),(0,1),(0,1)) \
		/**/

	#define BIT_MASKS__(use_bv, ...) \
		BIT_ITER__(BIT_MASK_SINGLE__, use_bv, __VA_ARGS__)
		/**/

	#define BIT_MASKS_CONDITIONAL__(use_bv, ...) \
		BIT_ITER__(BIT_MASK_IF_SINGLE__, use_bv, __VA_ARGS__)
		/**/

	#define BIT_CHANGE__(reg, use_bv, ...) \
		((reg) = ((reg) & ~BIT_MASKS__(use_bv, __VA_ARGS__)) | BIT_MASKS_CONDITIONAL__(use_bv, __VA_ARGS__)) \
		/**/

	#define BIT_CHANGE(reg, ...)        BIT_CHANGE__(reg, 0, __VA_ARGS__)
	#define BIT_CHANGE_BV(reg, ...)     BIT_CHANGE__(reg, 1, __VA_ARGS__)

#endif /* COMPILER_C99 */

#endif /* MACROS_H */


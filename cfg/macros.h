/*!
 * \file
 * <!--
 * Copyright 2003, 2004 Develer S.r.l. (http://www.develer.com/)
 * This file is part of DevLib - See devlib/README for information.
 * -->
 *
 * \brief Common and handy function macros
 *
 * \version $Id$
 * \author Bernardo Innocenti <bernie@develer.com>
 * \author Giovanni Bajo <rasky@develer.com>
 */

/*#*
 *#* $Log$
 *#* Revision 1.1  2005/04/11 19:04:13  bernie
 *#* Move top-level headers to cfg/ subdir.
 *#*
 *#* Revision 1.10  2005/01/22 04:19:50  bernie
 *#* Use non-uglified typeof().
 *#*
 *#* Revision 1.9  2004/12/08 08:51:34  bernie
 *#* Add type-generic macros for C++.
 *#*
 *#* Revision 1.8  2004/10/19 07:14:20  bernie
 *#* Add macros to test for specific compiler features.
 *#*
 *#* Revision 1.7  2004/09/20 03:30:45  bernie
 *#* C++ also has variadic macros.
 *#*
 *#* Revision 1.6  2004/09/14 21:02:04  bernie
 *#* SWAP(), MINMAX(): New macros.
 *#*
 *#* Revision 1.5  2004/08/29 21:57:58  bernie
 *#* Move back STATIC_ASSERT() to compiler.h as it's needed in cpu.h;
 *#* iptr_t, const_iptr_t: Replace IPTR macro with a real typedef.
 *#*
 *#* Revision 1.3  2004/08/24 14:13:48  bernie
 *#* Restore a few macros that were lost in the way.
 *#*
 *#* Revision 1.2  2004/08/24 13:32:14  bernie
 *#* PP_CAT(), PP_STRINGIZE(): Move back to compiler.h to break circular dependency between cpu.h/compiler.h/macros.h;
 *#* offsetof(), countof(): Move back to compiler.h to avoid including macros.h almost everywhere;
 *#* Trim CVS log;
 *#* Rename header guards;
 *#* Don't include arch_config.h in compiler.h as it's not needed there.
 *#*
 *#* Revision 1.1  2004/08/14 19:37:57  rasky
 *#* Merge da SC: macros.h, pool.h, BIT_CHANGE, nome dei processi, etc.
 *#*
 *#* Revision 1.4  2004/08/14 18:36:50  rasky
 *#* Doxygen fix e un livello di parentesi aggiuntivi per la macro
 *#*
 *#* Revision 1.3  2004/08/12 20:01:32  rasky
 *#* Aggiunte macro BIT_CHANGE e BIT_CHANGE_BV
 *#*
 *#* Revision 1.2  2004/08/10 21:36:14  rasky
 *#* Aggiunto include macros.h dove serve
 *#* Aggiunta dipendenza da compiler.h in macros.h
 *#*
 *#* Revision 1.1  2004/08/10 21:30:00  rasky
 *#* Estratte le funzioni macro in macros.h
 *#*
 *#*/

#ifndef MACROS_H
#define MACROS_H

#include <compiler.h>

/* avr-gcc does not seem to support libstdc++ */
#if defined(__cplusplus) && !CPU_AVR
	/* Type-generic macros implemented with template functions. */
	#include <algorithm>

	template<class T> inline T ABS(T n) { return n >= 0 ? n : -n; }
	#define MIN(a,b)   std::min(a, b)
	#define MAX(a,b)   std::max(a, b)
	#define SWAP(a,b)  std::swap(a, b)
#elif (COMPILER_STATEMENT_EXPRESSIONS && COMPILER_TYPEOF)
	/* Type-generic macros implemented with statement expressions. */
	#define ABS(n) ({ \
		typeof(n) _n = (n); \
		(_n < 0) ? -_n : _n; \
	})
	#define MIN(a,b) ({ \
		typeof(a) _a = (a); \
		typeof(b) _b = (b); \
		(void)(&_a == &_b); /* ensure same type */ \
		(_a < _b) ? _a : _b; \
	})
	#define MAX(a,b) ({ \
		typeof(a) _a = (a); \
		typeof(b) _b = (b); \
		(void)(&_a == &_b); /* ensure same type */ \
		(_a > _b) ? _a : _b; \
	})
#else /* !(COMPILER_STATEMENT_EXPRESSIONS && COMPILER_TYPEOF) */
	/* Buggy macros for inferior compilers.  */
	#define ABS(a)		(((a) < 0) ? -(a) : (a))
	#define MIN(a,b)	(((a) < (b)) ? (a) : (b))
	#define MAX(a,b)	(((a) > (b)) ? (a) : (b))
#endif /* !(COMPILER_STATEMENT_EXPRESSIONS && COMPILER_TYPEOF) */

/*! Bound \a x between \a min and \a max. */
#define MINMAX(min,x,max)  (MIN(MAX(min, x), max))

#ifdef __cplusplus
	/* Use standard implementation from <algorithm> */
	#define SWAP(a,b)  std::swap(a, b)
#elif COMPILER_TYPEOF
	/*!
	 * Type-generic macro to swap \a a with \a b.
	 *
	 * \note Arguments are evaluated multiple times.
	 */
	#define SWAP(a, b) \
		do { \
			(void)(&(a) == &(b)); /* type check */ \
			typeof(a) tmp; \
			tmp = (a); \
			(a) = (b); \
			(b) = tmp; \
		} while (0)
#else /* !COMPILER_TYPEOF */
	/* Sub-optimal implementation that only works with integral types. */
	#define SWAP(a, b) ((a) ^= (b) ^= (a) ^= (b))
#endif /* COMPILER_TYPEOF */

#ifndef BV
	/*! Convert a bit value to a binary flag. */
	#define BV(x)  (1<<(x))
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

#if COMPILER_VARIADIC_MACROS
	/*! Count the number of arguments (up to 16). */
	#define PP_COUNT(...) \
		PP_COUNT__(__VA_ARGS__,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0)
	#define PP_COUNT__(a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15,count,...) \
		count
#endif

#if COMPILER_VARIADIC_MACROS
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
	 */

	/*!
	 * \def BIT_CHANGE_BV(reg, (bit, value), ...)
	 *
	 * Similar to BIT_CHANGE(), but get bits instead of masks (and applies BV() to convert
	 * them to masks).
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

#endif /* COMPILER_VARIADIC_MACROS */

#endif /* MACROS_H */


/*!
 * \file
 * <!--
 * Copyright 2003, 2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2001, 2002, 2003 Bernardo Innocenti <bernie@codewiz.org>
 * This file is part of DevLib - See devlib/README for information.
 * -->
 *
 * \brief Additional support macros for compiler independance
 *
 * \version $Id$
 * \author Bernardo Innocenti <bernie@develer.com>
 */

/*#*
 *#* $Log$
 *#* Revision 1.26  2004/10/19 08:55:14  bernie
 *#* UNUSED_FUNC: New function attribute.
 *#*
 *#* Revision 1.25  2004/10/19 07:14:20  bernie
 *#* Add macros to test for specific compiler features.
 *#*
 *#* Revision 1.24  2004/10/03 18:35:13  bernie
 *#* Poison C++ keywords in C programs for better portability.
 *#*
 *#* Revision 1.23  2004/09/20 03:30:27  bernie
 *#* Remove vsprintf_P() proto, no longer needed with avr-libc 1.0.4.
 *#*
 *#* Revision 1.22  2004/09/14 21:03:04  bernie
 *#* PURE_FUNC, CONST_FUNC, MUST_CHECK: New function attributes; LIKELY()/UNLIKELY(): Fix for non-integral expressions.
 *#*
 *#* Revision 1.21  2004/09/06 21:38:31  bernie
 *#* Misc documentation and style fixes.
 *#*
 *#* Revision 1.20  2004/08/29 21:57:58  bernie
 *#* Move back STATIC_ASSERT() to compiler.h as it's needed in cpu.h;
 *#* iptr_t, const_iptr_t: Replace IPTR macro with a real typedef.
 *#*
 *#* Revision 1.19  2004/08/25 14:12:08  rasky
 *#* Aggiornato il comment block dei log RCS
 *#*
 *#* Revision 1.18  2004/08/24 16:32:37  bernie
 *#* Document custom types.
 *#*
 *#* Revision 1.17  2004/08/24 13:32:14  bernie
 *#* PP_CAT(), PP_STRINGIZE(): Move back to compiler.h to break circular dependency between cpu.h/compiler.h/macros.h;
 *#* offsetof(), countof(): Move back to compiler.h to avoid including macros.h almost everywhere;
 *#* Trim CVS log;
 *#* Rename header guards;
 *#* Don't include arch_config.h in compiler.h as it's not needed there.
 *#*
 *#* Revision 1.16  2004/08/14 19:37:57  rasky
 *#* Merge da SC: macros.h, pool.h, BIT_CHANGE, nome dei processi, etc.
 *#*
 *#* Revision 1.15  2004/08/13 03:23:26  bernie
 *#* Adjust a few MSVC tweaks from older projects.
 *#*
 *#* Revision 1.14  2004/08/10 06:56:29  bernie
 *#* RESTRICT: New C99-like macro; STATIC_ASSERT: Fix warning for multiple invocation in one file.
 *#*
 *#* Revision 1.13  2004/08/02 20:20:29  aleph
 *#* Merge from project_ks
 *#*
 *#* Revision 1.12  2004/08/01 01:21:17  bernie
 *#* LIKELY(), UNLIKELY(): New compiler-specific macros.
 *#*
 *#* Revision 1.11  2004/07/30 14:34:10  rasky
 *#* Vari fix per documentazione e commenti
 *#* Aggiunte PP_CATn e STATIC_ASSERT
 *#*
 *#* Revision 1.10  2004/07/30 14:15:53  rasky
 *#* Nuovo supporto unificato per detect della CPU
 *#*
 *#* Revision 1.9  2004/07/29 22:57:09  bernie
 *#* vsprintf(): Remove prototype for backwards compatibility with GCC 3.4; ssize_t: Add definition for inferior compilers.
 *#*/
#ifndef DEVLIB_COMPILER_H
#define DEVLIB_COMPILER_H

#include "cpu_detect.h"


#if defined __GNUC__ && defined __GNUC_MINOR__
	#define GNUC_PREREQ(maj, min) \
		((__GNUC__ << 16) + __GNUC_MINOR__ >= ((maj) << 16) + (min))
#else
	#define GNUC_PREREQ(maj, min) 0
#endif

#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
	#define COMPILER_C99      1
#else
	#define COMPILER_C99      0
#endif


/*! Concatenate two different preprocessor tokens (allowing macros to expand) */
#define PP_CAT(x,y)         PP_CAT__(x,y)
#define PP_CAT__(x,y)       x ## y
#define PP_CAT3(x,y,z)      PP_CAT(PP_CAT(x,y),z)
#define PP_CAT4(x,y,z,w)    PP_CAT(PP_CAT3(x,y,z),w)
#define PP_CAT5(x,y,z,w,j)  PP_CAT(PP_CAT4(x,y,z,w),j)

/*! String-ize a token (allowing macros to expand) */
#define PP_STRINGIZE(x)     PP_STRINGIZE__(x)
#define PP_STRINGIZE__(x)   #x


#if defined(__IAR_SYSTEMS_ICC) || defined(__IAR_SYSTEMS_ICC__)
	#pragma language=extended
	#define INTERRUPT(x)  interrupt [x]
	#define REGISTER      shortad
	#define INLINE        /* unsupported */

	/* Imported from <longjmp.h>. Unfortunately, we can't just include
	 * this header because it typedefs jmp_buf to be an array of chars.
	 * This would allow the compiler to place the buffer on an odd address.
	 * The CPU _should_ be able to perform word accesses to
	 * unaligned data, but there are *BUGS* in the 80196KC with
	 * some combinations of opcodes and addressing modes. One of
	 * these, "ST SP,[?GR]+" is used in the longjmp() implementation
	 * provided by the IAR compiler ANSI C library. When ?GR contains
	 * an odd address, surprisingly the CPU will copy the high order
	 * byte of the source operand (SP) in the low order byte of the
	 * destination operand (the memory location pointed to by ?GR).
	 *
	 * We also need to replace the library setjmp()/longjmp() with
	 * our own versions because the IAR implementation "forgets" to
	 * save the contents of local registers (?LR).
	 */
	struct _JMP_BUF
	{
		void *	sp;				/* Stack pointer */
		void *	return_addr;	/* Return address */
		int		lr[6];			/* 6 local registers */
	};

	typedef struct _JMP_BUF jmp_buf[1];

	int setjmp(jmp_buf env);
	void longjmp(jmp_buf env, int val);

	/* Fake bool support */
	#define true (1==1)
	#define false (1!=1)
	typedef unsigned char bool;

#elif defined(_MSC_VER) /* Win32 emulation support */

	#include <setjmp.h>
	#include <time.h> /* for time_t */

	/* FIXME: I can't remember why exactly this was needed (NdBernie) */
	#define float double

	/* Fake bool support */
	#ifndef __cplusplus
		#define true 1
		#define false 0
		typedef int bool;
	#endif /* !__cplusplus */

	/* These C99 functions are oddly named in MSVCRT32.lib */
	#define snprintf _snprintf
	#define vsnprintf _vsnprintf

#elif defined(__GNUC__)

	/* Compiler features */
	#define COMPILER_VARIADIC_MACROS 1 /* Even in C++ */
	#define COMPILER_TYPEOF 1
	#define COMPILER_STAEMENT_EXPRESSIONS 1

	/* GCC attributes */
	#define FORMAT(type,fmt,first)  __attribute__((__format__(type, fmt, first)))
	#define NORETURN                __attribute__((__noreturn__))
	#define UNUSED(type,arg)        __attribute__((__unused__)) type arg
	#define INLINE                  static inline __attribute__((__always_inline__))
	#define LIKELY(x)               __builtin_expect(!!(x), 1)
	#define UNLIKELY(x)             __builtin_expect(!!(x), 0)
	#define PURE_FUNC               __attribute__((pure))
	#define CONST_FUNC              __attribute__((const))
	#define UNUSED_FUNC             __attribute__((unused))
	#define RESTRICT                __restrict__
	#define MUST_CHECK              __attribute__((warn_unused_result))
	#if GNUC_PREREQ(3,1)
		#define DEPRECATED  __attribute__((__deprecated__))
	#endif

	#if CPU_X86

		/* Hack to avoid conflicts with system type */
		#define sigset_t system_sigset_t
		#include <stddef.h>
		#include <setjmp.h>
		#include <stdbool.h>
		#undef system_sigset_t

	#elif CPU_AVR

		#include <stddef.h>
		#include <stdbool.h>

		/* Support for harvard architectures */
		#ifdef _PROGMEM
			#define PGM_READ_CHAR(s) pgm_read_byte(s)
			#define PGM_FUNC(x) x ## _P
			#define PGM_ATTR  PROGMEM
		#endif

	#endif

	#ifndef __cplusplus
		/*
		 * Disallow some C++ keywords as identifiers in C programs,
		 * for improved portability.
		 */
		#pragma GCC poison new delete class template typename
		#pragma GCC poison private protected public operator
		#pragma GCC poison friend mutable using namespace
		#pragma GCC poison cin cout cerr clog
	#endif

#elif defined(__MWERKS__) && CPU_DSP56K

	/* Compiler features */
	#define COMPILER_TYPEOF 1
	#define COMPILER_STAEMENT_EXPRESSIONS 1

	#include <stdint.h>
	#include <stddef.h>
	#include <stdbool.h>
	#include <setjmp.h>

	// CodeWarrior has size_t as built-in type, but does not define this symbol.
	#define _SIZE_T_DEFINED

#else
	#error unknown compiler
#endif


/* Defaults for compiler extensions. */

/*!
 * \def COMPILER_VARIADIC_MACROS
 * Support for macros with variable arguments.
 */
#ifndef COMPILER_HAVE_VARIADIC_MACROS
#define COMPILER_HAVE_VARIADIC_MACROS (COMPILER_C99 != 0)
#endif

/*!
 * \def COMPILER_TYPEOF
 * Support for macros with variable arguments.
 */
#ifndef COMPILER_TYPEOF
#define COMPILER_TYPEOF 0
#endif

/*!
 * \def COMPILER_STATEMENT_EXPRESSIONS
 * Support for macros with variable arguments.
 */
#ifndef COMPILER_STATEMENT_EXPRESSIONS 0
#define COMPILER_STATEMENT_EXPRESSIONS 0
#endif

/* A few defaults for missing compiler features. */
#ifndef INLINE
#define INLINE                 static inline
#endif
#ifndef NORETURN
#define NORETURN               /* nothing */
#endif
#ifndef FORMAT
#define FORMAT(type,fmt,first) /* nothing */
#endif
#ifndef DEPRECATED
#define DEPRECATED             /* nothing */
#endif
#ifndef UNUSED
#define UNUSED(type,arg)       type arg
#endif
#ifndef REGISTER
#define REGISTER               /* nothing */
#endif
#ifndef INTERRUPT
#define INTERRUPT(x)           ERROR_NOT_IMPLEMENTED
#endif
#ifndef LIKELY
#define LIKELY(x)              x
#endif
#ifndef UNLIKELY
#define UNLIKELY(x)            x
#endif
#ifndef PURE_FUNC
#define PURE_FUNC              /* nothing */
#endif
#ifndef CONST_FUNC
#define CONST_FUNC             /* nothing */
#endif
#ifndef UNUSED_FUNC
#define UNUSED_FUNC            /* nothing */
#endif
#ifndef RESTRICT
#define RESTRICT               /* nothing */
#endif
#ifndef MUST_CHECK
#define MUST_CHECK             /* nothing */
#endif

/* Support for harvard architectures */
#ifndef PSTR
#define PSTR            /* nothing */
#endif
#ifndef PGM_READ_CHAR
#define PGM_READ_CHAR(s) (*(s))
#endif
#ifndef PGM_FUNC
#define PGM_FUNC(x) x
#endif
#ifndef PGM_ATTR
#define PGM_ATTR        /* nothing */
#endif


/* Misc definitions */
#ifndef NULL
#define NULL  (void *)0
#endif
#ifndef EOF
#define	EOF   (-1)
#endif


/* Support for hybrid C/C++ applications. */
#ifdef __cplusplus
	#define EXTERN_C        extern "C"
	#define EXTERN_C_BEGIN  extern "C" {
	#define EXTERN_C_END    }
#else
	#define EXTERN_C        extern
	#define EXTERN_C_BEGIN  /* nothing */
	#define EXTERN_C_END    /* nothing */
#endif


/*
 * Standard type definitions.
 * These should be in <sys/types.h>, but many compilers lack them.
 */
#if !(defined(size_t) || defined(_SIZE_T_DEFINED))
	typedef unsigned int size_t;
	typedef int ssize_t;
#endif
#if !(defined(_TIME_T_DEFINED) || defined(__time_t_defined))
	typedef long time_t;
#endif /* _TIME_T_DEFINED || __time_t_defined */

/*! Bulk storage large enough for both pointers or integers */
typedef void * iptr_t;
typedef const void * const_iptr_t;
#define IPTR iptr_t  /* OBSOLETE */

typedef long utime_t;            /*!< Type for time expressed in microseconds */
typedef unsigned char sig_t;     /*!< Type for signal bits */
typedef unsigned char sigset_t;  /*!< Type for signal masks */
typedef unsigned char page_t;    /*!< Type for banked memory pages */

#if (defined(_MSC_VER) || defined(__IAR_SYSTEMS_ICC) || defined(__IAR_SYSTEMS_ICC__))
	/*!
	 * \name ISO C99 fixed-size types
	 *
	 * These should be in <stdint.h>, but many compilers lack them.
	 * \{
	 */
	typedef signed char         int8_t;
	typedef short int           int16_t;
	typedef long int            int32_t;
	typedef unsigned char       uint8_t;
	typedef unsigned short int  uint16_t;
	typedef unsigned long int   uint32_t;
	/* \} */
#elif defined(__GNUC__) && CPU_AVR
	/* avr-libc is weird... */
	#include <inttypes.h>
#else
	/* This is the correct location. */
	#include <stdint.h>
#endif

/*!
 * \name Types for hardware registers.
 *
 * Only use these types for registers whose contents can
 * be changed asynchronously by external hardware.
 *
 * \{
 */
#if CPU_DSP56K
	/* Registers can be accessed only through 16-bit pointers */
	typedef volatile uint16_t  reg16_t;
#else
	typedef volatile uint8_t   reg8_t;
	typedef volatile uint16_t  reg16_t;
	typedef volatile uint32_t  reg32_t;
#endif
/*\}*/


/* Quasi-ANSI macros */
#ifndef offsetof
	/*!
	 * Return the byte offset of the member \a m in struct \a s.
	 *
	 * \note This macro should be defined in "stddef.h" and is sometimes
	 *       compiler-specific (g++ has a builtin for it).
	 */
	#define offsetof(s,m)  (size_t)&(((s *)0)->m)
#endif
#ifndef countof
	/*!
	 * Count the number of elements in the static array \a a.
	 *
	 * \note This macro is non-standard, but implements a very common idiom
	 */
	#define countof(a)  (sizeof(a) / sizeof(*(a)))
#endif

/*! Issue a compilation error if the \a condition is false */
#define STATIC_ASSERT(condition)  \
	extern char PP_CAT(CT_ASSERT___, __LINE__)[(condition) ? 1 : -1]

#endif /* DEVLIB_COMPILER_H */

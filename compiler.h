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
 *#* Revision 1.40  2005/01/20 18:46:04  aleph
 *#* Add progmem datatypes; PSTR() definition.
 *#*
 *#* Revision 1.39  2005/01/08 08:49:39  bernie
 *#* Define PROGMEM on AVR only when not including pgmspace.h.
 *#*
 *#* Revision 1.38  2004/12/31 16:41:52  bernie
 *#* PROGMEM: Define to nothing for non-Harvard processors.
 *#*
 *#* Revision 1.37  2004/12/08 09:43:21  bernie
 *#* Metrowerks supports variadic macros.
 *#*
 *#* Revision 1.36  2004/12/08 08:55:54  bernie
 *#* Rename sigset_t to sigmask_t and time_t to mtime_t, to avoid conflicts with POSIX definitions.
 *#*
 *#* Revision 1.35  2004/12/08 07:35:51  bernie
 *#* Typo in macro name.
 *#*
 *#* Revision 1.34  2004/11/28 23:21:33  bernie
 *#* Use mtime_t instead of overloading ANSI time_t with new semantics.
 *#*
 *#* Revision 1.33  2004/11/16 23:09:40  bernie
 *#* size_t: Add 64bit definitions; time_t: Add 16bit hack for tiny CPUs.
 *#*
 *#* Revision 1.32  2004/11/16 22:42:44  bernie
 *#* Doxygen fixes.
 *#*
 *#* Revision 1.31  2004/11/16 22:37:28  bernie
 *#* IPTR: Remove obsolete definition.
 *#*
 *#* Revision 1.30  2004/11/16 22:30:19  bernie
 *#* Declare fixed-size types before other types.
 *#*
 *#* Revision 1.29  2004/11/16 20:34:40  bernie
 *#* UNUSED_VAR, USED_VAR, USED_FUNC: New macros; UNUSED_ARG: Rename from UNUSED.
 *#*
 *#* Revision 1.28  2004/10/21 11:03:52  bernie
 *#* Typo.
 *#*
 *#* Revision 1.27  2004/10/21 10:09:40  bernie
 *#* Remove spurious token in preprocessor directive.
 *#*
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

/* Some CW versions do not allow enabling C99 from the settings panel. */
#if defined(__MWERKS__)
	#pragma c99 on
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

	/*
	 * Imported from <longjmp.h>. Unfortunately, we can't just include
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
		void *sp;           /* Stack pointer */
		void *return_addr;  /* Return address */
		int lr[6];          /* 6 local registers */
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

	/* FIXME: I can't remember why exactly this was needed (NdBernie) */
	#define float double

	/* MSVC doesn't provide <stdbool.h>. */
	#ifndef __cplusplus
		#define true (1==1)
		#define false (1!=1)
		typedef int bool;
	#endif /* !__cplusplus */

	/* These C99 functions are oddly named in MSVCRT32.lib */
	#define snprintf _snprintf
	#define vsnprintf _vsnprintf

#elif defined(__GNUC__)

	/* Compiler features */
	#define COMPILER_VARIADIC_MACROS 1 /* Even in C++ */
	#define COMPILER_TYPEOF 1
	#define COMPILER_STATEMENT_EXPRESSIONS 1

	/* GCC attributes */
	#define FORMAT(type,fmt,first)  __attribute__((__format__(type, fmt, first)))
	#define NORETURN                __attribute__((__noreturn__))
	#define UNUSED_ARG(type,arg)    __attribute__((__unused__)) type arg
	#define UNUSED_VAR(type,name)	__attribute__((__unused__)) type name
	#define USED_VAR(type,name)	__attribute__((__used__)) type name
	#define INLINE                  static inline __attribute__((__always_inline__))
	#define LIKELY(x)               __builtin_expect(!!(x), 1)
	#define UNLIKELY(x)             __builtin_expect(!!(x), 0)
	#define PURE_FUNC               __attribute__((pure))
	#define CONST_FUNC              __attribute__((const))
	#define UNUSED_FUNC             __attribute__((unused))
	#define USED_FUNC		__attribute__((__used__))
	#define RESTRICT                __restrict__
	#define MUST_CHECK              __attribute__((warn_unused_result))
	#if GNUC_PREREQ(3,1)
		#define DEPRECATED  __attribute__((__deprecated__))
	#endif

	#if CPU_X86

		#include <stddef.h>
		#include <setjmp.h>
		#include <stdbool.h>

	#elif CPU_AVR

		#include <stddef.h>
		#include <stdbool.h>

		/* Support for harvard architectures */
		#ifdef _PROGMEM
			#include <avr/pgmspace.h>
			#define PGM_READ_CHAR(s) pgm_read_byte(s)
			#define PGM_FUNC(x) x ## _P
			#define PGM_ATTR  PROGMEM
		#else
			/* We still need this for prototypes */
			#define PROGMEM  __attribute__((__progmem__))
			#define PSTR(s) ({static char __c[] PROGMEM = (s); __c;})
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
	#define COMPILER_VARIADIC_MACROS 1
	#define COMPILER_TYPEOF 1
	#define COMPILER_STATEMENT_EXPRESSIONS 1

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
#ifndef COMPILER_VARIADIC_MACROS
#define COMPILER_VARIADIC_MACROS (COMPILER_C99 != 0)
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
#ifndef COMPILER_STATEMENT_EXPRESSIONS
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
#ifndef UNUSED_ARG
#define UNUSED_ARG(type,arg)   type arg
#endif
#define UNUSED                 UNUSED_ARG /* OBSOLETE */
#ifndef UNUSED_VAR
#define UNUSED_VAR(type,name)  type name
#endif
#ifndef USED_VAR
#define USED_VAR(type,name)    type name
#endif
#ifndef REGISTER
#define REGISTER               /* nothing */
#endif
#ifndef PROGMEM
#define PROGMEM                /* nothing */
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
#ifndef USED_FUNC
#define USED_FUNC              /* nothing */
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
 * \name Types for variables stored in program memory (harvard processors).
 * \{
 */
typedef PROGMEM int8_t pgm_int8_t;
typedef PROGMEM uint8_t pgm_uint8_t;
typedef PROGMEM int16_t pgm_int16_t;
typedef PROGMEM uint16_t pgm_uint16_t;
typedef PROGMEM int32_t pgm_int32_t;
typedef PROGMEM uint32_t pgm_uint32_t;
/*\}*/

#if CPU_AVR_ATMEGA8
	/*
	 * The ATmega8 has a very small Flash, so we can't afford
	 * to link in support routines for 32bit integer arithmetic.
	 */
	typedef int16_t mtime_t;  /*!< Type for time expressed in milliseconds. */
	typedef int16_t utime_t;  /*!< Type for time expressed in microseconds. */
	#define SIZEOF_MTIME_T (16 / CPU_BITS_PER_CHAR)
	#define SIZEOF_UTIME_T (16 / CPU_BITS_PER_CHAR)
#else
	typedef int32_t mtime_t;  /*!< Type for time expressed in milliseconds. */
	typedef int32_t utime_t;  /*!< Type for time expressed in microseconds. */
	#define SIZEOF_MTIME_T (32 / CPU_BITS_PER_CHAR)
	#define SIZEOF_UTIME_T (32 / CPU_BITS_PER_CHAR)
#endif

/*! Bulk storage large enough for both pointers or integers. */
typedef void * iptr_t;
typedef const void * const_iptr_t;

typedef unsigned char sig_t;     /*!< Type for signal bits. */
typedef unsigned char sigmask_t; /*!< Type for signal masks. */
typedef unsigned char page_t;    /*!< Type for banked memory pages. */


/*!
 * \name Standard type definitions.
 *
 * These should be in <sys/types.h>, but many compilers lack them.
 *
 * \{
 */
#if !(defined(size_t) || defined(_SIZE_T_DEFINED))
	#if CPU_REG_BITS > 32
		/* 64bit. */
		typedef unsigned long size_t;
		typedef long ssize_t;
	#else
		/* 32bit or 16bit. */
		typedef unsigned int size_t;
		typedef int ssize_t;
	#endif
#endif

#if !(defined(_TIME_T_DEFINED) || defined(__time_t_defined) || defined(_EMUL))
	/*! For backwards compatibility.  Use mtime_t in new code. */
	#define time_t mtime_t
	#define SIZEOF_TIME_T SIZEOF_MTIME_T
#else
	/* Just a guess, but quite safe. */
	#define SIZEOF_TIME_T SIZEOF_LONG
#endif /* _TIME_T_DEFINED || __time_t_defined */
/*\}*/


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
	UNUSED_VAR(extern char,PP_CAT(CT_ASSERT___, __LINE__)[(condition) ? 1 : -1])

#endif /* DEVLIB_COMPILER_H */

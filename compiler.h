/*!
 * \file
 * <!--
 * Copyright 2003, 2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2001, 2002, 2003 Bernardo Innocenti <bernie@codewiz.org>
 * This file is part of DevLib - See devlib/README for information.
 * -->
 *
 * \version $Id$
 *
 * \author Bernardo Innocenti <bernie@develer.com>
 *
 * \brief Additional support macros for compiler independance
 */

/*
 * $Log$
 * Revision 1.6  2004/07/20 23:12:43  bernie
 * Reverse the logic to provide defaults for undefined macros.
 *
 * Revision 1.5  2004/07/20 17:08:03  bernie
 * Cleanup documentation
 *
 * Revision 1.4  2004/06/27 15:20:26  aleph
 * Change UNUSED() macro to accept two arguments: type and name;
 * Add macro GNUC_PREREQ to detect GCC version during build;
 * Some spacing cleanups and typo fix
 *
 * Revision 1.3  2004/06/06 18:00:39  bernie
 * PP_CAT(): New macro.
 *
 * Revision 1.2  2004/06/03 11:27:09  bernie
 * Add dual-license information.
 *
 * Revision 1.1  2004/05/23 17:48:35  bernie
 * Add top-level files.
 *
 */
#ifndef COMPILER_H
#define COMPILER_H

#include "arch_config.h"


#if defined __GNUC__ && defined __GNUC_MINOR__
	#define GNUC_PREREQ(maj, min) \
		((__GNUC__ << 16) + __GNUC_MINOR__ >= ((maj) << 16) + (min))
#else
	#define GNUC_PREREQ(maj, min) 0
#endif

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
	#define float double

	/* Ouch, ReleaseSemaphore() conflicts with a WIN32 call ;-( */
	#define ReleaseSemaphore KReleaseSemaphore

	/* Fake bool support */
	#ifndef __cplusplus
		#define true 1
		#define false 0
		typedef int bool;
	#endif /* !__cplusplus */

#elif defined(__GNUC__)

	/* GCC attributes */
	#define FORMAT(type,fmt,first)  __attribute__((__format__(type, fmt, first)))
	#define NORETURN                __attribute__((__noreturn__))
	#define UNUSED(type,arg)        __attribute__((__unused__)) type arg
	#define INLINE extern inline
	#if GNUC_PREREQ(3,1)
		#define DEPRECATED              __attribute__((__deprecated__))
	#endif

	#if defined(__i386__)

		/* hack to avoid conflicts with system type */
		#define sigset_t system_sigset_t
		#include <stddef.h>
		#include <setjmp.h>
		#include <stdbool.h>
		#undef system_sigset_t

	#elif defined(__AVR__)

		#include <stddef.h>
		#include <stdbool.h>

		/* Missing printf-family functions in avr-libc/stdio.h */
		#include <stdarg.h>
		#include <avr/pgmspace.h>
		#if !GNUC_PREREQ(3,4)
			int vsprintf(char *buf, const char *fmt, va_list ap);
		#endif
		int vsprintf_P(char *buf, const char * PROGMEM fmt, va_list ap);

		/* Support for harvard architectures */
		#ifdef _PROGMEM
			#define PGM_READ_CHAR(s) pgm_read_byte(s)
			#define PGM_FUNC(x) x ## _P
			#define PGM_ATTR        PROGMEM
		#endif

	#endif /* CPU */

#elif defined(__MWERKS__) && (defined(__m56800E__) || defined(__m56800__))

	#include <stdint.h>
	#include <stddef.h>
	#include <stdbool.h>
	#include <setjmp.h>

#else
	#error unknown compiler
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
#ifndef
#define DEPRECATED             /* nothing */
#endif
#ifndef
#define UNUSED(type,arg)       type arg
#endif
#ifndef REGISTER
#define REGISTER               /* nothing */
#endif
#ifndef INTERRUPT
#define INTERRUPT(x)           ERROR_NOT_IMPLEMENTED
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
#define NULL  0
#endif
#ifndef EOF
#define	EOF   (-1)
#endif


/* Support for hybrid C/C++ applications. */
#ifdef __cplusplus
	#define EXTERN_C_BEGIN  extern "C" {
	#define EXTERN_C_END    }
#else
	#define EXTERN_C_BEGIN  /* nothing */
	#define EXTERN_C_END    /* nothing */
#endif


/* Quasi-ANSI macros */
#ifndef offsetof
	/*! offsetof(s,m) - Return the byte offset of the member \a m in struct \a s */
	#define offsetof(s,m)   (size_t)&(((s *)0)->m)
#endif
#ifndef countof
	/*! Count the number of elements in the static array \a a */
	#define countof(a) (sizeof(a) / sizeof(*(a)))
#endif


/* Simple macros */
#define ABS(a)		(((a) < 0) ? -(a) : (a))
#define MIN(a,b)	(((a) < (b)) ? (a) : (b))
#define MAX(a,b)	(((a) > (b)) ? (a) : (b))

#ifndef BV
/*! Convert a bit value to a binary flag */
#define BV(x)	(1<<(x))
#endif

/*! Round up \a x to an even multiple of the 2's power \a pad */
#define ROUND2(x, pad) (((x) + ((pad) - 1)) & ~((pad) - 1))

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
#define PP_CAT(x,y)      PP_CAT__(x,y)
#define PP_CAT__(x,y)    x ## y


/*
 * Standard type definitions
 * These should be in <sys/types.h>, but many compilers lack them.
 */
#if !(defined(size_t) || defined(_SIZE_T_DEFINED))
	#define size_t unsigned int
#endif
#if !(defined(_TIME_T_DEFINED) || defined(__time_t_defined))
	typedef long time_t;
#endif /* _TIME_T_DEFINED || __time_t_defined */

/*! Storage for pointers and integers */
#define IPTR void *

typedef long utime_t;
typedef unsigned char sig_t;
typedef unsigned char sigset_t;
typedef unsigned char page_t;

#if (defined(_MSC_VER) || defined(__IAR_SYSTEMS_ICC) || defined(__IAR_SYSTEMS_ICC__))
	/*
	 * ISO C99 fixed-size types
	 * These should be in <stdint.h>, but many compilers lack them.
	 */
	typedef signed char         int8_t;
	typedef short int           int16_t;
	typedef long int            int32_t;
	typedef unsigned char       uint8_t;
	typedef unsigned short int  uint16_t;
	typedef unsigned long int   uint32_t;
#elif defined(__AVR__)
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
#if (defined(__m56800E__) || defined(__m56800__))
	/* Registers can be accessed only through 16-bit pointers */
	typedef volatile uint16_t  reg16_t;
#else
	typedef volatile uint8_t   reg8_t;
	typedef volatile uint16_t  reg16_t;
	typedef volatile uint32_t  reg32_t;
#endif
/*\}*/

#endif /* COMPILER_H */

/*!
 * \file
 * <!--
 * Copyright 2003,2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2001,2002,2003 Bernardo Innocenti <bernie@codewiz.org>
 * All Rights Reserved.
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
 * Revision 1.1  2004/05/23 17:48:35  bernie
 * Add top-level files.
 *
 */
#ifndef COMPILER_H
#define COMPILER_H

#include "arch_config.h"

#if defined(__IAR_SYSTEMS_ICC) || defined(__IAR_SYSTEMS_ICC__)
	#pragma language=extended
	#define INTERRUPT(x)	interrupt [x]
	#define REGISTER		shortad
	#define ATOMIC			monitor

	/* GCC attributes */
	#define FORMAT(type,fmt,first) /* nothing */
	#define NORETURN               /* nothing */
	#define UNUSED(arg)            arg

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

	#ifndef __cplusplus
		#define true 1
		#define false 0
		typedef unsigned char bool;
	#endif /* !__cplusplus */

#elif defined(_MSC_VER) /* Win32 emulation support */

	#include <setjmp.h>
	#include <time.h> /* for time_t */
	#define float double

	#define REGISTER		/* nothing */
	#define INTERRUPT(x)	/* nothing */

	/* GCC attributes */
	#define FORMAT(type,fmt,first) /* nothing */
	#define NORETURN               /* nothing */
	#define UNUSED(arg)            arg
	#define INLINE static inline

	#ifdef __cplusplus
		extern "C" {
	#endif
			void SchedulerIdle(void);
	#ifdef __cplusplus
		}
	#endif

	/* Ouch, ReleaseSemaphore() conflicts with a WIN32 call ;-( */
	#define ReleaseSemaphore KReleaseSemaphore

	#ifndef __cplusplus
		#define true 1
		#define false 0
		typedef int bool;
	#endif /* !__cplusplus */

#elif defined(__GNUC__)

	/* GCC attributes */
	#define FORMAT(type,fmt,first)  __attribute__((__format__(type, fmt, first)))
	#define NORETURN                __attribute__((__noreturn__))
	#define UNUSED(arg)             __attribute__((__unused__)) arg
	//FIXME #define INLINE static inline
	#define INLINE extern inline

	#if defined(__i386__)

		/* hack to avoid conflicts with system type */
		#define sigset_t system_sigset_t
		#include <stddef.h>
		#include <setjmp.h>
		#include <stdbool.h>
		#undef system_sigset_t

		#define REGISTER		/* nothing */
		#define INTERRUPT(x)	/* nothing */

		#ifdef __cplusplus
			extern "C" {
		#endif
				void SchedulerIdle(void);
		#ifdef __cplusplus
			}
		#endif

	#elif defined(__AVR__)

		#include <stddef.h>
		#include <stdbool.h>
		#define FLASH		__attribute__((progmem))
		#define REGISTER		/* nothing */

		/* Missing printf-family functions in avr-libc/stdio.h */
		#include <stdarg.h>
		#include <avr/pgmspace.h>
		int vsprintf(char *buf, const char *fmt, va_list ap);
		int vsprintf_P(char *buf, const char * PROGMEM fmt, va_list ap);

		/* Support for hardvard architectures */
		#ifdef _PROGMEM
			#define PGM_READ_CHAR(s) pgm_read_byte(s)
			#define PGM_FUNC(x) x ## _P
			#define PGM_ATTR	PROGMEM
		#else
			#define PGM_READ_CHAR(s) (*(s))
			#define PGM_FUNC(x) x
			#define PGM_ATTR	/*nop*/
		#endif

	#endif /* CPU */

#elif defined(__MWERKS__) && (defined(__m56800E__) || defined(__m56800__))

	#include <stdint.h>
	#include <stddef.h>
	#include <stdbool.h>
	#include <setjmp.h>

	#define FLASH           /* nothing */
	#define REGISTER		/* nothing */
	#define INTERRUPT(x)	ERROR_NOT_IMPLEMENTED
	#define SCHEDULER_IDLE	/* nothing */
	
	#define INLINE          static inline

	/* GCC attributes */
	#define FORMAT(type,fmt,first)  /* nothing */
	#define NORETURN                /* nothing */
	#define UNUSED(arg)             arg

	/* Support for hardvard architectures */
	#define PGM_READ_CHAR(s) (*(s))
	#define PGM_FUNC		/* nothing */
	#define PGM_ATTR		/* nothing */
	#define PSTR            /* nothing */

#else
	#error unknown compiler
#endif

/* Misc definitions */
#ifndef NULL
#define NULL	0
#endif
#ifndef EOF
#define	EOF		(-1)
#endif


/* Quasi-ANSI macros
 *
 * offsetof(s,m) - Give the byte offset of the member <m> in struct <s>
 * countof(a) - Count the number of elements in the static array <a>
 */
#ifndef offsetof
#define offsetof(s,m)   (size_t)&(((s *)0)->m)
#endif

#ifndef countof
#define countof(a) (sizeof(a) / sizeof(*(a)))
#endif

/* Simple macros */
#define ABS(a)		(((a) < 0) ? -(a) : (a))
#define MIN(a,b)	(((a) < (b)) ? (a) : (b))
#define MAX(a,b)	(((a) > (b)) ? (a) : (b))

/*! Convert a bit value to a binary flag */
#ifndef BV
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

/* Type definitions - should go in <sys/types.h> */
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

/* ISO C99 fixed-size types */
#if (defined(_MSC_VER) || defined(__IAR_SYSTEMS_ICC) || defined(__IAR_SYSTEMS_ICC__))
	typedef signed char         int8_t;
	typedef short int           int16_t;
	typedef long int            int32_t;
	typedef unsigned char       uint8_t;
	typedef unsigned short int  uint16_t;
	typedef unsigned long int   uint32_t;
#elif defined(__AVR__)
#	include <inttypes.h>
#else
#	include <stdint.h>
#endif

#if (defined(__m56800E__) || defined(__m56800__))
	/* Registers can be accessed only through 16-bit pointers */
	typedef volatile uint16_t  reg16_t;
#else
	typedef volatile uint8_t   reg8_t;
	typedef volatile uint16_t  reg16_t;
	typedef volatile uint32_t  reg32_t;
#endif

#endif /* COMPILER_H */

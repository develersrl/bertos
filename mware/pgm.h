/*!
 * \file
 * <!--
 * Copyright 2004, 2005 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2004 Giovanni Bajo
 * This file is part of DevLib - See README.devlib for information.
 * -->
 *
 * \brief Support for reading program memory on Harvard architectures.
 *
 * Support is currently provided for AVR microcontrollers only.
 *
 * These macros allow building code twice, with and without
 * pgm support (e.g.: strcpy() and strcpy_P()).
 *
 * Set the _PROGMEM predefine to compile in conditional
 * program-memory support.
 *
 *
 * \note This module contains code ripped out from avr-libc,
 *       which is distributed under a 3-clause BSD license.
 */
#ifndef MWARE_PGM_H
#define MWARE_PGM_H

#include <cpu_detect.h>

#if CPU_AVR

	#ifdef __AVR_ENHANCED__
		#define pgm_read_char(addr) \
		({ \
			uint16_t __addr16 = (uint16_t)(addr); \
			uint8_t __result; \
			__asm__ \
			( \
				"lpm %0, Z" "\n\t" \
				: "=r" (__result) \
				: "z" (__addr16) \
			); \
			__result; \
		})
	#else
		#define pgm_read_char(addr) \
		({ \
			uint16_t __addr16 = (uint16_t)(addr); \
			uint8_t __result; \
			__asm__ \
			( \
				"lpm" "\n\t" \
				"mov %0, r0" "\n\t" \
				: "=r" (__result) \
				: "z" (__addr16) \
				: "r0" \
			); \
			__result; \
		})
	#endif

	#define PROGMEM  __attribute__((__progmem__))
	#define PSTR(s) ({static const char __c[] PROGMEM = (s); __c;})

#elif CPU_HARVARD
	#error Missing CPU support
#endif

#ifndef PSTR
#define PSTR            /* nothing */
#endif

#ifndef PROGMEM
#define PROGMEM                /* nothing */
#endif

/*!
 * \name Types for variables stored in program memory (harvard processors).
 * \{
 */
typedef PROGMEM char pgm_char;
typedef PROGMEM int8_t pgm_int8_t;
typedef PROGMEM uint8_t pgm_uint8_t;
typedef PROGMEM int16_t pgm_int16_t;
typedef PROGMEM uint16_t pgm_uint16_t;
typedef PROGMEM int32_t pgm_int32_t;
typedef PROGMEM uint32_t pgm_uint32_t;
/*\}*/

/*!
 * \def PGM_READ_CHAR
 * \def PGM_FUNC
 * \def PGM_ATTR
 *
 * These macros enable dual compilation of code for both program
 * and data memory.
 *
 * Such a function may be defined like this:
 *
 * \code
 *	void PGM_FUNC(lcd_puts)(PGM_ATTR const char *str)
 *	{
 *		char c;
 *		while ((c = PGM_READ_CHAR(str++))
 *			lcd_putchar(c);
 *	}
 * \endcode
 *
 * The above code can be compiled twice: once with the _PROGMEM preprocessor
 * symbol defined, and once without.  The two object modules can then be
 * linked in the same application for use by client code:
 *
 * \code
 *	lcd_puts("Hello, world!");
 *	lcd_puts_P(PSTR("Hello, world!"));
 *
 *	// To be used when invoking inside other PGM_FUNC functions:
 *	PGM_FUNC(lcd_puts)(some_string);
 * \endcode
 */
#ifdef _PROGMEM
	#define PGM_READ_CHAR(s) pgm_read_char(s)
	#define PGM_FUNC(x)      x ## _P
	#define PGM_ATTR         PROGMEM
#else
	#define PGM_READ_CHAR(s) (*(s))
	#define PGM_FUNC(x)      x
	#define PGM_ATTR         /* nothing */
#endif


#endif /* MWARE_PGM_H */

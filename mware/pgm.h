/*!
 * \file
 * <!--
 * Copyright 2005 Develer S.r.l. (http://www.develer.com/)
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

#include <cfg/cpu_detect.h>
#include <cfg/compiler.h> /* For intXX_t */

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
		#if 0 // 128/103
		#define pgm_read_uint16_t(addr) \
		({ \
			uint32_t __addr32 = (uint32_t)(addr); \
			uint16_t __result; \
			__asm__ \
			( \
				"out %2, %C1"   "\n\t" \
				"movw r30, %1"  "\n\t" \
				"elpm %A0, Z+"  "\n\t" \
				"elpm %B0, Z"   "\n\t" \
				: "=r" (__result) \
				: "r" (__addr32), \
				  "I" (_SFR_IO_ADDR(RAMPZ)) \
				: "r30", "r31" \
			); \
			__result; \
		})
		#endif

		#define pgm_read_uint16_t(addr) \
		({ \
			uint16_t __addr16 = (uint16_t)(addr); \
			uint16_t __result; \
			__asm__ \
			( \
				"lpm %A0, Z+"   "\n\t" \
				"lpm %B0, Z"    "\n\t" \
				: "=r" (__result), "=z" (__addr16) \
				: "1" (__addr16) \
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
		#if 0 // 128/103
		#define pgm_read_uint16_t(addr) \
		({ \
			uint32_t __addr32 = (uint32_t)(addr); \
			uint16_t __result; \
			__asm__ \
			( \
				"out %2, %C1"   "\n\t" \
				"mov r31, %B1"  "\n\t" \
				"mov r30, %A1"  "\n\t" \
				"elpm"          "\n\t" \
				"mov %A0, r0"   "\n\t" \
				"in r0, %2"     "\n\t" \
				"adiw r30, 1"   "\n\t" \
				"adc r0, __zero_reg__" "\n\t" \
				"out %2, r0"    "\n\t" \
				"elpm"          "\n\t" \
				"mov %B0, r0"   "\n\t" \
				: "=r" (__result) \
				: "r" (__addr32), \
				  "I" (_SFR_IO_ADDR(RAMPZ)) \
				: "r0", "r30", "r31" \
			); \
			__result; \
		})
		#endif
		#define pgm_read_uint16_t(addr) \
		({ \
			uint16_t __addr16 = (uint16_t)(addr); \
			uint16_t __result; \
			__asm__ \
			( \
				"lpm"           "\n\t" \
				"mov %A0, r0"   "\n\t" \
				"adiw r30, 1"   "\n\t" \
				"lpm"           "\n\t" \
				"mov %B0, r0"   "\n\t" \
				: "=r" (__result), "=z" (__addr16) \
				: "1" (__addr16) \
				: "r0" \
			); \
			__result; \
		})

	#endif

	#ifndef PROGMEM
	#define PROGMEM  __attribute__((__progmem__))
	#endif
	#ifndef PSTR
	#define PSTR(s) ({ static const char __c[] PROGMEM = (s); &__c[0]; })
	#endif

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
 * \name PGM support macros.
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
 *
 * \{
 */
#ifdef _PROGMEM
	#define PGM_READ_CHAR(s) pgm_read_char(s)
	#define PGM_FUNC(x)      x ## _P
	#define PGM_STR(x)	 PSTR(x)
	#define PGM_ATTR         PROGMEM
#else
	#define PGM_READ_CHAR(s) (*(s))
	#define PGM_FUNC(x)      x
	#define PGM_STR(x)	 x
	#define PGM_ATTR         /* nothing */
#endif
/* \} */


#endif /* MWARE_PGM_H */

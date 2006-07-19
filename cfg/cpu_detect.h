/**
 * \file
 * <!--
 * Copyright 2004, 2005 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2004 Giovanni Bajo
 * This file is part of DevLib - See README.devlib for information.
 * -->
 *
 * \brief CPU detection through special preprocessor macros
 *
 */

/*#*
 *#* $Log$
 *#* Revision 1.4  2006/07/19 12:56:25  bernie
 *#* Convert to new Doxygen style.
 *#*
 *#* Revision 1.3  2006/02/10 12:37:37  bernie
 *#* Add support for ARM on IAR.
 *#*
 *#* Revision 1.2  2005/06/14 06:15:10  bernie
 *#* Add X86_64 support.
 *#*
 *#* Revision 1.1  2005/04/11 19:04:13  bernie
 *#* Move top-level headers to cfg/ subdir.
 *#*
 *#* Revision 1.4  2005/02/16 20:33:24  bernie
 *#* Preliminary PPC support.
 *#*
 *#* Revision 1.3  2004/12/31 17:39:26  bernie
 *#* Use C89 comments only.
 *#*
 *#* Revision 1.2  2004/08/25 14:12:08  rasky
 *#* Aggiornato il comment block dei log RCS
 *#*
 *#* Revision 1.1  2004/07/30 17:14:49  rasky
 *#* File sfuggito al commit precedente (nuova gestione unificata del detect della CPU
 *#*
 *#* Revision 1.2  2004/07/30 10:31:07  rasky
 *#* Aggiunto detect per ATmega128
 *#*/

#ifndef CPU_DETECT_H
#define CPU_DETECT_H

#if defined(__arm__) /* GCC */ \
	|| defined(__ARM4TM__) /* IAR: defined for all cores >= 4tm */
	#define CPU_ARM                 1
	#define CPU_ID                  arm
#else
	#define CPU_ARM                 0
#endif

#if (defined(__IAR_SYSTEMS_ICC__) || defined(__IAR_SYSTEMS_ICC)) \
	&& !defined(__ARM4TM__) /* IAR: if not ARM assume I196 */
	#define CPU_I196		1
	#define CPU_ID                  i196
#else
	#define CPU_I196                0
#endif

#if defined(__i386__) /* GCC */ \
	|| (defined(_M_IX86) && !defined(_WIN64)) /* MSVC */
	#define CPU_X86                 1
	#define CPU_X86_32              1
	#define CPU_X86_64              0
	#define CPU_ID                  x86
#elif defined(__x86_64__) /* GCC */ \
	|| (defined(_M_IX86) && defined(_WIN64)) /* MSVC */
	#define CPU_X86                 1
	#define CPU_X86_32              0
	#define CPU_X86_64              1
	#define CPU_ID                  x86
#else
	#define CPU_X86                 0
	#define CPU_I386                0
	#define CPU_X86_64              0
#endif

#if defined (_ARCH_PPC) || defined(_ARCH_PPC64)
	#define CPU_PPC                 1
	#define CPU_ID                  ppc
	#if defined(_ARCH_PPC)
		#define CPU_PPC32       1
	#else
	        #define CPU_PPC32       0
	#endif
	#if defined(_ARCH_PPC64)
		#define CPU_PPC64       1
	#else
	        #define CPU_PPC64       0
	#endif
#else
	#define CPU_PPC                 0
	#define CPU_PPC32               0
	#define CPU_PPC64               0
#endif

#if defined(__m56800E__) || defined(__m56800__)
	#define CPU_DSP56K              1
	#define CPU_ID                  dsp56k
#else
	#define CPU_DSP56K              0
#endif

#if defined (__AVR__)
	#define CPU_AVR                 1
	#define CPU_ID                  avr

	#if defined(__AVR_ATmega64__)
		#define CPU_AVR_ATMEGA64    1
	#else
		#define CPU_AVR_ATMEGA64    0
	#endif

	#if defined(__AVR_ATmega103__)
		#define CPU_AVR_ATMEGA103   1
	#else
		#define CPU_AVR_ATMEGA103   0
	#endif

	#if defined(__AVR_ATmega128__)
		#define CPU_AVR_ATMEGA128   1
	#else
		#define CPU_AVR_ATMEGA128   0
	#endif

	#if defined(__AVR_ATmega8__)
		#define CPU_AVR_ATMEGA8     1
	#else
		#define CPU_AVR_ATMEGA8     0
	#endif
#else
	#define CPU_AVR                 0
	#define CPU_AVR_ATMEGA8         0
	#define CPU_AVR_ATMEGA64        0
	#define CPU_AVR_ATMEGA103       0
	#define CPU_AVR_ATMEGA128       0
#endif


/* Self-check for the detection: only one CPU must be detected */
#if CPU_ARM + CPU_I196 + CPU_X86 + CPU_PPC + CPU_DSP56K + CPU_AVR == 0
	#error Unknown CPU
#elif !defined(CPU_ID)
	#error CPU_ID not defined
#elif CPU_ARM + CPU_I196 + CPU_X86 + CPU_PPC + CPU_DSP56K + CPU_AVR != 1
	#error Internal CPU configuration error
#endif


#endif /* CPU_DETECT_H */

/*!
 * \file
 * <!--
 * Copyright 2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2004 Giovanni Bajo
 * This file is part of DevLib - See devlib/README for information.
 * -->
 *
 * \brief CPU detection through special preprocessor macros
 *
 */

/*#*
 *#* $Log$
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
 *#*
 *#* Revision 1.1  2004/07/22 16:37:34  rasky
 *#* Nuovo file cpu_detect.h per il detect della CPU
 *#* Nuova macro CPU_HEADER per includere l'header cpu-specific di ogni modulo
 *#* Razionalizzazione dei principali #ifdef per piattaforma con le nuove macro
 *#* Nuove macro-utility PP_STRINGIZE e PP_CATn
 *#*
 *#*/

#ifndef CPU_DETECT_H
#define CPU_DETECT_H

#if defined(__IAR_SYSTEMS_ICC) || defined(__IAR_SYSTEMS_ICC__)
	#define CPU_I196                1
	#define CPU_ID                  i196
#else
	#define CPU_I196                0
#endif

#if defined(__i386__) || defined(_MSC_VER)
	#define CPU_X86                 1
	#define CPU_ID                  x86
#else
	#define CPU_X86                 0
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
#if CPU_I196 + CPU_X86 + CPU_DSP56K + CPU_AVR == 0
	#error Unknown CPU
#elif !defined(CPU_ID)
	#error CPU_ID not defined
#elif CPU_I196 + CPU_X86 + CPU_DSP56K + CPU_AVR != 1
	#error Internal CPU configuration error
#endif


#endif /* CPU_DETECT_H */

/**
 * \file
 * <!--
 * Copyright 2003, 2004, 2005 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2000 Bernardo Innocenti <bernie@develer.com>
 * This file is part of DevLib - See README.devlib for information.
 * -->
 *
 * \version $Id$
 *
 * \author Bernardo Innocenti <bernie@develer.com>
 * \author Francesco Sacchi <batt@develer.com>
 *
 * \brief Low-level timer module for AVR (interface).
 */

/*#*
 *#* $Log$
 *#* Revision 1.30  2007/06/07 14:35:12  batt
 *#* Merge from project_ks.
 *#*
 *#* Revision 1.29  2007/03/21 11:01:36  batt
 *#* Add missing support for ATMega1281.
 *#*
 *#* Revision 1.28  2006/07/19 12:56:26  bernie
 *#* Convert to new Doxygen style.
 *#*
 *#* Revision 1.27  2006/05/18 00:38:24  bernie
 *#* Use hw_cpu.h instead of ubiquitous hw.h.
 *#*
 *#* Revision 1.26  2006/02/21 21:28:02  bernie
 *#* New time handling based on TIMER_TICKS_PER_SEC to support slow timers with ticks longer than 1ms.
 *#*
 *#* Revision 1.25  2005/07/19 07:26:37  bernie
 *#* Refactor to decouple timer ticks from milliseconds.
 *#*
 *#* Revision 1.24  2005/04/11 19:10:28  bernie
 *#* Include top-level headers from cfg/ subdir.
 *#*
 *#* Revision 1.23  2005/03/01 23:24:51  bernie
 *#* Tweaks for avr-libc 1.2.x.
 *#*
 *#* Revision 1.21  2004/12/13 12:07:06  bernie
 *#* DISABLE_IRQSAVE/ENABLE_IRQRESTORE: Convert to IRQ_SAVE_DISABLE/IRQ_RESTORE.
 *#*
 *#* Revision 1.20  2004/11/16 20:59:46  bernie
 *#* Include <avr/io.h> explicitly.
 *#*
 *#* Revision 1.19  2004/10/19 08:56:41  bernie
 *#* TIMER_STROBE_ON, TIMER_STROBE_OFF, TIMER_STROBE_INIT: Move from timer_avr.h to timer.h, where they really belong.
 *#*
 *#* Revision 1.18  2004/09/20 03:31:03  bernie
 *#* Fix racy racy code.
 *#*/
#ifndef DRV_TIMER_AVR_H
#define DRV_TIMER_AVR_H

#include <appconfig.h>     /* CONFIG_TIMER */
#include <cfg/compiler.h>  /* uint8_t */
#include <hw_cpu.h>        /* CLOCK_FREQ */

/**
 * \name Values for CONFIG_TIMER.
 *
 * Select which hardware timer interrupt to use for system clock and softtimers.
 * \note The timer 1 overflow mode set the timer as a 24 kHz PWM.
 *
 * \{
 */
#define TIMER_ON_OUTPUT_COMPARE0  1
#define TIMER_ON_OVERFLOW1        2
#define TIMER_ON_OUTPUT_COMPARE2  3
#define TIMER_ON_OVERFLOW3        4
/* \} */

/*
 * Hardware dependent timer initialization.
 */
#if (CONFIG_TIMER == TIMER_ON_OUTPUT_COMPARE0)

	#define TIMER_PRESCALER      64
	#define TIMER_HW_BITS        8
	#define DEFINE_TIMER_ISR     SIGNAL(SIG_OUTPUT_COMPARE0)
	#define TIMER_TICKS_PER_SEC  1000
	#define TIMER_HW_CNT         OCR_DIVISOR

	/// Type of time expressed in ticks of the hardware high-precision timer
	typedef uint8_t hptime_t;

#elif (CONFIG_TIMER == TIMER_ON_OVERFLOW1)

	#define TIMER_PRESCALER      1
	#define TIMER_HW_BITS        8
	/** This value is the maximum in overflow based timers. */
	#define TIMER_HW_CNT         (1 << TIMER_HW_BITS)
	#define DEFINE_TIMER_ISR     SIGNAL(SIG_OVERFLOW1)
	#define TIMER_TICKS_PER_SEC  ((TIMER_HW_HPTICKS_PER_SEC + TIMER_HW_CNT / 2) / TIMER_HW_CNT)

	/// Type of time expressed in ticks of the hardware high precision timer
	typedef uint16_t hptime_t;

#elif (CONFIG_TIMER == TIMER_ON_OUTPUT_COMPARE2)

	#define TIMER_PRESCALER      64
	#define TIMER_HW_BITS        8
	#if CPU_AVR_ATMEGA1281 || CPU_AVR_ATMEGA168
		#define DEFINE_TIMER_ISR     SIGNAL(SIG_OUTPUT_COMPARE2A)
	#else
		#define DEFINE_TIMER_ISR     SIGNAL(SIG_OUTPUT_COMPARE2)
	#endif
	#define TIMER_TICKS_PER_SEC  1000
	/** Value for OCR register in output-compare based timers. */
	#define TIMER_HW_CNT         OCR_DIVISOR


	/// Type of time expressed in ticks of the hardware high precision timer
	typedef uint8_t hptime_t;

#elif (CONFIG_TIMER == TIMER_ON_OVERFLOW3)

	#define TIMER_PRESCALER      1
	#define TIMER_HW_BITS        8
	/** This value is the maximum in overflow based timers. */
	#define TIMER_HW_CNT         (1 << TIMER_HW_BITS)
	#define DEFINE_TIMER_ISR     SIGNAL(SIG_OVERFLOW3)
	#define TIMER_TICKS_PER_SEC  ((TIMER_HW_HPTICKS_PER_SEC + TIMER_HW_CNT / 2) / TIMER_HW_CNT)

	/// Type of time expressed in ticks of the hardware high precision timer
	typedef uint16_t hptime_t;
#else

	#error Unimplemented value for CONFIG_TIMER
#endif /* CONFIG_TIMER */


/** Frequency of the hardware high precision timer. */
#define TIMER_HW_HPTICKS_PER_SEC  ((CLOCK_FREQ + TIMER_PRESCALER / 2) / TIMER_PRESCALER)

/**
 * System timer: additional division after the prescaler
 * 12288000 / 64 / 192 (0..191) = 1 ms
 */
#define OCR_DIVISOR  (((CLOCK_FREQ + TIMER_PRESCALER / 2) / TIMER_PRESCALER + TIMER_TICKS_PER_SEC / 2) / TIMER_TICKS_PER_SEC - 1)

/** Not needed, IRQ timer flag cleared automatically */
#define timer_hw_irq() do {} while (0)


#endif /* DRV_TIMER_AVR_H */

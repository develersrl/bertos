/*!
 * \file
 * <!--
 * Copyright 2003, 2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2000 Bernardo Innocenti <bernie@develer.com>
 * This file is part of DevLib - See devlib/README for information.
 * -->
 *
 * \version $Id$
 *
 * \author Bernardo Innocenti <bernie@develer.com>
 *
 * \brief Low-level timer module for AVR
 */

/*#*
 *#* $Log$
 *#* Revision 1.19  2004/10/19 08:56:41  bernie
 *#* TIMER_STROBE_ON, TIMER_STROBE_OFF, TIMER_STROBE_INIT: Move from timer_avr.h to timer.h, where they really belong.
 *#*
 *#* Revision 1.18  2004/09/20 03:31:03  bernie
 *#* Fix racy racy code.
 *#*
 *#* Revision 1.17  2004/09/14 21:07:09  bernie
 *#* Include hw.h explicitly.
 *#*
 *#* Revision 1.16  2004/09/06 21:49:26  bernie
 *#* CONFIG_TIMER_STROBE: be tolerant with missing optional macro.
 *#*
 *#* Revision 1.15  2004/08/25 14:12:08  rasky
 *#* Aggiornato il comment block dei log RCS
 *#*
 *#* Revision 1.14  2004/08/24 16:27:01  bernie
 *#* Add missing headers.
 *#*
 *#* Revision 1.13  2004/08/24 14:30:11  bernie
 *#* Use new-style config macros for drv/timer.c
 *#*
 *#* Revision 1.12  2004/08/10 06:59:45  bernie
 *#* CONFIG_TIMER_STROBE: Define no-op default macros.
 *#*
 *#* Revision 1.11  2004/08/03 15:53:17  aleph
 *#* Fix spacing
 *#*
 *#* Revision 1.10  2004/08/02 20:20:29  aleph
 *#* Merge from project_ks
 *#*
 *#* Revision 1.9  2004/07/22 02:01:14  bernie
 *#* Use TIMER_PRESCALER consistently.
 *#*/
#ifndef DRV_TIMER_AVR_H
#define DRV_TIMER_AVR_H

#include <arch_config.h> // ARCH_BOARD_KC
#include "hw.h"

#include <avr/wdt.h>
#include <avr/signal.h>

#if defined(ARCH_BOARD_KC) && (ARCH & ARCH_BOARD_KC)
	#include <drv/adc.h>
#endif


/*!
 * Values for CONFIG_TIMER.
 *
 * Select which hardware timer interrupt to use for system clock and softtimers.
 * \note The timer 1 overflow mode set the timer as a 24 kHz PWM.
 */
#define TIMER_ON_OUTPUT_COMPARE0  1
#define TIMER_ON_OVERFLOW1        2
#define TIMER_ON_OUTPUT_COMPARE2  3


/* Not needed, IRQ timer flag cleared automatically */
#define timer_hw_irq() do {} while (0)

#define TIMER_PRESCALER 64

/*!
 * System timer: additional division after the prescaler
 * 12288000 / 64 / 192 (0..191) = 1 ms
 */
#define OCR_DIVISOR  (CLOCK_FREQ / TIMER_PRESCALER / TICKS_PER_SEC - 1) /* 191 */

/*! HW dependent timer initialization  */
#if (CONFIG_TIMER == TIMER_ON_OUTPUT_COMPARE0)

	//! Type of time expressed in ticks of the hardware high-precision timer
	typedef uint8_t hptime_t;

	static void timer_hw_init(void)
	{
		cpuflags_t flags;
		DISABLE_IRQSAVE(flags);

		/* Reset Timer flags */
		TIFR = BV(OCF0) | BV(TOV0);

		/* Setup Timer/Counter interrupt */
		ASSR = 0x00;                  /* Internal system clock */
		TCCR0 = BV(WGM01)             /* Clear on Compare match */
			#if TIMER_PRESCALER == 64
				| BV(CS02)
			#else
				#error Unsupported value of TIMER_PRESCALER
			#endif
		;
		TCNT0 = 0x00;                 /* Initialization of Timer/Counter */
		OCR0 = OCR_DIVISOR;           /* Timer/Counter Output Compare Register */

		/* Enable timer interrupts: Timer/Counter2 Output Compare (OCIE2) */
		TIMSK &= ~BV(TOIE0);
		TIMSK |= BV(OCIE0);

		ENABLE_IRQRESTORE(flags);
	}

	//! Frequency of the hardware high precision timer
	#define TIMER_HW_HPTICKS_PER_SEC  (CLOCK_FREQ / TIMER_PRESCALER)

	INLINE hptime_t timer_hw_hpread(void)
	{
		return TCNT0;
	}

#elif (CONFIG_TIMER == TIMER_ON_OVERFLOW1)

	//! Type of time expressed in ticks of the hardware high precision timer
	typedef uint16_t hptime_t;

	static void timer_hw_init(void)
	{
		cpuflags_t flags;
		DISABLE_IRQSAVE(flags);

		/* Reset Timer overflow flag */
		TIFR |= BV(TOV1);

		/* Fast PWM mode, 9 bit, 24 kHz, no prescaling. When changing freq or
		   resolution (top of TCNT), change TIMER_HW_HPTICKS_PER_SEC too */
		TCCR1A |= BV(WGM11);
		TCCR1A &= ~BV(WGM10);
		TCCR1B |= BV(WGM12) | BV(CS10);
		TCCR1B &= ~(BV(WGM13) | BV(CS11) | BV(CS12));

		TCNT1 = 0x00;         /* initialization of Timer/Counter */

		/* Enable timer interrupt: Timer/Counter1 Overflow */
		TIMSK |= BV(TOIE1);

		ENABLE_IRQRESTORE(flags);
	}

	//! Frequency of the hardware high precision timer
	#define TIMER_HW_HPTICKS_PER_SEC  (24000ul * 512)

	INLINE hptime_t timer_hw_hpread(void)
	{
		return TCNT1;
	}

#elif (CONFIG_TIMER == TIMER_ON_OUTPUT_COMPARE2)

	//! Type of time expressed in ticks of the hardware high precision timer
	typedef uint8_t hptime_t;

	static void timer_hw_init(void)
	{
		cpuflags_t flags;
		DISABLE_IRQSAVE(flags);

		/* Reset Timer flags */
		TIFR = BV(OCF2) | BV(TOV2);

		/* Setup Timer/Counter interrupt */
		TCCR2 = BV(WGM21)
			#if TIMER_PRESCALER == 64
				| BV(CS21) | BV(CS20)
			#else
				#error Unsupported value of TIMER_PRESCALER
			#endif
		;
		/* Clear on Compare match & prescaler = 64, internal sys clock.
		   When changing prescaler change TIMER_HW_HPTICKS_PER_SEC too */
		TCNT2 = 0x00;         /* initialization of Timer/Counter */
		OCR2 = OCR_DIVISOR;   /* Timer/Counter Output Compare Register */

		/* Enable timer interrupts: Timer/Counter2 Output Compare (OCIE2) */
		TIMSK &= ~BV(TOIE2);
		TIMSK |= BV(OCIE2);

		ENABLE_IRQRESTORE(flags);
	}

	//! Frequency of the hardware high precision timer
	#define TIMER_HW_HPTICKS_PER_SEC  (CLOCK_FREQ / TIMER_PRESCALER)

	INLINE hptime_t timer_hw_hpread(void)
	{
		return TCNT2;
	}

#else
	#error Unimplemented value for CONFIG_TIMER
#endif /* CONFIG_TIMER */


#if (CONFIG_TIMER == TIMER_ON_OVERFLOW1)

	#define DEFINE_TIMER_ISR	\
		static void timer_handler(void)

	DEFINE_TIMER_ISR;

	/*
	 * Timer 1 overflow irq handler. It's called at the frequency of the timer 1
	 * PWM (should be 24 kHz). It's too much for timer purposes, so the interrupt
	 * handler is really a counter that call the true handler in timer.c
	 * every 1 ms.
	 */
	SIGNAL(SIG_OVERFLOW1)
	{
	#if (ARCH & ARCH_BOARD_KC)
		/*
		 * Super-optimization-hack: switch CPU ADC mux here, ASAP after the start
		 * of conversion (auto-triggered with timer 1 overflow).
		 * The switch can be done 2 ADC cycles after start of conversion.
		 * The handler prologue takes a little more than 32 CPU cycles: with
		 * the prescaler at 1/16 the timing should be correct even at the start
		 * of the handler.
		 *
		 * The switch is synchronized with the ADC handler using _adc_trigger_lock.
		 *
		 *      Mel (A Real Programmer)
		 */
		extern uint8_t _adc_idx_next;
		extern bool _adc_trigger_lock;

		if (!_adc_trigger_lock)
		{
			/*
			 * Disable free-running mode to avoid starting a
			 * new conversion before the ADC handler has read
			 * the ongoing one.  This condition could occur
			 * under very high interrupt load and would have the
			 * unwanted effect of reading from the wrong ADC
			 * channel.
			 *
			 * NOTE: writing 0 to ADSC and ADIF has no effect.
			 */
			ADCSRA = ADCSRA & ~(BV(ADFR) | BV(ADIF) | BV(ADSC));

			ADC_SETCHN(_adc_idx_next);
			_adc_trigger_lock = true;
		}
	#endif // ARCH_BOARD_KC

		/*!
		 * How many timer overflows we must count before calling the real
		 * timer handler.
		 * When the timer is programmed to overflow at 24 kHz, a value of
		 * 24 will result in 1ms between each call.
		 */
		#define TIMER1_OVF_COUNT 24
		//#warning TIMER1_OVF_COUNT for timer at 12 kHz
		//#define TIMER1_OVF_COUNT 12

		static uint8_t count = TIMER1_OVF_COUNT;

		count--;
		if (!count)
		{
			timer_handler();
			count = TIMER1_OVF_COUNT;
		}
	}

#elif (CONFIG_TIMER == TIMER_ON_OUTPUT_COMPARE0)

	#define DEFINE_TIMER_ISR	\
		SIGNAL(SIG_OUTPUT_COMPARE0)

#elif (CONFIG_TIMER == TIMER_ON_OUTPUT_COMPARE2)

	#define DEFINE_TIMER_ISR	\
		SIGNAL(SIG_OUTPUT_COMPARE2)

#else
	#error Unimplemented value for CONFIG_TIMER
#endif /* CONFIG_TIMER */

#endif /* DRV_TIMER_AVR_H */

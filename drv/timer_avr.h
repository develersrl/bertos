/*!
 * \file
 * <!--
 * Copyright 2003,2004 Develer S.r.l. (http://www.develer.com/)
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

/*
 * $Log$
 * Revision 1.11  2004/08/03 15:53:17  aleph
 * Fix spacing
 *
 * Revision 1.10  2004/08/02 20:20:29  aleph
 * Merge from project_ks
 *
 * Revision 1.9  2004/07/22 02:01:14  bernie
 * Use TIMER_PRESCALER consistently.
 *
 * Revision 1.8  2004/07/20 23:50:20  bernie
 * Also define TIMER_PRESCALER.
 *
 * Revision 1.7  2004/07/20 23:49:40  bernie
 * Compute value of OCR_DIVISOR from CLOCK_FREQ.
 *
 * Revision 1.6  2004/07/20 23:48:16  bernie
 * Finally remove redundant protos.
 *
 * Revision 1.5  2004/07/18 22:16:35  bernie
 * Add missing header; Prevent warning for project_ks-specific code.
 *
 * Revision 1.4  2004/06/27 15:22:15  aleph
 * Fix spacing
 *
 * Revision 1.3  2004/06/07 15:57:40  aleph
 * Update to latest AVR timer code
 *
 * Revision 1.2  2004/06/03 11:27:09  bernie
 * Add dual-license information.
 *
 * Revision 1.1  2004/05/23 18:23:30  bernie
 * Import drv/timer module.
 *
 */

#ifndef DRV_TIMER_AVR_H
#define DRV_TIMER_AVR_H

#include <avr/wdt.h>
#include <avr/signal.h>

#if defined(ARCH_BOARD_KC) && (ARCH & ARCH_BOARD_KC)
	#include <drv/adc.h>
#endif


/* Not needed, IRQ timer flag cleared automatically */
#define timer_hw_irq() do {} while (0)

#define TIMER_PRESCALER 64

/*!
 * System timer: additional division after the prescaler
 * 12288000 / 64 / 192 (0..191) = 1 ms
 */
#define OCR_DIVISOR  (CLOCK_FREQ / TIMER_PRESCALER / TICKS_PER_SEC - 1) /* 191 */

/*! HW dependent timer initialization  */
#if defined(CONFIG_TIMER_ON_TIMER0)

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

#elif defined(CONFIG_TIMER_ON_TIMER1_OVERFLOW)

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

#elif defined(CONFIG_TIMER_ON_TIMER2)

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
	#error Choose witch timer to use with CONFIG_TIMER_ON_TIMERx
#endif /* CONFIG_TIMER_ON_TIMERx */


#if defined(CONFIG_TIMER_ON_TIMER1_OVERFLOW)

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
		/*!
		 * How many overflow we have to count before calling the true timer handler.
		 * If timer overflow is at 24 kHz, with a value of 24 we have 1 ms between
		 * each call.
		 */
		#define TIMER1_OVF_COUNT 24

		static uint8_t count = TIMER1_OVF_COUNT;

		count--;
		if (!count)
		{
			timer_handler();
			count = TIMER1_OVF_COUNT;
		}

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
		 */
		extern uint8_t _adc_idx_next;
		extern bool _adc_trigger_lock;

		if (!_adc_trigger_lock)
		{
			TIMER_STROBE_ON;
			ADC_SETCHN(_adc_idx_next);
			TIMER_STROBE_OFF;
			_adc_trigger_lock = true;
		}
	#endif
	}

#elif defined (CONFIG_TIMER_ON_TIMER0)

	#define DEFINE_TIMER_ISR	\
		SIGNAL(SIG_OUTPUT_COMPARE0)

#elif defined(CONFIG_TIMER_ON_TIMER2)

	#define DEFINE_TIMER_ISR	\
		SIGNAL(SIG_OUTPUT_COMPARE2)

#else
	#error Choose witch timer to use with CONFIG_TIMER_ON_TIMERx
#endif /* CONFIG_TIMER_ON_TIMERx */

#endif /* DRV_TIMER_AVR_H */

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

#	define TIMER_RETRIGGER /* Not needed, timer retriggers automatically  */

	/*!
	 * System timer: additional division after the prescaler
	 * 12288000 / 64 / 192 (0..191) = 1 ms
	 */
#	define OCR_DIVISOR 191

	/*! HW dependent timer initialization  */
#if defined(CONFIG_TIMER_ON_TIMER0)

#		define TIMER_INIT \
		do { \
			DISABLE_INTS; \
			\
			/* Reset Timer flags */ \
			TIFR = BV(OCF0) | BV(TOV0); \
			\
			/* Setup Timer/Counter interrupt */ \
			ASSR = 0x00;                  /* internal system clock */ \
			TCCR0 = BV(WGM01) | BV(CS02); /* Clear on Compare match & prescaler = 64 */ \
			TCNT0 = 0x00;                 /* initialization of Timer/Counter */ \
			OCR0 = OCR_DIVISOR;           /* Timer/Counter Output Compare Register */ \
			\
			/* Enable timer interrupts: Timer/Counter2 Output Compare (OCIE2) */ \
			TIMSK &= ~BV(TOIE0); \
			TIMSK |= BV(OCIE0); \
			\
			ENABLE_INTS; \
		} while (0)

#elif defined(CONFIG_TIMER_ON_TIMER1_OVERFLOW)

#		define TIMER_INIT \
		do { \
			DISABLE_INTS; \
			\
			/* Reset Timer overflow flag */ \
			TIFR |= BV(TOV1); \
			\
			/* Fast PWM mode, 24 kHz, no prescaling */ \
			TCCR1A |= BV(WGM11); \
			TCCR1A &= ~BV(WGM10); \
			TCCR1B |= BV(WGM12) | BV(CS10); \
			TCCR1B &= ~(BV(WGM13) | BV(CS11) | BV(CS12)); \
			\
			TCNT1 = 0x00;         /* initialization of Timer/Counter */ \
			\
			/* Enable timer interrupt: Timer/Counter1 Overflow */ \
			TIMSK |= BV(TOIE1); \
			\
			ENABLE_INTS; \
		} while (0)

#elif defined(CONFIG_TIMER_ON_TIMER2)

#		define TIMER_INIT \
		do { \
			DISABLE_INTS; \
			\
			/* Reset Timer flags */ \
			TIFR = BV(OCF2) | BV(TOV2); \
			\
			/* Setup Timer/Counter interrupt */ \
			TCCR2 = BV(WGM21) | BV(CS21) | BV(CS20); \
			                      /* Clear on Compare match & prescaler = 64, internal sys clock */ \
			TCNT2 = 0x00;         /* initialization of Timer/Counter */ \
			OCR2 = OCR_DIVISOR;   /* Timer/Counter Output Compare Register */ \
			\
			/* Enable timer interrupts: Timer/Counter2 Output Compare (OCIE2) */ \
			TIMSK &= ~BV(TOIE2); \
			TIMSK |= BV(OCIE2); \
			\
			ENABLE_INTS; \
		} while (0)

#else
#		error Choose witch timer to use with CONFIG_TIMER_ON_TIMERx
#endif /* CONFIG_TIMER_ON_TIMERx */


#if defined(CONFIG_TIMER_ON_TIMER1_OVERFLOW)

	#define DEFINE_TIMER_ISR	\
		static void timer_handler(void)

	/*
	* Timer 1 overflow irq handler.
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
	}

#elif defined (CONFIG_TIMER_ON_TIMER0)

	#define DEFINE_TIMER_ISR	\
		SIGNAL(SIG_OUTPUT_COMPARE0)

#elif defined(CONFIG_TIMER_ON_TIMER2)

	#define DEFINE_TIMER_ISR	\
		SIGNAL(SIG_OUTPUT_COMPARE2)

#else
#	error Choose witch timer to use with CONFIG_TIMER_ON_TIMERx
#endif /* CONFIG_TIMER_ON_TIMERx */

#endif /* DRV_TIMER_AVR_H */

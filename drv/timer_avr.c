/*!
 * \file
 * <!--
 * Copyright 2005 Develer S.r.l. (http://www.develer.com/)
 * This file is part of DevLib - See README.devlib for information.
 * -->
 *
 * \version $Id$
 *
 * \author Bernardo Innocenti <bernie@develer.com>
 * \author Francesco Sacchi <batt@develer.com>
 *
 * \brief Low-level timer module for AVR (implementation).
 */

/*#*
 *#* $Log$
 *#* Revision 1.1  2005/07/19 07:28:36  bernie
 *#* Refactor to decouple timer ticks from milliseconds.
 *#*
 *#* Revision 1.1  2005/05/24 09:17:58  batt
 *#* Move drivers to top-level.
 *#*
 *#*/
#include <drv/timer_avr.h>
#include <arch_config.h> // ARCH_BOARD_KC
#include <cfg/macros.h> // BV()
#include <hw.h>

#include <avr/signal.h>
#include <avr/io.h>

/*! HW dependent timer initialization  */
#if (CONFIG_TIMER == TIMER_ON_OUTPUT_COMPARE0)

	static void timer_hw_init(void)
	{
		cpuflags_t flags;
		IRQ_SAVE_DISABLE(flags);

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

		IRQ_RESTORE(flags);
	}

	INLINE hptime_t timer_hw_hpread(void)
	{
		return TCNT0;
	}

#elif (CONFIG_TIMER == TIMER_ON_OVERFLOW1)

	static void timer_hw_init(void)
	{
		cpuflags_t flags;
		IRQ_SAVE_DISABLE(flags);

		/* Reset Timer overflow flag */
		TIFR |= BV(TOV1);

		/* Fast PWM mode, 9 bit, 24 kHz, no prescaling. */
		#if (TIMER_PRESCALER == 1) && (TIMER_HW_BITS == 9)
			TCCR1A |= BV(WGM11);
			TCCR1A &= ~BV(WGM10);
			TCCR1B |= BV(WGM12) | BV(CS10);
			TCCR1B &= ~(BV(WGM13) | BV(CS11) | BV(CS12));
		/* Fast PWM mode, 8 bit, 24 kHz, no prescaling. */
		#elif (TIMER_PRESCALER == 1) && (TIMER_HW_BITS == 8)
			TCCR1A |= BV(WGM10);
			TCCR1A &= ~BV(WGM11);
			TCCR1B |= BV(WGM12) | BV(CS10);
			TCCR1B &= ~(BV(WGM13) | BV(CS11) | BV(CS12));
		#else
			#error Unsupported value of TIMER_PRESCALER or TIMER_HW_BITS
		#endif

		TCNT1 = 0x00;         /* initialization of Timer/Counter */

		/* Enable timer interrupt: Timer/Counter1 Overflow */
		TIMSK |= BV(TOIE1);

		IRQ_RESTORE(flags);
	}

	INLINE hptime_t timer_hw_hpread(void)
	{
		return TCNT1;
	}

#elif (CONFIG_TIMER == TIMER_ON_OUTPUT_COMPARE2)

	static void timer_hw_init(void)
	{
		cpuflags_t flags;
		IRQ_SAVE_DISABLE(flags);

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

		IRQ_RESTORE(flags);
	}

	INLINE hptime_t timer_hw_hpread(void)
	{
		return TCNT2;
	}
#elif (CONFIG_TIMER == TIMER_ON_OVERFLOW3)

	static void timer_hw_init(void)
	{
		cpuflags_t flags;
		IRQ_SAVE_DISABLE(flags);

		/* Reset Timer overflow flag */
		TIFR |= BV(TOV3);

		/* Fast PWM mode, 9 bit, 24 kHz, no prescaling. */
		#if (TIMER_PRESCALER == 1) && (TIMER_HW_BITS == 9)
			TCCR3A |= BV(WGM31);
			TCCR3A &= ~BV(WGM30);
			TCCR3B |= BV(WGM32) | BV(CS30);
			TCCR3B &= ~(BV(WGM33) | BV(CS31) | BV(CS32));
		/* Fast PWM mode, 8 bit, 24 kHz, no prescaling. */
		#elif (TIMER_PRESCALER == 1) && (TIMER_HW_BITS == 8)
			TCCR3A |= BV(WGM30);
			TCCR3A &= ~BV(WGM31);
			TCCR3B |= BV(WGM32) | BV(CS30);
			TCCR3B &= ~(BV(WGM33) | BV(CS31) | BV(CS32));
		#else
			#error Unsupported value of TIMER_PRESCALER or TIMER_HW_BITS
		#endif

		TCNT3 = 0x00;         /* initialization of Timer/Counter */

		/* Enable timer interrupt: Timer/Counter3 Overflow */
		/* ATTENTION! TOIE3 is only on ETIMSK, not TIMSK */
		ETIMSK |= BV(TOIE3);

		IRQ_RESTORE(flags);
	}

	INLINE hptime_t timer_hw_hpread(void)
	{
		return TCNT3;
	}

#else
	#error Unimplemented value for CONFIG_TIMER
#endif /* CONFIG_TIMER */


/*!
 * \file
 * <!--
 * Copyright 2007 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 * \version $Id$
 *
 * \brief ADC hardware-specific definition
 *
 * \version $Id$
 * \author Francesco Sacchi <batt@develer.com>
 */

#include "adc_avr.h"

#include <drv/adc.h>
#include <appconfig.h>

#include <cfg/macros.h>
#include <cfg/compiler.h>

#include <avr/io.h>
#include <avr/interrupt.h>

#define ADC_AVR_AREF   0
#define ADC_AVR_AVCC   1
#define ADC_AVR_INT256 2

#if CONFIG_KERNEL
	#include <cfg/module.h>
	#include <config_kern.h>
	#include <kern/proc.h>
	#include <kern/signal.h>


	#if !CONFIG_KERN_SIGNALS
		#error Signals must be active to use ADC with kernel
	#endif

	/* Signal adc convertion end */
	#define SIG_ADC_COMPLETE SIG_SINGLE

	/* ADC waiting process */
	static struct Process *adc_process;

	/**
	 * ADC ISR.
	 * Simply signal the adc process that convertion is complete.
	 */
	ISR(ADC_vect)
	{
		sig_signal(adc_process, SIG_ADC_COMPLETE);
	}
#endif /* CONFIG_KERNEL */

/**
 * Select mux channel \a ch.
 * \todo only first 8 channels are selectable!
 */
INLINE void adc_hw_select_ch(uint8_t ch)
{
	/* Set to 0 all mux registers */
	ADMUX &= ~(BV(MUX3) | BV(MUX3) | BV(MUX2) | BV(MUX1) | BV(MUX0));

	/* Select channel, only first 8 channel modes are supported for now */
	ADMUX |= (ch & 0x07);
}


/**
 * Start an ADC convertion.
 * If a kernel is present, preempt until convertion is complete, otherwise
 * a busy wait on ADCS bit is done.
 */
INLINE uint16_t adc_hw_read(void)
{
	// Ensure another convertion is not running.
	ASSERT(!(ADCSRA & BV(ADSC)));

	// Start convertion
	ADCSRA |= BV(ADSC);

	#if CONFIG_KERNEL
		// Ensure IRQs enabled.
		ASSERT(IRQ_ENABLED());
		adc_process = proc_current();
		sig_wait(SIG_ADC_COMPLETE);
	#else
		//Wait in polling until is done
		while (ADCSRA & BV(ADSC)) ;
	#endif

	return(ADC);
}

/**
 * Init ADC hardware.
 */
INLINE void adc_hw_init(void)
{
	/*
	 * Select channel 0 as default,
	 * result right adjusted.
	 */
	ADMUX = 0;

	#if CONFIG_ADC_AVR_REF == ADC_AVR_AREF
		/* External voltage at AREF as analog ref source */
		/* None */
	#elif CONFIG_ADC_AVR_REF == ADC_AVR_AVCC
		/* AVCC as analog ref source */
		ADMUX |= BV(REFS0);
	#elif CONFIG_ADC_AVR_REF == ADC_AVR_INT256
		/* Internal 2.56V as ref source */
		ADMUX |= BV(REFS1) | BV(REFS0);
	#else
		#error Unsupported ADC ref value.
	#endif

	/* Disable Auto trigger source: ADC in Free running mode. */
	ADCSRB = 0;
	
	/* Enable ADC, disable autotrigger mode. */
	ADCSRA = BV(ADEN);

	#if CONFIG_KERNEL
		MOD_CHECK(proc);
		ADCSRA |= BV(ADIE);
	#endif

	/* Set convertion frequency */
	#if CONFIG_ADC_AVR_DIVISOR == 2
		ADCSRA |= BV(ADPS0);
	#elif CONFIG_ADC_AVR_DIVISOR == 4
		ADCSRA |= BV(ADPS1);
	#elif CONFIG_ADC_AVR_DIVISOR == 8
		ADCSRA |= BV(ADPS1) | BV(ADPS0);
	#elif CONFIG_ADC_AVR_DIVISOR == 16
		ADCSRA |= BV(ADPS2);
	#elif CONFIG_ADC_AVR_DIVISOR == 32
		ADCSRA |= BV(ADPS2) | BV(ADPS0);
	#elif CONFIG_ADC_AVR_DIVISOR == 64
		ADCSRA |= BV(ADPS2) | BV(ADPS1);
	#elif CONFIG_ADC_AVR_DIVISOR == 128
		ADCSRA |= BV(ADPS2) | BV(ADPS1) | BV(ADPS0);
	#else
		#error Unsupported ADC prescaler value.
	#endif

	/* Start a convertion to init ADC hw */
	adc_hw_read();
}

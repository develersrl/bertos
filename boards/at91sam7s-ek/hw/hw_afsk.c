/**
 * \file
 * <!--
 * This file is part of BeRTOS.
 *
 * Bertos is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * As a special exception, you may use this file as part of a free software
 * library without restriction.  Specifically, if other files instantiate
 * templates or use macros or inline functions from this file, or you compile
 * this file and link it with other files to produce an executable, this
 * file does not by itself cause the resulting executable to be covered by
 * the GNU General Public License.  This exception does not however
 * invalidate any other reasons why the executable file might be covered by
 * the GNU General Public License.
 *
 * Copyright 2006 Develer S.r.l. (http://www.develer.com/)
 * All Rights Reserved.
 * -->
 *
 * \brief AFSK modem hardware-specific definitions.
 *
 * \version $Id$
 *
 * \author Francesco Sacchi <batt@develer.com>
 */


#include <net/afsk.h>
#include <drv/pwm.h>

#include <io/arm.h>
#include <cfg/compiler.h>
#include <cfg/macros.h>
#include <cfg/module.h>


#define CONFIG_ADC_CLOCK        4800000UL
#define CONFIG_ADC_STARTUP_TIME 20
#define CONFIG_ADC_SHTIME       834

#define ADC_COMPUTED_PRESCALER    ((CPU_FREQ/(2 * CONFIG_ADC_CLOCK)) - 1)
#define ADC_COMPUTED_STARTUPTIME  (((CONFIG_ADC_STARTUP_TIME * CONFIG_ADC_CLOCK)/ 8000000UL) - 1)
#define ADC_COMPUTED_SHTIME       (((CONFIG_ADC_SHTIME * CONFIG_ADC_CLOCK)/1000000000UL) - 1)

static Afsk *afsk_ctx;
bool hw_afsk_dac_isr;


static void __attribute__((interrupt)) hw_afsk_adc_isr(void)
{
	afsk_adc_isr(afsk_ctx, ADC_LCDR - 128);

	/* Enable block writing */
	PIOA_OWER = DAC_PIN_MASK;

	if (hw_afsk_dac_isr)
		PIOA_ODSR = (afsk_dac_isr(afsk_ctx) << 15) & DAC_PIN_MASK;
	else
		/* Vdac/2 = 128 */
		PIOA_ODSR = 0x4000000;

	PIOA_OWDR = DAC_PIN_MASK;

	AIC_EOICR = 0;
}

void hw_afsk_adc_init(int ch, struct Afsk * ctx)
{
	afsk_ctx = ctx;
	afsk_ctx->adc_ch = ch;
	ADC_MR = 0;
	ADC_MR |= BV(ADC_LOWRES);

	//Apply computed prescaler value
	ADC_MR &= ~ADC_PRESCALER_MASK;
	ADC_MR |= ((ADC_COMPUTED_PRESCALER << ADC_PRESCALER_SHIFT) & ADC_PRESCALER_MASK);

	//Apply computed start up time
	ADC_MR &= ~ADC_STARTUP_MASK;
	ADC_MR |= ((ADC_COMPUTED_STARTUPTIME << ADC_STARTUP_SHIFT) & ADC_STARTUP_MASK);

	//Apply computed sample and hold time
	ADC_MR &= ~ADC_SHTIME_MASK;
	ADC_MR |= ((ADC_COMPUTED_SHTIME << ADC_SHTIME_SHIFT) & ADC_SHTIME_MASK);

	// Disable all interrupt
	ADC_IDR = 0xFFFFFFFF;

	//Register interrupt vector
	AIC_SVR(ADC_ID) = hw_afsk_adc_isr;
	AIC_SMR(ADC_ID) = AIC_SRCTYPE_INT_EDGE_TRIGGERED;
	AIC_IECR = BV(ADC_ID);

	//Enable data ready irq
	ADC_IER = BV(ADC_DRDY);

	///////
	PMC_PCER = BV(TC0_ID);
	TC_BMR = TC_NONEXC0;
	TC0_CCR = BV(TC_SWTRG) | BV(TC_CLKEN);

	TC0_CMR = BV(TC_WAVE);
	TC0_CMR |= (TC_WAVSEL_UP_RC_TRG | TC_ACPC_CLEAR_OUTPUT | TC_ACPA_SET_OUTPUT);
	TC0_RC = (CPU_FREQ / 2) / 9600;
	TC0_RA = TC0_RC / 2;
	///////

	// Auto trigger enabled on TIOA channel 0
	ADC_MR |= BV(ADC_TRGEN);

	//Disable all channels
	ADC_CHDR = ADC_CH_MASK;
	//Enable channel
	ADC_CHER = BV(ch);
}

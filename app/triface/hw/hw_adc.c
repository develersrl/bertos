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
 * Copyright 2003, 2004, 2006 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2000 Bernardo Innocenti <bernie@codewiz.org>
 *
 * -->
 *
 * \brief ADC: Analogic to Digital Converter
 *
 *
 * \version $Id$
 *
 * \author Andrea Grandi <andrea@develer.com>
 */

#include "hw_adc.h"


#include <cfg/macros.h>

#include <drv/timer.h>

#include <avr/io.h>

/** Set the active AI channel */
void adc_set_active_ain(int ai)
{
	/* If number of channels is <= 4 we use the first two MUX bits */
	STATIC_ASSERT(ADC_CHANNEL_NUM <= 4);

	ai &= BV(0) | BV(1);
	ADMUX &= ~BV(MUX0);
	ADMUX &= ~BV(MUX1);
	ADMUX |= ai;
}

/** Initialize the ADC */
void adc_init(void)
{
	/* Set analog IN as input */
	DDRF &= ~(BV(PF0) | BV(PF1) | BV(PF2) | BV(PF3));

	/* Disable pull-up */
	PORTF &= ~(BV(PF0) | BV(PF1) | BV(PF2) | BV(PF3));

	ENABLE_ADC;
	adc_set_vref_avcc();
	SET_AI_ADLAR;

	/* Set the Division Factor to 128 */
	ADCSRA |= (BV(ADPS0) | BV(ADPS1) | BV(ADPS2));
}

/** Set the initial reference */
void adc_set_vref_avcc(void)
{
	ADMUX &= ~BV(REFS1);
	ADMUX |= BV(REFS0);
}

/** Read the specified AI channel */
int adc_read_ai_channel(int channel)
{
	adc_set_active_ain(channel);
	timer_udelay(STABILIZING_AI_CHANNEL_TIME);
	START_CONVERTION;

	while(ADCSRA & BV(ADSC))
	{
		/*
		 * Intentionally empty loop.
		 * It waits the convertion to be completed by ADC
		 */
	}

	ADCSRA |= BV(ADIF);

	uint16_t val;
	val = ADCL;
	val |= (uint16_t)ADCH << 8;

	return val;
}

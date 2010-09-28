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
 * Copyright 2010 Develer S.r.l. (http://www.develer.com/)
 * All Rights Reserved.
 * -->
 *
 * \brief Some ADC utilis.
 *
 * \author Daniele Basile <asterix@develer.com>
 */

#ifndef HW_ADC_H
#define HW_ADC_H

#include <drv/adc.h>
#include <drv/clock_lm3s.h>

#include <io/cm3.h>

/*
 * Return the cpu core temperature in raw format
 */
INLINE uint16_t hw_readRawTemp(void)
{
	/* Trig the temperature sampling */
	HWREG(ADC0_BASE + ADC_O_PSSI) |= ADC_PSSI_SS3;

	/* Poll untill acquisition end */
	while (!(HWREG(ADC0_BASE + ADC_O_SSFSTAT3) & ADC_SSFSTAT3_FULL));

	return (uint16_t)HWREG(ADC0_BASE + ADC_O_SSFIFO3);
}

/*
 * Return the cpu core temperature in degrees C*100
 */
INLINE uint16_t hw_readIntTemp(void)
{
	/* Trig the temperature sampling */
	HWREG(ADC0_BASE + ADC_O_PSSI) |= ADC_PSSI_SS3;

	/* Poll untill acquisition end */
	while (!(HWREG(ADC0_BASE + ADC_O_SSFSTAT3) & ADC_SSFSTAT3_FULL));

	return (uint16_t)(14750 - ADC_RANGECONV(HWREG(ADC0_BASE + ADC_O_SSFIFO3), 0, 300) * 75);
}

INLINE void hw_initIntTemp(void)
{
	/* Enable ADC0 clock */
	SYSCTL_RCGC0_R |= SYSCTL_RCGC0_ADC0;

	/*
	 * We wait some time because the clock is istable
	 * and that could cause system hardfault
	 */
	lm3s_busyWait(10);

	/* Disable all sequence */
	HWREG(ADC0_BASE + ADC_O_ACTSS) = 0;
	/* Set trigger event to programmed (for all sequence) */
	HWREG(ADC0_BASE + ADC_O_EMUX) = 0;
	/* Enalbe read of temperature sensor */
	HWREG(ADC0_BASE + ADC_O_SSCTL3) |= ADC_SSCTL3_TS0;
	/* Enable sequence S03 (single sample on select channel) */
	HWREG(ADC0_BASE + ADC_O_ACTSS) |= ADC_ACTSS_ASEN3;
}

#endif /* HW_ADC_H */

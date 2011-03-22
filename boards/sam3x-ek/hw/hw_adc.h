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
 * Copyright 2011 Develer S.r.l. (http://www.develer.com/)
 * All Rights Reserved.
 * -->
 *
 * \brief Some ADC utilis.
 *
 * \author Daniele Basile <asterix@develer.com>
 */

#ifndef HW_ADC_H
#define HW_ADC_H

#include <io/cm3.h>


/*
 * Enable temperature sensor
 */
INLINE void hw_enableTempRead(void)
{

	ADC_ACR = BV(ADC_TSON);
}


/*
 * The VT voltage equals 0.8V at 27°C with a +/-15% accuracy.
 * The VT output voltage linearly varies with a temperature slope dVT/dT = 2.65.
 * So the formula is: x = (vol - 800) / 2.65 + 27.
 */
INLINE uint16_t hw_convertToDegree(uint16_t raw_temp)
{
	raw_temp = raw_temp * 3300 * 10 / 4096;
	return ((raw_temp - 8000) * 100 / 265 + 270);
}

#endif /* HW_ADC_H */

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
 * Copyright 2000 Bernie Innocenti <bernie@codewiz.org>
 *
 * -->ght 2006 Develer S.r.l. (http://www.develer.com/)
 *
 * \brief Macro for HW_AIN_H
 *
 *
 * \version $Id$
 *
 * \author Andrea Grandi <andrea@develer.com>
 */

#ifndef HW_ADC_H
#define HW_ADC_H

#include <avr/io.h>

#define START_CONVERTION (ADCSRA |= BV(ADSC))
#define ENABLE_ADC (ADCSRA |= BV(ADEN))
#define SET_AI_ADLAR (ADMUX &= ~BV(ADLAR))

/** Microseconds to wait before starting conversion after changing a channel */
#define STABILIZING_AI_CHANNEL_TIME 125

/** Number of AIN channels */
#define ADC_CHANNEL_NUM 4

void adc_set_active_ain(int ai);
void adc_init(void);
void adc_set_vref_avcc(void);
int adc_read_ai_channel(int channel);

#endif // HW_ADC_H


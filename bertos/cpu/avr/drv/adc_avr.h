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

#ifndef DRV_ADC_AVR_H
#define DRV_ADC_AVR_H

#include <cfg/compiler.h>

#define ADC_MUX_MAXCH 7
#define ADC_BITS      10

void adc_hw_select_ch(uint8_t ch);
uint16_t adc_hw_read(void);
void adc_hw_init(void);

#endif /* DRV_ADC_AVR_H */

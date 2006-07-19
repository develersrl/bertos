/**
 * \file
 * <!--
 * Copyright 2005 Develer S.r.l. (http://www.develer.com/)
 * This file is part of DevLib - See README.devlib for information.
 * -->
 *
 * \version $Id$
 *
 * \brief ADC driver (interface)
 *
 * \version $Id$
 * \author Francesco Sacchi <batt@develer.com>
 */

/*#*
 *#* $Log$
 *#* Revision 1.2  2006/07/19 12:56:25  bernie
 *#* Convert to new Doxygen style.
 *#*
 *#* Revision 1.1  2005/06/27 21:28:31  bernie
 *#* Import ADC driver.
 *#*
 *#*/

#ifndef DRV_ADC_H
#define DRV_ADC_H

#include <hw_adc.h>
#include <cfg/compiler.h>
#include <cfg/debug.h>

/**Type for ADC return value. */
typedef uint16_t adcread_t;

#define adc_bits() ADC_BITS

adcread_t adc_read(uint16_t ch);
void adc_init(void);
DB(extern bool adc_initialized;)
#endif /* DRV_ADC_H */

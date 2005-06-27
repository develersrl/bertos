/*!
 * \file
 * <!--
 * Copyright 2005 Develer S.r.l. (http://www.develer.com/)
 * This file is part of DevLib - See README.devlib for information.
 * -->
 *
 * \version $Id$
 *
 * \brief ADC driver (implementation)
 *
 * \version $Id$
 * \author Francesco Sacchi <batt@develer.com>
 */

/*#*
 *#* $Log$
 *#* Revision 1.1  2005/06/27 21:28:31  bernie
 *#* Import ADC driver.
 *#*
 *#*/


#include <drv/adc.h>
#include <drv/timer.h>
#include <hw_adc.h>

#include <cfg/debug.h>
#include <cfg/macros.h>
#include <cfg/compiler.h>

DB(bool adc_initialized = false;)

/*!
 * Read the ADC channel \a ch.
 */
adcread_t adc_read(uint16_t ch)
{
	ASSERT(ch <= (uint16_t)ADC_MUX_MAXCH);
	ch = MIN(ch, (uint16_t)ADC_MUX_MAXCH);

	adc_hw_select_ch(ch);

	return(adc_hw_read());
}

/*!
 * Initialize the ADC hardware.
 */
void adc_init(void)
{
	cpuflags_t flags;
	IRQ_SAVE_DISABLE(flags);

	ADC_HW_INIT;
	DB(adc_initialized = true;)
	IRQ_RESTORE(flags);
}

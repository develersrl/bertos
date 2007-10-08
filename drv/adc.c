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
 *#* Revision 1.4  2006/09/20 17:32:46  marco
 *#* Use MOD_* macros instead of DB.
 *#*
 *#* Revision 1.3  2006/09/13 18:30:07  bernie
 *#* Add a FIXME.
 *#*
 *#* Revision 1.2  2006/07/19 12:56:25  bernie
 *#* Convert to new Doxygen style.
 *#*
 *#* Revision 1.1  2005/06/27 21:28:31  bernie
 *#* Import ADC driver.
 *#*
 *#*/


#include <drv/adc.h>
#include <drv/timer.h>

// FIXME: move CPU specific part to adc_CPU.c
#include <hw_adc.h>

#include <cfg/debug.h>     // ASSERT()
#include <cfg/macros.h>    // MIN()
#include <cfg/compiler.h>
#include <cfg/module.h>

/**
 * Read the ADC channel \a ch.
 */
adcread_t adc_read(uint16_t ch)
{
	ASSERT(ch <= (uint16_t)ADC_MUX_MAXCH);
	ch = MIN(ch, (uint16_t)ADC_MUX_MAXCH);

	adc_hw_select_ch(ch);

	return(adc_hw_read());
}

MOD_DEFINE(adc)

/**
 * Initialize the ADC hardware.
 */
void adc_init(void)
{
	cpuflags_t flags;
	IRQ_SAVE_DISABLE(flags);

	ADC_HW_INIT;
	IRQ_RESTORE(flags);

	MOD_INIT(adc);
}

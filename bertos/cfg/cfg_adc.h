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
 * Copyright 2008 Develer S.r.l. (http://www.develer.com/)
 * All Rights Reserved.
 * -->
 *
 * \brief Configuration file for ADC module.
 *
 * \version $Id$
 *
 * \author Daniele Basile <asterix@develer.com>
 */

#ifndef CFG_ADC_H
#define CFG_ADC_H

/**
 * Logging level definition.
 *
 * Use 0 to log only the error messages
 * Use 1 to log the error and warning messages
 * Use 2 to log all messages
 */
#define ADC_LOG_LEVEL      2

/**
 * Set logging verbosity.
 *
 * If verbosity is zero print short log messages.
 */
#define ADC_LOG_VERBOSITY    1


/**
 * ADC timing setting parameter
 *
 * - CONFIG_ADC_CLOCK is frequency clock for ADC conversion.
 * - CONFIG_ADC_STARTUP_TIME  minimum time for startup a conversion in micro second.
 * - CONFIG_ADC_SHTIME minimum time for sample and hold in nano second.
 * \{
 */
#define CONFIG_ADC_CLOCK          4800000UL
#define CONFIG_ADC_STARTUP_TIME        20
#define CONFIG_ADC_SHTIME             834
/* \ * } */

/// ADC setting for AVR target
#define CONFIG_ADC_AVR_REF      1
#define CONFIG_ADC_AVR_DIVISOR  2

/// Enable ADS strobe.
#define CONFIG_ADC_STROBE  0

#endif /* CFG_ADC_H */


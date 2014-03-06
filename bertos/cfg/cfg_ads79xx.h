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
 *
 * -->
 *
 * \brief Configuration file for the ADS79xx module.
 *
 * \author Francesco Sacchi <batt@develer.com>
 */

#ifndef CFG_ADS79XX_H
#define CFG_ADS79XX_H

/**
 * Module logging level.
 *
 * $WIZ$ type = "enum"
 * $WIZ$ value_list = "log_level"
 */
#define ADS79XX_LOG_LEVEL      LOG_LVL_INFO

/**
 * Module logging format.
 *
 * $WIZ$ type = "enum"
 * $WIZ$ value_list = "log_format"
 */
#define ADS79XX_LOG_FORMAT     LOG_FMT_VERBOSE

/**
 * Clock Frequency for ADC conversion.
 * This frequency will be rounded down to an integer
 * submultiple of CPU_FREQ.
 *
 * $WIZ$ type = "int"
 * $WIZ$ supports = "at91"
 * $WIZ$ max = 5000000
 */
#define CONFIG_ADS79XX_SAMPLE_RATE 12000


/**
 * Clock Frequency for ADC TCP
 *
 * $WIZ$ type = "int"
 */
#define CONFIG_ADCTCP_SAMPLE_RATE 3000

/**
 * Adc channels
 *
 * $WIZ$ type = "int"
 */
#define CONFIG_ADS79XX_CHANNELS 6

/**
 * Adc pad bits
 *
 * $WIZ$ type = "int"
 */
#define CONFIG_ADS79XX_PAD 4


#endif /* CFG_ADS79XX_H */

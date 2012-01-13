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
 *
 * -->
 *
 * \brief Configuration file for the WM8731 module.
 *
 * \author Daniele Basile <asterix@develer.com>
 */

#ifndef CFG_WM8731_H
#define CFG_WM8731_H

/**
 * Module logging level.
 * $WIZ$ type = "enum"
 * $WIZ$ value_list = "log_level"
 */
#define WM8731_LOG_LEVEL      LOG_LVL_INFO

/**
 * Module logging format.
 * $WIZ$ type = "enum"
 * $WIZ$ value_list = "log_format"
 */
#define WM8731_LOG_FORMAT     LOG_FMT_TERSE


/**
 * Digital control: ADC High pass filter
 * $WIZ$ type = "boolean"
 */
#define CONFIG_WM8731_DAPC     0


/**
 * Digital control: De-emphasis control
 * $WIZ$ type = "enum"
 * $WIZ$ value_list = "wm8731_deemp"
 */
#define CONFIG_WM8731_DEEMP     WM8731_DEEMP_DISABLE

/**
 * Digital control: DAC soft mute control
 * $WIZ$ type = "boolean"
 */
#define CONFIG_WM8731_DACMU     0


/**
 * Analog control: Microphone boost
 * $WIZ$ type = "boolean"
 */
#define CONFIG_WM8731_MICBOOST     0

/**
 * Analog control: Microphone/Line Input select to ADC
 * $WIZ$ type = "enum"
 * $WIZ$ value_list = "wm8731_insel"
 */
#define CONFIG_WM8731_INSEL     WM8731_INSEL_MIC


/**
 * Analog control: out selector
 * $WIZ$ type = "enum"
 * $WIZ$ value_list = "wm8731_bypass"
 */
#define CONFIG_WM8731_BYPASS     WM8731_DACSEL

/**
 * Analog control: Side tone attenuation
 * $WIZ$ type = "enum"
 * $WIZ$ value_list = "wm8731_sideatt"
 */
#define CONFIG_WM8731_SIDEATT     WM8731_SIDEATT_6dB


/**
 * Digital Audio interface format.
 * $WIZ$ type = "enum"
 * $WIZ$ value_list = "wm8731_fmt"
 */
#define CONFIG_WM8731_INTERFACE_FORMAT    WM8731_FORMAT_I2S

/**
 * Digital Audio data bit length.
 * $WIZ$ type = "enum"
 * $WIZ$ value_list = "wm8731_databit"
 */
#define CONFIG_WM8731_IWL_BITS            WM8731_IWL_16_BIT

/**
 * Enable Master mode.
 * $WIZ$ type = "boolean"
 */
#define CONFIG_WM8731_MS     0


/**
 * Data sampling rate, refer to datasheet for this value.
 * $WIZ$ type = "hex"
 */
#define CONFIG_WM8731_SAMPLING_RATES     0x6

#endif /* CFG_WM8731_H */

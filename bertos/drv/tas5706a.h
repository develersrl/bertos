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
 * Copyright 2009 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief TAS5706A Power DAC i2c driver.
 *
 *
 * \version $Id$
 * \author Francesco Sacchi <batt@develer.com>
 *
 * $WIZ$ module_name = "tas5706a"
 * $WIZ$ module_depends = "timer", "i2c"
 * $WIZ$ module_configuration = "bertos/cfg/cfg_tas5706a.h"
 * $WIZ$ module_hw = "bertos/hw/hw_tas5706a.h"
 */

#ifndef DRV_TAS5706A_H
#define DRV_TAS5706A_H

#include <cfg/compiler.h>

typedef enum Tas5706aCh
{
	TAS_CH1,
	TAS_CH2,
	TAS_CNT,
} Tas5706aCh;

/**
 * TAS minimum volume (%).
 */
#define TAS_VOL_MIN 0

/**
 * TAS maximum volume (%).
 */
#define TAS_VOL_MAX 100

typedef uint8_t tas5706a_vol_t;

/**
 * Set the volume for the specified channel.
 *
 * The volume must be expressed in % and will be at maximum CONFIG_TAS_MAX_VOL.
 *
 * \param ch The channel to be controlled.
 * \param vol The volume you want to set.
 */
void tas5706a_setVolume(Tas5706aCh ch, tas5706a_vol_t vol);

/**
 * Initialize the TAS chip.
 */
void tas5706a_init(void);

/**
 * Set TAS chip to low power mode.
 *
 * When in low power mode, the TAS will not play any sound. You should put the TAS chip in low
 * power whenever possible to prevent overheating and to save power.
 *
 * \param val True if you want to enable low power mode, false otherwise.
 */
void tas5706a_setLowPower(bool val);

#endif /* DRV_TAS5706A_H */

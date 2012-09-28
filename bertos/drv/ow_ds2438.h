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
 *  Copyright (C) 2012 Robin Gilks
 * -->
 *
 * \addtogroup ow_driver 1-wire driver
 * \ingroup drivers
 * \{
 *
 * \brief Driver for Dallas battery monitor 1-wire device
 *
 * \author Robin Gilks <g8ecj@gilks.org>
 * $WIZ$ module_name = "ow_ds2438"
 * $WIZ$ module_depends = "1wire", "crc8"
 */

#ifndef OW_DS2438_H_
#define OW_DS2438_H_


#include <stdint.h>

#include "drv/ow_1wire.h"


#define SBATTERY_FAM  0x26

#define CONF2438_IAD  0x01
#define CONF2438_CA   0x02
#define CONF2438_EE   0x04
#define CONF2438_AD   0x08
#define CONF2438_TB   0x10
#define CONF2438_NVB  0x20
#define CONF2438_ADB  0x40
#define CONF2438_ALL  0x7f


	/**
	 * \defgroup battery_api DS2438 API
	 * With this driver you can read the parameters of a DS2438 battery monitor chip.
	 * You can initialise the module to sync the charge value to that with the actual battery.
	 *
	 * API usage example:
	 * \code
	 * static CTX2438_t BatteryCtx;   // declare a context structure
	 * ow_ds2438_init (ids[battid], &BatteryCtx, RSHUNT, CHARGE);  set the current charge state of the battery and the value of the shunt resistor
	 * ow_ds2438_doconvert (ids[battid]);          // do the current and temperature conversions
	 * if (!ow_ds2438_readall (ids[battid], &BatteryCtx))
	 * BatteryCtx.Temp BatteryCtx.Volts BatteryCtx.Amps BatteryCtx.Charge etc contain the values requested
	 * \endcode
	 * \{
	 */

/**
 * Results from reading everything from a DS2438 chip, also tracks charge accumulator on a particular chip
 */

typedef struct 
{
	int16_t Temp;           ///< Temperature
	uint16_t Volts;         ///< Voltage on Vdd or Vad pin
	int16_t Amps;           ///< Current, depends on extrenal shunt to interpret the actual value
	uint16_t Charge;        ///< Charge value in amp-hrs
	uint16_t CCA;           ///< Charging Current Accumulator
	uint16_t DCA;           ///< Discharge Current Accumulator
// internal variables
	uint16_t fullICA;       ///< internal representation of value read expanded to 16 bits
	uint8_t lastICA;        ///< last value read from the chip
	float shunt;            ///< value of the shunt resistor used to measure current and charge
} CTX2438_t;

int ow_ds2438_init(uint8_t id[], CTX2438_t * context, float shunt, uint16_t charge);
int ow_ds2438_setup(uint8_t id[], int config);
void ow_ds2438_doconvert(uint8_t id[]);
int ow_ds2438_readall(uint8_t id[], CTX2438_t * context);
int ow_ds2438_calibrate(uint8_t id[], CTX2438_t * context, int16_t offset);
int ow_ds2438_setCCADCA(uint8_t id[], CTX2438_t * context);
	/** \} */ //defgroup battery_api

/** \} */ //addtogroup ow_driver

#endif

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
 *
 * \brief Driver for Dallas 1-wire temperature sensors
 *
 * \author Robin Gilks <g8ecj@gilks.org>
 *
 * $WIZ$ module_name = "ow_ds18x20"
 * $WIZ$ module_depends = "1wire", "crc8"
 */



#ifndef OW_DS18X20_H_
#define OW_DS18X20_H_

#include <stdint.h>
#include <stdbool.h>


/* DS18X20 specific values (see datasheet) */
#define DS18S20_FAMILY_CODE       0x10
#define DS18B20_FAMILY_CODE       0x28
#define DS1822_FAMILY_CODE        0x22

	/**
	 * \defgroup temp_api DS18x20 API
	 * With this driver you can read the temperature and set the resolution.
	 * No initialisation is required.
	 *
	 * API usage example:
	 * \code
	 * ow_ds18x20_resolution(ids[thermid], bits);         // set resolution to 9-12 bits
	 * ow_ds18X20_start (ids[thermid], false);            // start the conversion process
	 * while (ow_busy ());                                // wait for the conversion to complete
	 * ow_ds18X20_read_temperature (ids[thermid], &temperature);       // read the result
	 * \endcode
	 * \{
	 */
 
bool ow_ds18X20_start(uint8_t id[], bool parasitic);
bool ow_ds18x20_resolution(uint8_t id[], uint8_t bits);
bool ow_ds18X20_read_temperature(uint8_t id[], int16_t * temperature);

	/** \} */ //defgroup temp_api

/** \} */ //addtogroup ow_driver

#endif

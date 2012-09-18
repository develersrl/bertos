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
 * \brief Driver for Dallas DS18X20 series temperature sensors
 *
 * \author Robin Gilks <g8ecj@gilks.org>
 * $WIZ$ module_depends = "1wire"
 */

#include "ow_ds18x20.h"

#include "drv/ow_1wire.h"


#include <algo/crc8.h>
#include <stdint.h>
#include <stdbool.h>



/** Start measurement (CONVERT_T) for all sensors if input id is NULL 
 *   or for single sensor where id is the rom-code
 *
 * \param parasitic true if parasitic power being used
 * \param id The address of the device.
 * \return true if OK, false if error
 */
bool
ow_ds18X20_start(uint8_t id[], bool parasitic)
{
	bool ret = false;

	ow_reset();

	if (!ow_busy())
	{
		// only send if bus is "idle" = high
		if (parasitic)
			ow_command_with_parasite_enable(0x44, id);
		else
			ow_command(0x44, id);

		ret = true;
	}

	return ret;
}

/**
* Set resolution for those devices that support it
*
* \param bits Number of bits resolution being 9,10,11 or 12.
* \param id The address of the device.
* \return true if OK, false if error
*/
bool
ow_ds18x20_resolution(uint8_t id[], uint8_t bits)
{
	bool ret = true;

	switch (id[0])
	{
	case DS18S20_FAMILY_CODE:
		// fixed at 9 bits
		break;
	case DS18B20_FAMILY_CODE:
	case DS1822_FAMILY_CODE:
		// these are the same, just difference accuracy
		if ((bits < 9) || (bits > 12))
		{
			ret = false;
			break;
		}
		// write bytes 2, 3 & 4 of the scratchpad memory
		ow_command(0x4e, id);
		// dummy alarm high
		ow_byte_wr(0xff);
		// dummy alarm low
		ow_byte_wr(0xff);
		// configuration register
		ow_byte_wr(0x1f | ((bits - 9) << 5));
		break;
	}
	return ret;
}


/**
* Read the scratchpad memory in the device
*
* \param sp byte array to hold 9 bytes of data read
* \param id The address of the device.
* \return true if OK, false if error
*/
static bool
read_scratchpad(uint8_t id[], uint8_t sp[])
{
	uint8_t i;

	ow_command(0xbe, id);
	for (i = 0; i < 9; i++)
	{
		sp[i] = ow_byte_rd();
	}
	if (crc8(&sp[0], 9) != 0)
		return false;
	else
		return true;
}


/**
* Read the temperature from the device, normalised between device types and 
* the resolution set in them.
* Scale the output to 2 decimal places i.e. 12345 = 123.45 degrees
* Maintain the sign bit for the full range -55.00 to +125.00
*
* \param temperature pointer to int to hold temperature
* \param id The address of the device.
* \return true if OK, false if error
*/
bool
ow_ds18X20_read_temperature(uint8_t id[], int16_t * temperature)
{
	uint8_t sp[10];
	uint8_t bits;
	bool ret;
	uint32_t tmp, mask;
	uint8_t shift = 0;

	if ((ret = read_scratchpad(id, sp)))
	{
		tmp = sp[0] | (sp[1] << 8);
		switch (id[0])
		{
		case DS18B20_FAMILY_CODE:
		case DS1822_FAMILY_CODE:
			// read the config register to get resolution
			// get the number of bits 9-12 to 0-3
			bits = ((sp[4] >> 5) & 3);
			// convert to 0x..f8, 0x..fc, 0x..fe, 0x..ff
			mask = ~((1 << (3 - bits)) - 1);
			// mask out 0 to 3 LS bits according to resolution
			tmp &= mask;
			shift = 4;
			break;
		case DS18S20_FAMILY_CODE:
			shift = 1;
			break;
		}
		// Scale up by 100 to keep the following arithmetic as integer
		tmp = tmp * 100;

		// now for a bit of magic!
		// There are 8 bits to 1 deg but the b20 & 22 have up to 12 bits so shift by 4
		// whereas the x20 has 9 bits so shift by 1
		*temperature = tmp >> shift;
	}
	return ret;
}

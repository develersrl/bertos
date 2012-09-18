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
 *
 * \brief Driver for Dallas dual GPIO 1-wire device
 *
 * \author Robin Gilks <g8ecj@gilks.org>
 */

#include "ow_ds2413.h"

#include "drv/ow_1wire.h"

#include <stdbool.h>


/** This routine reads the PIO bits
 *
 * \param id The serial number of the switch that is to be turned on or off
 * \return true(1):    If set is successful
 *         false(0):   If set is not successful
*/
uint8_t
ow_ds2413_read(uint8_t id[])
{

	// PIO Access Read Command
	ow_command(0xf5, id);
	// Used to read the PIO Status Bit Assignment
	return ow_byte_wr(0xFF);

}



/** This routine writes the PIO bits
 *
 * \param id The serial number of the switch that is to be turned on or off
 * \param state The PIO Status Bit Assignment. (2 LSBs)
 * \return true(1):    If set is successful
 *         false(0):   If set is not successful
*/
uint8_t
ow_ds2413_write(uint8_t id[], uint8_t state)
{
	uint8_t buff[5];
	uint8_t wrByte;

	wrByte = state | 0xFC;

	// PIO Access Write Command
	ow_command(0x5a, id);
	// Channel write information
	buff[1] = ow_byte_wr(wrByte);
	// Inverted write byte
	buff[2] = ow_byte_wr(~wrByte);
	// Used to read the Status Bits
	buff[3] = ow_byte_wr(0xFF);
	// Used to read the PIO State
	buff[4] = ow_byte_wr(0xFF);

	if (buff[3] != 0xAA)
	{
		return false;
	}

	return true;
}

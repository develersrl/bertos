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
 *  Copyright (C) 2012 Robin Gilks, Peter Dannegger, Martin Thomas
 * -->
 *
 * \brief Bit bang driver for Dallas 1-wire chips
 *
 * \author Peter Dannegger (danni(at)specs.de)
 * \author Martin Thomas (mthomas(at)rhrk.uni-kl.de)
 * \author Robin Gilks <g8ecj@gilks.org>
 *
 */

#include "ow_1wire.h"

// this pulls in the low level implementation from the hardware directory
#include "hw/hw_1wire.h"


/**
 * Reset the bus, disable parasitic mode
 *
 * \return non zero = error code
 */
uint8_t
ow_reset(void)
{
	return ow_reset_intern();
}


/**
* Check for 1-wire bus being busy
*
* \return true if busy, false if not
*/
bool
ow_busy(void)
{
	return ow_bit_io(1) ? false : true;
}

/**
 * Function to output a bit
 *
 * \param b bit to output
 * \return bit read from I/O
 */
uint8_t
ow_bit_io(uint8_t b)
{
	return ow_bit_io_intern(b & 1, 0);
}

/**
 * Function to output a byte
 *
 * \param b byte to output
 * \return byte read from I/O
 */
uint8_t
ow_byte_wr(uint8_t b)
{
	uint8_t i = 8, j;

	do
	{
		j = ow_bit_io(b & 1);
		b >>= 1;
		if (j)
		{
			b |= 0x80;
		}
	}
	while (--i);

	return b;
}

/**
 * Function to output a byte in parasitic mode
 *
 * \param b byte to output
 * \return byte read from I/O
 */
uint8_t
ow_byte_wr_with_parasite_enable(uint8_t b)
{
	uint8_t i = 8, j;

	do
	{
		if (i != 1)
		{
			j = ow_bit_io_intern(b & 1, 0);
		}
		else
		{
			j = ow_bit_io_intern(b & 1, 1);
		}
		b >>= 1;
		if (j)
		{
			b |= 0x80;
		}
	}
	while (--i);

	return b;
}


/**
 * Function to read a byte
 *
 * \return byte read from I/O
 */
uint8_t
ow_byte_rd(void)
{
	// read by sending only "1"s, so bus gets released
	// after the init low-pulse in every slot
	return ow_byte_wr(0xFF);
}


/**
 * Function to implement ROM search algorithm
 *
 * \param diff Where to start in the search path
 * \param id pointer to 8 octet id buffer
 * \return next search path dirction
 */
uint8_t
ow_rom_search(uint8_t diff, uint8_t * id)
{
	uint8_t i, j, next_diff;
	uint8_t b;

	if (ow_reset())
	{
		// error, no device found <--- early exit!
		return OW_PRESENCE_ERR;
	}

	ow_byte_wr(OW_SEARCH_ROM);
	// unchanged on last device
	next_diff = OW_LAST_DEVICE;

	// number of bits in romcode
	i = OW_ROMCODE_SIZE * 8;

	do
	{
		// 8 bits
		j = 8;
		do
		{
			// read bit
			b = ow_bit_io(1);
			if (ow_bit_io(1))
			{
				// read complement bit
				if (b)
				{
					// bit pattern 0b11 - data error <--- early exit!
					return OW_DATA_ERR;
				}
			}
			else
			{
				// 0b00 = 2 devices
				if (!b)
				{
					if (diff > i || ((*id & 1) && diff != i))
					{
						b = 1;
						next_diff = i;
					}
				}
			}
			// write bit
			ow_bit_io(b);
			*id >>= 1;
			if (b)
			{
				// store bit
				*id |= 0x80;
			}

			i--;

		}
		while (--j);

		// next byte of ID
		id++;

	}
	while (i);

	// to continue search
	return next_diff;
}


/**
 * Internal function to output a command to a device
 *
 * \param command byte to output to device
 * \param id pointer to 8 octet id buffer of device to send to
 * \param with_parasite_enable flag to indicate whether parasitic mode to be used
 */
static void
ow_command_intern(uint8_t command, uint8_t * id, uint8_t with_parasite_enable)
{
	uint8_t i;

	ow_reset();

	if (id)
	{
		// to a single device
		ow_byte_wr(OW_MATCH_ROM);
		i = OW_ROMCODE_SIZE;
		do
		{
			ow_byte_wr(*id);
			id++;
		}
		while (--i);
	}
	else
	{
		// to all devices
		ow_byte_wr(OW_SKIP_ROM);
	}

	if (with_parasite_enable)
	{
		ow_byte_wr_with_parasite_enable(command);
	}
	else
	{
		ow_byte_wr(command);
	}
}

/**
 * Function to output a command to a device
 *
 * \param command byte to output to device
 * \param id pointer to 8 octet id buffer of device to send to
 */
void
ow_command(uint8_t command, uint8_t * id)
{
	ow_command_intern(command, id, 0);
}

/**
 * Function to output a command to a device in parasitic mode
 *
 * \param command byte to output to device
 * \param id pointer to 8 octet id buffer of device to send to
 */
void
ow_command_with_parasite_enable(uint8_t command, uint8_t * id)
{
	ow_command_intern(command, id, 1);
}

/**
 * Function to output a block of data to a device
 * Device must have been previously selected with a 'command'
 *
 * \param buff buffer to output to device
 * \param len length of data to send
 */
void
ow_block(uint8_t * buff, uint8_t len)
{
	uint8_t i;

	for (i = 0; i < len; i++)
		buff[i] = ow_byte_wr(buff[i]);

}

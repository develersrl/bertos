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
 * \defgroup ow_driver 1-wire driver
 * \ingroup drivers
 * \{
 *
 * \brief Bit bang driver interface for Dallas 1-wire chips
 *
 * \author Peter Dannegger (danni(at)specs.de)
 * \author Martin Thomas (mthomas(at)rhrk.uni-kl.de)
 * \author Robin Gilks <g8ecj@gilks.org>
 *
 * $WIZ$ module_name = "1wire"
 * $WIZ$ module_hw = "bertos/hw/hw_1wire.h"
 * $WIZ$ module_configuration = "bertos/cfg/cfg_1wire.h"
 */

#ifndef OW_1WIRE_H_
#define OW_1WIRE_H_

#include <stdint.h>
#include <stdbool.h>

#include "cfg/cfg_1wire.h"


#define OW_MATCH_ROM    0x55     ///< command to match ROM address
#define OW_SKIP_ROM     0xCC     ///< command to skip trying to match ROM address
#define OW_SEARCH_ROM   0xF0     ///< initiate a search for devices

#define OW_SEARCH_FIRST 0xFF     ///< start of new search
#define OW_PRESENCE_ERR 0xFF     ///< search failed with presence error
#define OW_DATA_ERR     0xFE     ///< search failed with data error
#define OW_LAST_DEVICE  0x00     ///< last device found - search effectively done!

#define OW_ROMCODE_SIZE 8        ///< rom-code (serial number) size including Family & CRC

	/**
	 * \defgroup 1wire_api 1-wire API
	 * This is the interface to the low level bit-bang driver
	 * Initialisation is only required if more that 1 I/O line is being used for multiple busses
	 *
	 * It is expected that the only interface to this code is to search for devices and to define the bus
	 * if multiple busses are in use.
	 *
	 * API usage example:
	 * \code
	 * uint8_t ids[4][OW_ROMCODE_SIZE];      // only expect to find 3 actually!!
	 *
	 * for (diff = OW_SEARCH_FIRST, cnt = 0; diff != OW_LAST_DEVICE; cnt++)
	 * {
	 * 	diff = ow_rom_search (diff, ids[cnt]);
	 *
	 * 	if ((diff == OW_PRESENCE_ERR) || (diff == OW_DATA_ERR))
	 * 		break;	// <--- early exit!
	 * 	if (crc8 (ids[cnt], 8))
	 * 		kfile_print(&ser.fd, "CRC suspect\r\n");
	 *
	 * 	switch (ids[cnt][0])
	 * 	{
	 * 	case SBATTERY_FAM:
	 * 	// note which device is of this type
	 * 		break;
	 * 	case DS18S20_FAMILY_CODE:
	 * 	case DS18B20_FAMILY_CODE:
	 * 	case DS1822_FAMILY_CODE:
	 * 		break;
	 * 	}
	 * }
	 * \endcode
	 * \{
	 */

uint8_t ow_reset(void);
bool ow_busy(void);

uint8_t ow_bit_io(uint8_t b);
uint8_t ow_byte_wr(uint8_t b);
uint8_t ow_byte_rd(void);

uint8_t ow_rom_search(uint8_t diff, uint8_t * id);

void ow_command(uint8_t command, uint8_t * id);
void ow_command_with_parasite_enable(uint8_t command, uint8_t * id);
uint8_t ow_byte_wr_with_parasite_enable(uint8_t b);

void ow_block(uint8_t * buff, uint8_t len);


void ow_set_bus(volatile uint8_t * in, volatile uint8_t * out, volatile uint8_t * ddr, uint8_t pin);

	/** \} */ //defgroup 1wire_api

/** \} */ //defgroup ow_driver

/**
 * \name Data direction register
 *
 * $WIZ$ onewire_ddr_list = "DDRB", "DDRD"
 */

/**
 * \name One-wire I/O pin.
 *
 * $WIZ$ onewire_pin_list = "PD0", "PD1", "PD2", "PD3", "PD4", "PD5", "PD6", "PD7", "PB0", "PB1", "PB2", "PB3", "PB4", "PB5"
 */

/**
 * \name One-wire input port
 *
 * $WIZ$ onewire_portin_list = "PINB", "PIND"
 */

/**
 * \name One-wire output port
 *
 * $WIZ$ onewire_portout_list = "PORTB", "PORTD"
 */

#endif /* OW_1WIRE_H_ */

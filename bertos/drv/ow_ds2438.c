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
 *  Copyright (C) 2011 Robin Gilks
 *
 * \brief Driver for Dallas battery monitor  1-wire device
 *
 * \author Robin Gilks <g8ecj@gilks.org>
 */

#include "ow_ds2438.h"

#include "drv/ow_1wire.h"

#include <algo/crc8.h>
#include <drv/timer.h>
#include <stdbool.h>


/**
 * Reads a page of EEPROM/SRAM via the scratchpad
 * \param id       the serial number for the part that the read is to be done on.
 * \param page_num page number to read fromn the 2438
 * \param rec_buf  8 byte (minimum) buffer for the page contents
 *
 * \return true if all OK else return false
 */

static int
ReadPage(uint8_t id[], int page_num, uint8_t * rec_buf)
{
	uint8_t read_block[10];
	int cnt = 0;
	uint8_t crc;

	// Recall page command
	ow_command(0xB8, id);
	// Page to Recall
	ow_byte_wr(page_num);

	// Read scratchpad command
	ow_command(0xBE, id);
	// Page to read
	ow_byte_wr(page_num);

	for (cnt = 0; cnt < 9; cnt++)
		read_block[cnt] = ow_byte_rd();

	crc = crc8(read_block, 9);
	if (crc != 0x00)
		return false;

	for (cnt = 0; cnt < 8; cnt++)
		rec_buf[cnt] = read_block[cnt];

	return true;
}

/**
 * Writes a page of EEPROM/SRAM via the scratchpad
 *          assumes the serial num has already been set
 * \param id       the serial number for the part that the read is to be done on.
 * \param page_num page number to write to the 2438
 * \param tx_buf   buffer of the page contents
 * \param tx_len   length of data to be written (need not be a full page)
 *
 * \return true if all OK else return false
 */

static int
WritePage(uint8_t id[], int page_num, uint8_t * tx_buf, int tx_len)
{
	uint8_t busybyte;

	// Write scratchpad command
	ow_command(0x4E, id);

	// Write page
	ow_byte_wr(page_num);
	ow_block(tx_buf, tx_len);

	// Copy scratchpad command
	ow_command(0x48, id);

	// Copy page
	ow_byte_wr(page_num);
	busybyte = ow_byte_rd();

	while (busybyte == 0)
		busybyte = ow_byte_rd();

	return true;
}


/**
 * Sets the DS2438 to trigger all readable values
 *
 */

void
ow_ds2438_startall(uint8_t id[])
{
	uint8_t busybyte;

	// start temperature conversion
	ow_command(0x44, id);
	busybyte = ow_byte_rd();
	while (busybyte == 0)
		busybyte = ow_byte_rd();

	// start voltage conversion
	ow_command(0xB4, id);
	busybyte = ow_byte_rd();
	while (busybyte == 0)
		busybyte = ow_byte_rd();

}

/**
 * Sets the DS2438 to read all values
 *
 * \param id       the serial number for the part that the read is to be done on.
 * \param result  pointer to a structure of results
 * \param shunt    the value of the shunt resistor being used to measure current
 *
 * \return 'true' if the read was complete
 */

int
ow_ds2438_readall(uint8_t id[], Result_t * result, float shunt)
{
	uint8_t page_data[10];
	int16_t t;
	int32_t lt;

	if (!ReadPage(id, 0, page_data))
		return false;

	t = (page_data[2] << 8) | page_data[1];
	// Scale up by 100 to keep the following arithmetic as integer. Note we have to go to 32 bits here!
	lt = t * 100L;
	// now for a bit of magic!
	// There are 8 bits to 1 deg but the 38 has 12 bits but the 3 LSBs are not used 
	// so shift by 8 to scale the value and get the resolution to the correct place
	result->Temp = lt >> 8;

	// voltage can only be positive!! 10mV resolution gives us 2 decimal places
	result->Volts = ((page_data[4] << 8) | page_data[3]);

	// This is the voltage across the shunt resistor in units of 0.2441mV
	t = page_data[6] << 8 | page_data[5];
	// We get passed the shunt resistor value as we're the only ones who know what the algorithm is
	// for calculating amps from the measured value.
	result->Amps = (int16_t) ((float) t / (4096 * shunt) * 100);

	if (!ReadPage(id, 1, page_data))
		return false;
	result->ICA = page_data[4];
	result->Charge = (uint16_t) ((float) (result->ICA + 0.5) / (float) (2048.0 * shunt));	// beware of rounding errors here!!

	if (!ReadPage(id, 7, page_data))
		return false;
	result->CCA = ((page_data[5] << 8) | page_data[4]) / (64.0 * shunt);
	result->DCA = ((page_data[7] << 8) | page_data[6]) / (64.0 * shunt);

	return true;
}

/**
 * Sets the DS2438 configuration register
 *
 * \param id       the serial number for the part that the read is to be done on.
 * \param config   value to write to the config register to set the mode
 *
 * \return 'true' if the read was complete
 */
int
ow_ds2438_setup(uint8_t id[], int config)
{
	uint8_t send_block[10];

	// Get the Status/Configuration page
	if (!ReadPage(id, 0, send_block))
		return false;

	// if there is no change required in the config register, then leave it alone
	if ((send_block[0] & CONF2438_ALL) == config)
		return true;

	// set config byte
	send_block[0] = config;
	if (WritePage(id, 0, send_block, 1))
		return true;

	return false;
}

/**
 * Sets the offset register and clears the threshold
 * \param id       the serial number for the part that the read is to be done on.
 * \param offset   does automatic if zero else sets value blindly
 *
 * \return true if all OK else return false
 */

int
ow_ds2438_calibrate(uint8_t id[], int offset)
{

	uint8_t rec_block[20];
	int i;


	// turn off current sensing
	ow_ds2438_setup(id, 0);
	// wait for the ADC to stop
	timer_delay(40);

	// Get offset register from page 1
	if (!ReadPage(id, 1, rec_block))
		return false;

	// clear the offset reg
	rec_block[5] = 0;
	rec_block[6] = 0;

	// Write the page back
	if (!WritePage(id, 1, rec_block, 8))
		return false;

	ow_ds2438_setup(id, CONF2438_IAD);
	// wait for the ADC to start
	timer_delay(40);
	// Get Current register - it should read zero from page 0
	if (!ReadPage(id, 0, rec_block))
		return false;

	// turn off current sensing
	ow_ds2438_setup(id, 0);
	// wait for the ADC to stop
	timer_delay(40);

	i = (rec_block[6] << 8) | rec_block[5];
	// get a clean copy of what is in page 1
	if (!ReadPage(id, 1, rec_block))
		return false;

	if (offset)
		i = offset;
	i = (0 - i) << 3;
	rec_block[5] = i & 0xff;
	rec_block[6] = i >> 8;

	// Write the page back
	if (!WritePage(id, 1, rec_block, 8))
		return false;

	return true;

}

/**
 * Sets the ICA register
 * \param id       the serial number for the part that the read is to be done on.
 * \param value    what to write to the ICA register
 *
 * \return true if all OK else return false
 */
int
ow_ds2438_setICA(uint8_t id[], uint8_t value)
{
	uint8_t page_data[10];

	// Get ICA register from page 1
	if (!ReadPage(id, 1, page_data))
		return false;

	// slip in the new value
	page_data[4] = value;

	// Write the page back
	if (!WritePage(id, 1, page_data, 8))
		return false;

	return true;

}

/**
 * Sets the CCA and DCA registers
 * \param id       the serial number for the part that the read is to be done on.
 * \param cca    what to write to the CCA register
 * \param dca    what to write to the DCA register
 *
 * \return true if all OK else return false
 */
int
ow_ds2438_setCCADCA(uint8_t id[], int cca, int dca)
{
	uint8_t page_data[10];

	// Get registers from page 7
	if (!ReadPage(id, 7, page_data))
		return false;

	// do the CCA/DCA regs
	if (cca >= 0)
	{
		page_data[4] = cca & 0xff;
		page_data[5] = cca >> 8;
	}
	if (dca >= 0)
	{
		page_data[6] = dca & 0xff;
		page_data[7] = dca >> 8;
	}

	// Write the page back
	if (!WritePage(id, 7, page_data, 8))
		return false;

	return true;
}

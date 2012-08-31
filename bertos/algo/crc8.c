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
 * please read copyright-notice at EOF
 *
 * -->
 *
 * \brief CRC8 functions
 *
 * \author Colin O'Flynn
 */

#include "crc8.h"
#include <stdint.h>


/**
 * A very useful CRC8 as used by Dallas one-wire chips
 *
 * \param data Pointer to buffer containing data 
 * \param len Length of buffer
 * \return 8 bit CRC
 *
 */

uint8_t
crc8(uint8_t * data, size_t len)
{
	uint8_t crc;
	uint16_t loop_count;
	uint8_t bit_counter;
	uint8_t b;
	uint8_t feedback_bit;

	crc = CRC8INIT;

	for (loop_count = 0; loop_count != len; loop_count++)
	{
		b = data[loop_count];

		bit_counter = 8;
		do
		{
			feedback_bit = (crc ^ b) & 0x01;

			if (feedback_bit == 0x01)
			{
				crc = crc ^ CRC8POLY;
			}
			crc = (crc >> 1) & 0x7F;
			if (feedback_bit == 0x01)
			{
				crc = crc | 0x80;
			}

			b = b >> 1;
			bit_counter--;

		}
		while (bit_counter > 0);
	}

	return crc;
}

/*
This code is from Colin O'Flynn - Copyright (c) 2002 
only minor changes by M.Thomas 9/2004

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

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
 * -->
 *
 * \brief Bitarray module
 *
 * \author Daniele Basile <asterix@develer.com>
 *
 */

#include "bitarray.h"

#include <string.h>

// De Bruijn constant coefficents.
static const uint8_t DeBruijn_coefficents[32] =
{
  0, 1, 28, 2, 29, 14, 24, 3, 30, 22, 20, 15, 25, 17, 4, 8,
  31, 27, 13, 23, 21, 19, 16, 7, 26, 12, 18, 6, 11, 5, 10, 9
};

/**
 * Return the position of the first bit non zero in bitarray
 *
 * \param bitx BitArray context
 * \param idx Starting bit
 * \param offset Number of bits to test
 * \return Position in bitarray when firt bit non zero occur,  -1 otherwise
 */
int bitarray_firstSetBit(BitArray *bitx)
{
	ASSERT(bitx);

	uint32_t *b = (uint32_t *)bitx->array;
	int pos =  -1;
	int curr = 0;
	int i = 0;
	size_t bytes = bitx->size;

	while (bytes)
	{
		if (bytes < 4)
		{
			uint32_t data = 0;
			memcpy(&data, &b[i], bytes);

			if (data == 0)
				return -1;

			return (DeBruijn_coefficents[((uint32_t)((data & -data) * 0x077CB531U)) >> 27] + curr);
		}

		if (b[i])
		{
			pos = DeBruijn_coefficents[((uint32_t)((b[i] & -b[i]) * 0x077CB531U)) >> 27] + curr;
			break;
		}

		bytes -= 4;
		curr += 32;
		i++;
	}

	return pos;
}

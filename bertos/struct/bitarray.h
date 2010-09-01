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
 * Copyright 2010 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 * \brief Bitarray module
 *
 * \author Daniele Basile <asterix@develer.com>
 *
 * $WIZ$ module_name = "bitarray"
 */

#ifndef STRUCT_BITARRAY_H
#define STRUCT_BITARRAY_H

#include <cfg/compiler.h>
#include <cfg/macros.h>
#include <cfg/debug.h>

#include <cpu/types.h>


#define ALLOC_BITARRAY(name, size)   uint8_t name[DIV_ROUNDUP((size), 8)]
#define BIT_ARRAY_SIZE(name)         (sizeof((name)) * 8)

INLINE void bitarray_set(int idx, uint8_t *bit_array, size_t len)
{
	ASSERT((size_t)idx <= len);
	int page = idx / 8;
	uint8_t bit = idx % 8;

	bit_array[page] |= BV(bit);
}

INLINE void bitarray_clear(int idx, uint8_t *bit_array, size_t len)
{
	ASSERT((size_t)idx <= len);
	int page = idx / 8;
	uint8_t bit = idx % 8;

	bit_array[page] &= ~BV(bit);
}

INLINE void bitarray_setOffset(int idx, int offset, uint8_t *bit_array, size_t len)
{
	ASSERT((size_t)idx <= len);

	for (int i = idx; i < offset + idx; i++)
		bitarray_set(i, bit_array, len);
}


INLINE void bitarray_clearOffset(int idx, int offset, uint8_t *bit_array, size_t len)
{
	ASSERT((size_t)idx <= len);

	for (int i = idx; i < offset + idx; i++)
		bitarray_clear(i, bit_array, len);
}

INLINE bool bitarray_check(int idx, uint8_t *bit_array, size_t len)
{
	ASSERT((size_t)idx <= len);
	int page = idx / 8;
	uint8_t bit = idx % 8;

	return (bit_array[page] & BV(bit));
}

INLINE bool bitarray_full(uint8_t *bit_array, size_t len)
{
	int count = len;
	for (size_t page = 0; page <= len / 8; page++)
	{
		if (count < 8)
		{
			for (size_t i = page * 8; i <= len; i++)
				if (!bitarray_check(i, bit_array, len))
					return 0;
				count--;
		}
		else
		{
			if (!(bit_array[page] == 0xff))
				return 0;
		}
		count -= 8;
	}

	return 1;
}

/*
 * Ugly!.. reformat it.
 */
INLINE bool bitarray_blockFull(int idx, int offset, uint8_t *bit_array, size_t len)
{
	ASSERT((size_t)(idx + offset) <= len);

	for (int i = idx; i <= idx + offset; i++)
		if (!bitarray_check(i, bit_array, len))
			return 0;

	return 1;
}


/*
 * Ugly!.. reformat it.
 */
INLINE bool bitarray_blockEmpty(int idx, int offset, uint8_t *bit_array, size_t len)
{
	ASSERT((size_t)(idx + offset) <= len);

	for (int i = idx; i <= idx + offset; i++)
		if (bitarray_check(i, bit_array, len))
			return 0;

	return 1;
}


INLINE void bitarray_dump(uint8_t *bit_array, size_t len)
{
	kprintf("bitarray size[%zu]bits\n", len);

	int i = 0;
	int j = 0;
	int count = len;

	while (count--)
	{
		kprintf("%d", bitarray_check(i++, bit_array, len));
		if (j == 7)
		{
			kprintf("..%02x [%d]\n", bit_array[(i / 8) - 1], i);
			j = 0;
			continue;
		}
		j++;
	}

	if (j != 0)
		kprintf("..%02x [%d]\n", bit_array[i / 8], i);
}

int bitarray_testSetup(void);
int bitarray_testRun(void);
int bitarray_testTearDown(void);

#endif /* STRUCT_BITARRAY_H */

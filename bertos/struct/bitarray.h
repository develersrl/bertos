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

typedef struct BitArray
{
	size_t size;
	size_t bitarray_len;
	uint8_t *array;
} BitArray;

#define BITARRAY_ALLOC(name, size)   uint8_t name[DIV_ROUNDUP((size), 8)]
#define BITARRAY_SIZE(name)         (sizeof((name)) * 8)

INLINE void bitarray_set(BitArray *bitx, int idx)
{
	ASSERT((size_t)idx <= bitx->bitarray_len);

	int page = idx / 8;
	uint8_t bit = idx % 8;

	bitx->array[page] |= BV(bit);
}

INLINE void bitarray_clear(BitArray *bitx, int idx)
{
	ASSERT((size_t)idx <= bitx->bitarray_len);

	int page = idx / 8;
	uint8_t bit = idx % 8;

	bitx->array[page] &= ~BV(bit);
}

INLINE void bitarray_setRange(BitArray *bitx, int idx, int offset)
{
	ASSERT((size_t)idx <= bitx->bitarray_len);

	for (int i = idx; i < offset + idx; i++)
		bitarray_set(bitx, i);
}


INLINE void bitarray_clearRange(BitArray *bitx, int idx, int offset)
{
	ASSERT((size_t)idx <= bitx->bitarray_len);

	for (int i = idx; i < offset + idx; i++)
		bitarray_clear(bitx, i);
}

INLINE bool bitarray_test(BitArray *bitx, int idx)
{
	ASSERT((size_t)idx <= bitx->bitarray_len);
	int page = idx / 8;
	uint8_t bit = idx % 8;

	return (bitx->array[page] & BV(bit));
}

/*
 * Ugly!.. reformat it.
 */
INLINE bool bitarray_isFull(BitArray *bitx)
{
	int count = bitx->size;
	for (size_t page = 0; page <= bitx->size / 8; page++)
	{
		if (count < 8)
		{
			for (size_t i = page * 8; i <= bitx->bitarray_len; i++)
				if (!bitarray_test(bitx, i))
					return 0;
				count--;
		}
		else
		{
			if (!(bitx->array[page] == 0xff))
				return 0;
		}
		count -= 8;
	}

	return 1;
}

/*
 * Ugly!.. reformat it.
 */
INLINE bool bitarray_isRangeFull(BitArray *bitx, int idx, int offset)
{
	ASSERT((size_t)(idx + offset) <= bitx->bitarray_len);

	for (int i = idx; i <= idx + offset; i++)
		if (!bitarray_test(bitx, i))
			return 0;

	return 1;
}

/*
 * Ugly!.. reformat it.
 */
INLINE bool bitarray_isRangeEmpty(BitArray *bitx, int idx, int offset)
{
	ASSERT((size_t)(idx + offset) <= bitx->bitarray_len);

	for (int i = idx; i <= idx + offset; i++)
		if (bitarray_test(bitx, i))
			return 0;

	return 1;
}


INLINE void bitarray_dump(BitArray *bitx)
{
	kprintf("bitarray size[%zu]bits on [%zu]bytes\n", bitx->bitarray_len, bitx->size);

	int i = 0;
	int j = 0;
	int count = bitx->bitarray_len;

	while (count--)
	{
		kprintf("%d", bitarray_test(bitx, i++));
		if (j == 7)
		{
			kprintf("..%02x [%d]\n", bitx->array[(i / 8) - 1], i);
			j = 0;
			continue;
		}
		j++;
	}

	if (j != 0)
		kprintf("..%02x [%d]\n", bitx->array[i / 8], i);
}

INLINE void init_bitarray(BitArray *bitx, size_t bitarray_len, uint8_t *array, size_t size)
{
	bitx->size = size;
	bitx->array = array;
	bitx->bitarray_len = bitarray_len;
}


int bitarray_testSetup(void);
int bitarray_testRun(void);
int bitarray_testTearDown(void);

#endif /* STRUCT_BITARRAY_H */

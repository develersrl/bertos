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
	uint8_t *array;

} BitArray;


#define ALLOC_BITARRAY(name, size)   uint8_t name[DIV_ROUNDUP((size),8)]


INLINE void bitarray_set(BitArray *ctx, int idx)
{
	ASSERT((size_t)idx <= ctx->size);
	int page = idx / 8;
	int bit = idx % 8;

	ctx->array[page] |= BV(bit);
}

INLINE void bitarray_clear(BitArray *ctx, int idx)
{
	ASSERT((size_t)idx <= ctx->size);
	int page = idx / 8;
	int bit = idx % 8;

	ctx->array[page] &= ~BV(bit);
}

INLINE bool bitarray_check(BitArray *ctx, int idx)
{
	ASSERT((size_t)idx <= ctx->size);
	int page = idx / 8;
	int bit = idx % 8;

	return (ctx->array[page] & BV(bit));
}

INLINE void init_bitarray(BitArray *ctx, uint8_t *array, size_t size)
{
	ctx->size = size;
	ctx->array = array;
}


int bitarray_testSetup(void);
int bitarray_testRun(void);
int bitarray_testTearDown(void);

#endif /* STRUCT_BITARRAY_H */

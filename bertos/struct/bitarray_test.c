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
 *
 * -->
 *
 * \brief Bitarray test
 *
 * \author Daniele Basile <asterix@develer.com>
 */

#include <struct/bitarray.h>

#include <cfg/compiler.h>
#include <cfg/test.h>
#include <cfg/debug.h>

#include <string.h>

ALLOC_BITARRAY(test1, 31);
BitArray ctx;

int bitarray_testSetup(void)
{
	kdbg_init();
	init_bitarray(&ctx, test1, sizeof(test1));
	return 0;
}

int bitarray_testRun(void)
{
	memset(test1, 0xaa, sizeof(test1));
	bitarray_dump(&ctx);

	for (int i = 0; i < bitarray_size(&ctx); i++)
	{
		if (!((bool)(i % 2) == bitarray_check(&ctx, i)))
		{
			kprintf("Error!\n");
			return -1;
		}
	}

	memset(test1, 0, sizeof(test1));
	for (int i = 0; i < bitarray_size(&ctx); i++)
	{
		if ((i % 2) == 0)
			bitarray_clear(&ctx, i);
		else
			bitarray_set(&ctx, i);
	}

	bitarray_dump(&ctx);
	for (int i = 0; i < bitarray_size(&ctx); i++)
	{
		if (!((bool)(i % 2) == bitarray_check(&ctx, i)))
		{
			kprintf("Error!\n");
			return -1;
		}
	}

	memset(test1, 0, sizeof(test1));
	bitarray_set(&ctx, 0);
	bitarray_dump(&ctx);
	if (!bitarray_check(&ctx, 0))
	{
		kprintf("Error!\n");
		return -1;
	}

	memset(test1, 0, sizeof(test1));
	bitarray_set(&ctx, bitarray_size(&ctx));
	bitarray_dump(&ctx);
	if (!bitarray_check(&ctx, bitarray_size(&ctx)))
	{
		kprintf("Error!\n");
		return -1;
	}

	return 0;
}

int bitarray_testTearDown(void)
{
	return 0;
}

TEST_MAIN(bitarray);

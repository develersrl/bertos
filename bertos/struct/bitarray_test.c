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

#define TEST1_LEN   31
#define TEST2_LEN   17
#define TEST3_LEN   16
#define TEST4_LEN   23
#define TEST5_LEN   72

BITARRAY_ALLOC(test1, TEST1_LEN);
BITARRAY_ALLOC(test2, TEST2_LEN);
BITARRAY_ALLOC(test3, TEST3_LEN);
BITARRAY_ALLOC(test4, TEST4_LEN);
BITARRAY_ALLOC(test5, TEST5_LEN);

BitArray bitx1;
BitArray bitx2;
BitArray bitx3;
BitArray bitx4;
BitArray bitx5;

int bitarray_testSetup(void)
{
	kdbg_init();
	bitarray_init(&bitx1, TEST1_LEN, test1, sizeof(test1));
	bitarray_init(&bitx2, TEST2_LEN, test2, sizeof(test2));
	bitarray_init(&bitx3, TEST3_LEN, test3, sizeof(test3));
	bitarray_init(&bitx4, TEST4_LEN, test4, sizeof(test4));
	bitarray_init(&bitx5, TEST5_LEN, test5, sizeof(test5));
	return 0;
}

int bitarray_testRun(void)
{
	memset(test1, 0xaa, sizeof(test1));

	bitarray_dump(&bitx1);
	for (size_t i = 0; i < TEST1_LEN; i++)
	{
		if (!((bool)(i % 2) == bitarray_test(&bitx1,i)))
			goto error;
	}

	memset(test1, 0, sizeof(test1));
	for (size_t i = 0; i < TEST1_LEN; i++)
	{
		if ((i % 2) == 0)
			bitarray_clear(&bitx1,i);
		else
			bitarray_set(&bitx1, i);
	}

	bitarray_dump(&bitx1);
	for (size_t i = 0; i < TEST1_LEN; i++)
	{
		if (!((bool)(i % 2) == bitarray_test(&bitx1, i)))
		goto error;
	}

	memset(test1, 0, sizeof(test1));
	bitarray_set(&bitx1, 0);
	bitarray_dump(&bitx1);
	if (!bitarray_test(&bitx1, 0))
		goto error;

	memset(test1, 0, sizeof(test1));
	bitarray_set(&bitx1, TEST1_LEN);
	bitarray_dump(&bitx1);
	if (!bitarray_test(&bitx1, TEST1_LEN))
		goto error;

	kprintf("Test 2\n");
	memset(test2, 0xFF, sizeof(test2));
	bitarray_dump(&bitx2);
	if (!bitarray_isFull(&bitx2))
		goto error;

	memset(test2, 0xFF, sizeof(test2));
	bitarray_clear(&bitx2, 5);
	bitarray_dump(&bitx2);
	if (bitarray_isFull(&bitx2))
		goto error;

	memset(test2, 0xFF, sizeof(test2));
	bitarray_clear(&bitx2, 13);
	bitarray_dump(&bitx2);
	if (bitarray_isFull(&bitx2))
		goto error;

	kprintf("Test 3\n");
	bitarray_set(&bitx3, 12);
	bitarray_dump(&bitx3);
	int pos = 0;
	pos = bitarray_firstSetBit(&bitx3);
	if (pos != 12)
		goto error;

	kprintf("Test 4\n");
	bitarray_set(&bitx4, TEST4_LEN);
	bitarray_dump(&bitx4);
	pos = 0;
	pos = bitarray_firstSetBit(&bitx4);
	if (pos != 23)
		goto error;

	kprintf("Test 5\n");
	bitarray_set(&bitx5, 71);
	bitarray_dump(&bitx5);
	pos = 0;
	pos = bitarray_firstSetBit(&bitx5);
	kprintf("pos %d\n", pos);
	if (pos != 71)
		goto error;

	kprintf("Test 6\n");
	bitarray_clear(&bitx5, 71);
	bitarray_set(&bitx5, 5);
	bitarray_dump(&bitx5);
	pos = 0;
	pos = bitarray_firstSetBit(&bitx5);
	if (pos != 5)
		goto error;

	return 0;

error:
	kprintf("Error!\n");
	return -1;
}

int bitarray_testTearDown(void)
{
	return 0;
}

TEST_MAIN(bitarray);

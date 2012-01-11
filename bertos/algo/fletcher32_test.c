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
 *
 * -->
 *
 * \brief Fletcher-32 checksum algorithm
 *
 * \author Francesco Sacchi <batt@develer.com>
 */

#include "fletcher32.h"

#include <cfg/test.h>
#include <cfg/debug.h>
#include <stdlib.h>
#include <string.h>


/* Wikipedia reference implementation */
static uint32_t fletcher32( uint16_t *data, size_t len )
{
        uint32_t sum1 = 0xffff, sum2 = 0xffff;

        while (len) {
                unsigned tlen = len > 360 ? 360 : len;
				//kprintf("tlen %d\n", tlen);
                len -= tlen;
                do {
                        sum1 += *data++;
                        sum2 += sum1;
						//kprintf("sum1 %d sum2 %d\n", sum1, sum2);
                } while (--tlen);
				///kprintf("b sum1 %d sum2 %d\n", sum1, sum2);
                sum1 = (sum1 & 0xffff) + (sum1 >> 16);
                sum2 = (sum2 & 0xffff) + (sum2 >> 16);
				//kprintf("a sum1 %d sum2 %d\n", sum1, sum2);
        }
        /* Second reduction step to reduce sums to 16 bits */
        sum1 = (sum1 & 0xffff) + (sum1 >> 16);
        sum2 = (sum2 & 0xffff) + (sum2 >> 16);
        return sum2 << 16 | sum1;
}

static uint32_t fletcher32_b(const void *buf, size_t len)
{
	size_t roundl = (len + 1) & 0xFFFFFFFE;
	uint16_t *b = (uint16_t *)malloc(roundl);
	ASSERT(b);
	memset(b, 0, roundl);
	memcpy(b, buf, len);
	uint32_t f2 = fletcher32(b, roundl / 2);
	free(b);
	return f2;
}



int fletcher32_testSetup(void)
{
	kdbg_init();
	return 0;
}

int fletcher32_testTearDown(void)
{
	return 0;
}

static const char *tstbufs[] =
{
	"",
	"\x01",
	"\xff",
	"\x01\xff",
	"\x01\x01",
	"1", "2", "3", "4", "5", "6", "7", "8", "9",
	"11", "22", "33", "44", "55", "66", "77", "88", "99",
	"111", "222", "333", "444", "555", "666", "777", "888", "999",
	"123",
	"asd",
	"0123456789",
	"abcdefghijklmnopqrstuvwxyz",
	"abcdefghijklmnopqrstuvwxyza",
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
	"abcdefghijklmnopqrstuvwxyz0123456789"
};


int fletcher32_testRun(void)
{
	Fletcher32 f;
	Fletcher32 ft;
	size_t tot = 0;
	fletcher32_init(&ft);
	for (unsigned t = 0; t < countof(tstbufs); t++)
	{
		fletcher32_init(&f);
		fletcher32_update(&f, tstbufs[t], strlen(tstbufs[t]));
		uint32_t f1 = fletcher32_final(&f);

		uint32_t f2 = fletcher32_b(tstbufs[t], strlen(tstbufs[t]));

		fletcher32_update(&ft, tstbufs[t], strlen(tstbufs[t]));
		tot += strlen(tstbufs[t]);

		//kdump(tstbufs[t], strlen(tstbufs[t]));
		kprintf("f1 %04lX, f2 %04lX\n", f1, f2);
		ASSERT(f1 == f2);
	}

	uint8_t *b = malloc(tot);
	uint8_t *start = b;
	ASSERT(b);
	for (unsigned t = 0; t < countof(tstbufs); t++)
	{
		memcpy(b, tstbufs[t], strlen(tstbufs[t]));
		b += strlen(tstbufs[t]);
	}
	uint32_t ft2 = fletcher32_b(start, tot);
	uint32_t ft1 = fletcher32_final(&ft);
	//kdump(start, tot);
	free(start);
	kprintf("ft1 %04lX, ft2 %04lX\n", ft1, ft2);
	ASSERT(ft1 == ft2);
	return 0;
}

TEST_MAIN(fletcher32);



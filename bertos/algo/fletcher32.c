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

#include <cfg/macros.h> //MIN()

void fletcher32_init(Fletcher32 *f)
{
	f->sum1 = 0xFFFF;
	f->sum2 = 0xFFFF;
	f->carry = -1;
}

void fletcher32_update(Fletcher32 *f, const void *_buf, size_t len)
{
	uint16_t data = 0;
	const uint8_t *buf = (const uint8_t *)_buf;
	uint8_t last = buf[len-1];

	if (f->carry != -1 && len)
	{
		data = f->carry | *buf++ << 8;
		f->carry = -1;
		++len;
	}
	else if (len > 1)
	{
		data = buf[0] | buf[1] << 8;
		buf += 2;
	}

	if (len & 1)
		f->carry = last;

	size_t l = len / 2;
	while (l)
	{
		size_t tlen = MIN(l, (size_t)360);
		l -= tlen;
		do
		{
			f->sum1 += data;
			f->sum2 += f->sum1;
			data = buf[0] | buf[1] << 8;
			buf += 2;
		}
		while (--tlen);
		f->sum1 = (f->sum1 & 0xffff) + (f->sum1 >> 16);
		f->sum2 = (f->sum2 & 0xffff) + (f->sum2 >> 16);
	}
}

uint32_t fletcher32_final(Fletcher32 *f)
{
	uint32_t sum1, sum2;
	sum1 = f->sum1;
	sum2 = f->sum2;

	if (f->carry != -1)
	{
		sum1 += f->carry;
		sum2 += sum1;
		sum1 = (sum1 & 0xffff) + (sum1 >> 16);
		sum2 = (sum2 & 0xffff) + (sum2 >> 16);
	}

	/* Second reduction step to reduce sums to 16 bits */
	sum1 = (sum1 & 0xffff) + (sum1 >> 16);
	sum2 = (sum2 & 0xffff) + (sum2 >> 16);

	return sum2 << 16 | sum1;
}

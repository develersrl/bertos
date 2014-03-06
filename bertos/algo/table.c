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
 * \brief Table conversion
 *
 * \author Francesco Sacchi <batt@develer.com>
 */

#include <cfg/compiler.h>

#include "table.h"

static size_t upper_bound(const Table *orig_table, size_t size, int x)
{
	const Table *table = orig_table;

	while (size)
	{
		size_t pos = size / 2;
		if (x < table[pos].x)
			size = pos;
		else
		{
			table += pos+1;
			size -= pos+1;
		}
	}

	return table - orig_table;
}


/**
 * Give an array of (x, y) coordinates and a x value, this function returns the
 * linear interpolation of y between the two closest x values found in the
 * table.
 *
 * \note The table must have the x coordinates monotonically increasing wrt
 *       indexes.
 *
 * \param table the array of (x, y) values.
 * \param size the number of elements in the table.
 * \param x the x value we want to find the interpolation to.
 *
 * \return if x is lesser than the minimum x coordinate in the table, the first y.
 * \return if x is greater than the maximum, the last y.
 * \return if x is between minimum and maximum, the linear interpolation for y.
 *
 */
int table_linearInterpolation(const Table *table, size_t size, int x)
{
	size_t i = upper_bound(table, size, x);
	if (i == 0)
		return table[0].y;
	if (i == size)
		return table[size - 1].y;

	int dy = table[i].y - table[i - 1].y;
	int dx = table[i].x - table[i - 1].x;

	return ((long)(x - table[i - 1].x) * dy) / dx + table[i - 1].y;
}

#if 0
#include <stdio.h>

static const Table t[] =
{
	{0, 0},
	{1023, 1023},
};

int main(void)
{
	int i;
	for (i = 0; i < 1024; i++)
	{
		printf("%d %d\n", i, table_find(t, 2, i));
	}

	printf("%d %d\n", -10, table_find(t, 2, -10));
	printf("%d %d\n", 10240, table_find(t, 2, 10240));
}

#endif

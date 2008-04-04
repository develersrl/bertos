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
 * Copyright 2004, 2005 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief sprintf() implementation based on _formatted_write()
 *
 * \version $Id$
 * \author Bernardo Innocenti <bernie@develer.com>
 */

#include "sprintf.c"
#include "formatwr.c"
#include "hex.c"
#include <cfg/compiler.h>
#include <mware/pgm.h>
#include <stdio.h>

#include <assert.h> /* assert() */
#include <string.h> /* strcmp() */


int main(UNUSED_ARG(int, argc), UNUSED_ARG(char **,argv))
{
	char buf[256];
	static const char test_string[] = "Hello, world!\n";
	static const pgm_char test_string_pgm[] = "Hello, world!\n";

	snprintf(buf, sizeof buf, "%s", test_string);
	assert(strcmp(buf, test_string) == 0);

	snprintf(buf, sizeof buf, "%S", test_string_pgm);
	assert(strcmp(buf, test_string_pgm) == 0);

#define TEST(FMT, VALUE, EXPECT) do { \
		snprintf(buf, sizeof buf, FMT, VALUE); \
		assert(strcmp(buf, EXPECT) == 0); \
	} while (0)

	TEST("%d",       12345,        "12345");
	TEST("%ld",  123456789L,   "123456789");
	TEST("%ld",  -12345678L,   "-12345678");
	TEST("%lu", 4294967295UL, "4294967295");
	TEST("%hd",     -12345,       "-12345");
	TEST("%hu",      65535U,       "65535");

	TEST("%8d",      123,       "     123");
	TEST("%8d",     -123,       "    -123");
	TEST("%-8d",     -123,      "-123    ");
	TEST("%08d",     -123,      "-0000123");

	TEST("%8.2f",  -123.456,    " -123.46");
	TEST("%-8.2f", -123.456,    "-123.46 ");
	TEST("%8.0f",  -123.456,    "    -123");

#undef TEST

	/*
	 * Stress tests.
	 */
	snprintf(buf, sizeof buf, "%s", NULL);
	assert(strcmp(buf, "<NULL>") == 0);
	snprintf(buf, sizeof buf, "%k");
	assert(strcmp(buf, "???") == 0);
	sprintf(NULL, test_string); /* must not crash */

	return 0;
}


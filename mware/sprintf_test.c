/*!
 * \file
 * <!--
 * Copyright 2004, 2005 Develer S.r.l. (http://www.develer.com/)
 * This file is part of DevLib - See README.devlib for information.
 * -->
 *
 * \brief sprintf() implementation based on _formatted_write()
 *
 * \version $Id$
 * \author Bernardo Innocenti <bernie@develer.com>
 */

/*#*
 *#* $Log$
 *#* Revision 1.5  2005/11/04 17:47:26  bernie
 *#* Fix one warning.
 *#*
 *#* Revision 1.4  2005/11/04 17:43:27  bernie
 *#* Fix for LP64 architectures; Add some more tests.
 *#*
 *#*/

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


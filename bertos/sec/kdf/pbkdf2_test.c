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
 * \brief PBKDF2 testsuite
 * \author Giovanni Bajo <rasky@develer.com>
 *
 */


#include "pbkdf2.h"

#include <cfg/test.h>
#include <cfg/debug.h>

#include <sec/mac/hmac.h>
#include <sec/hash/sha1.h>

#include <cpu/detect.h>

#include <string.h>


int PBKDF2_testSetup(void)
{
	kdbg_init();
	return 0;
}

int PBKDF2_testTearDown(void)
{
	return 0;
}

int PBKDF2_testRun(void)
{
	Kdf *kdf = PBKDF2_stackinit(hmac_stackinit(SHA1_stackinit()));

	uint8_t res[32];

	PBKDF2_set_iterations(kdf, 1);
	kdf_begin(kdf, "password", 8, (const uint8_t*)"salt", 4);
	kdf_read(kdf, res, 20);
	ASSERT(memcmp(res, "\x0c\x60\xc8\x0f\x96\x1f\x0e\x71\xf3\xa9\xb5\x24\xaf\x60\x12\x06\x2f\xe0\x37\xa6", 20) == 0);

	PBKDF2_set_iterations(kdf, 2);
	kdf_begin(kdf, "password", 8, (const uint8_t*)"salt", 4);
	kdf_read(kdf, res, 20);
	ASSERT(memcmp(res, "\xea\x6c\x01\x4d\xc7\x2d\x6f\x8c\xcd\x1e\xd9\x2a\xce\x1d\x41\xf0\xd8\xde\x89\x57", 20) == 0);

	PBKDF2_set_iterations(kdf, 4096);
	kdf_begin(kdf, "password", 8, (const uint8_t*)"salt", 4);
	kdf_read(kdf, res, 20);
	ASSERT(memcmp(res, "\x4b\x00\x79\x01\xb7\x65\x48\x9a\xbe\xad\x49\xd9\x26\xf7\x21\xd0\x65\xa4\x29\xc1", 20) == 0);

#if CPU_X86
	// Too slow for an embedded system...
	PBKDF2_set_iterations(kdf, 16777216);
	kdf_begin(kdf, "password", 8, (const uint8_t*)"salt", 4);
	kdf_read(kdf, res, 20);
	ASSERT(memcmp(res, "\xee\xfe\x3d\x61\xcd\x4d\xa4\xe4\xe9\x94\x5b\x3d\x6b\xa2\x15\x8c\x26\x34\xe9\x84", 20) == 0);
#endif

	PBKDF2_set_iterations(kdf, 4096);
	kdf_begin(kdf, "pass\0word", 9, (const uint8_t*)"sa\0lt", 5);
	kdf_read(kdf, res, 16);
	ASSERT(memcmp(res, "\x56\xfa\x6a\xa7\x55\x48\x09\x9d\xcc\x37\xd7\xf0\x34\x25\xe0\xc3", 16) == 0);

	PBKDF2_set_iterations(kdf, 4096);
	kdf_begin(kdf, "passwordPASSWORDpassword", 24, (const uint8_t*)"saltSALTsaltSALTsaltSALTsaltSALTsalt", 36);
	kdf_read(kdf, res, 25);
	ASSERT(memcmp(res, "\x3d\x2e\xec\x4f\xe4\x1c\x84\x9b\x80\xc8\xd8\x36\x62\xc0\xe4\x4a\x8b\x29\x1a\x96\x4c\xf2\xf0\x70\x38", 25) == 0);

	return 0;
}

TEST_MAIN(PBKDF2);

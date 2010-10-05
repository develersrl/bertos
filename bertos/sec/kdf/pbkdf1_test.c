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
 * \brief PBKDF1 testsuite
 * \author Giovanni Bajo <rasky@develer.com>
 *
 */

#include "pbkdf1.h"

#include <cfg/test.h>
#include <cfg/debug.h>

#include <cpu/detect.h>

#include <sec/mac/hmac.h>
#include <sec/hash/sha1.h>

#include <string.h>

int PBKDF1_testSetup(void)
{
	kdbg_init();
	return 0;
}

int PBKDF1_testTearDown(void)
{
	return 0;
}

int PBKDF1_testRun(void)
{
	Kdf *kdf = PBKDF1_stackinit(SHA1_stackinit());

	uint8_t res[16];

	PBKDF1_set_iterations(kdf, 1000);
	kdf_begin(kdf, "password", 8, (const uint8_t*)"\x78\x57\x8E\x5A\x5D\x63\xCB\x06", 8);
	kdf_read(kdf, res, 16);
	ASSERT(memcmp(res, "\xDC\x19\x84\x7E\x05\xC6\x4D\x2F\xAF\x10\xEB\xFB\x4A\x3D\x2A\x20", 16) == 0);

	return 0;
}

TEST_MAIN(PBKDF1);

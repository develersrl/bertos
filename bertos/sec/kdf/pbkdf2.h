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
 * \brief PBKDF2 implementation
 * \author Giovanni Bajo <rasky@develer.com>
 *
 */

#ifndef SEC_KDF_PBKDF2_H
#define SEC_KDF_PBKDF2_H

#include <sec/kdf.h>
#include <sec/mac.h>

typedef struct
{
	Kdf kdf;
	Mac *mac;
	uint8_t block[20];
	uint32_t c;
	uint32_t iterations;
	uint8_t salt_len;
	uint8_t salt[48];
} PBKDF2_Context;

void PBKDF2_init(PBKDF2_Context *ctx, Mac *mac);
void PBKDF2_set_iterations(Kdf *kdf, uint32_t iterations);

#define PBKDF2_stackinit(...) \
	({ PBKDF2_Context *ctx = alloca(sizeof(PBKDF2_Context)); PBKDF2_init(ctx , ##__VA_ARGS__); &ctx->kdf; })

int PBKDF2_testSetup(void);
int PBKDF2_testRun(void);
int PBKDF2_testTearDown(void);

#endif /* SEC_KDF_PBKDF2_H */

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

#include "pbkdf2.h"
#include <cpu/byteorder.h>
#include <sec/util.h>
#include <cfg/debug.h>

static void PBKDF2_begin(Kdf *ctx_, const char *pwd, size_t pwd_len,
					     const uint8_t *salt, size_t salt_len)
{
	PBKDF2_Context *ctx = (PBKDF2_Context*)ctx_;
	
	ASSERT(sizeof(ctx->salt) >= salt_len);
	
	mac_set_key(ctx->mac, (const uint8_t*)pwd, pwd_len);
	ctx->salt_len = salt_len;
	memcpy(ctx->salt, salt, salt_len);
	ctx->c = 0;
	ctx->kdf.to_read = 0;
	ctx->kdf.block = NULL;
}

static void PBKDF2_next(Kdf *ctx_)
{
	PBKDF2_Context *ctx = (PBKDF2_Context*)ctx_;
	int dlen = mac_digest_len(ctx->mac);
	uint8_t last[dlen];
	
	++ctx->c;
	uint32_t bec = cpu_to_be32(ctx->c);
	
	mac_begin(ctx->mac);
	mac_update(ctx->mac, ctx->salt, ctx->salt_len);
	mac_update(ctx->mac, &bec, 4);
	memcpy(last, mac_final(ctx->mac), dlen);
	memcpy(ctx->block, last, dlen);
	
	for (uint32_t i=0; i<ctx->iterations-1; ++i)
	{
		mac_begin(ctx->mac);
		mac_update(ctx->mac, last, dlen);
		memcpy(last, mac_final(ctx->mac), dlen);
		xor_block(ctx->block, ctx->block, last, dlen);
	}

	ctx->kdf.to_read = dlen;
	ctx->kdf.block = ctx->block;
	
	PURGE(last);
}


/**********************************************************************/

// Default iteration count. The RFC does not specify a "good" default
// value; it just says that this should be a high value to slow down
// computations. Since slowing down is not much of a concern for an
// embedded system, we settle for a value which is not too big.
#define PBKDF2_DEFAULT_ITERATIONS      100


void PBKDF2_init(PBKDF2_Context *ctx, Mac *mac)
{
	ctx->salt_len = 0;
	ctx->mac = mac;
	ctx->iterations = PBKDF2_DEFAULT_ITERATIONS;
	ctx->kdf.begin = PBKDF2_begin;
	ctx->kdf.next = PBKDF2_next;
	ctx->kdf.block_len = mac_digest_len(mac);
	ASSERT(ctx->kdf.block_len <= sizeof(ctx->block));
}

void PBKDF2_set_iterations(Kdf *ctx_, uint32_t iterations)
{
	PBKDF2_Context *ctx = (PBKDF2_Context*)ctx_;
	ctx->iterations = iterations;
}


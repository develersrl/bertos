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
 * \brief HMAC implementation
 * \author Giovanni Bajo <rasky@develer.com>
 *
 */

#include "hmac.h"
#include <sec/util.h>
#include <string.h>


static void hmac_set_key(Mac *m, const void *key, size_t key_len)
{
	HmacContext *ctx = (HmacContext *)m;

	memset(ctx->key, 0, ctx->m.key_len);
	if (key_len <= ctx->m.key_len)
		memcpy(ctx->key, key, key_len);
	else
	{
		hash_begin(ctx->h);
		hash_update(ctx->h, key, key_len);
		memcpy(ctx->key, hash_final(ctx->h), hash_digest_len(ctx->h));
	}

	xor_block_const(ctx->key, ctx->key, 0x5C, ctx->m.key_len);
}

static void hmac_begin(Mac *m)
{
	HmacContext *ctx = (HmacContext *)m;
	int klen = ctx->m.key_len;

	xor_block_const(ctx->key, ctx->key, 0x36^0x5C, klen);
	hash_begin(ctx->h);
	hash_update(ctx->h, ctx->key, klen);
}

static void hmac_update(Mac *m, const void *data, size_t len)
{
	HmacContext *ctx = (HmacContext *)m;
	hash_update(ctx->h, data, len);
}

static uint8_t *hmac_final(Mac *m)
{
	HmacContext *ctx = (HmacContext *)m;
	int hlen = hash_digest_len(ctx->h);

	uint8_t temp[hlen];
	memcpy(temp, hash_final(ctx->h), hlen);

	xor_block_const(ctx->key, ctx->key, 0x5C^0x36, ctx->m.key_len);
	hash_begin(ctx->h);
	hash_update(ctx->h, ctx->key, ctx->m.key_len);
	hash_update(ctx->h, temp, hlen);

	PURGE(temp);
	return hash_final(ctx->h);
}

/*********************************************************************/

void hmac_init(HmacContext *ctx, Hash *h)
{
	ctx->h = h;
	ctx->m.key_len = hash_block_len(h);
	ctx->m.digest_len = hash_digest_len(h);
	ctx->m.set_key = hmac_set_key;
	ctx->m.begin = hmac_begin;
	ctx->m.update = hmac_update;
	ctx->m.final = hmac_final;
	ASSERT(sizeof(ctx->key) >= ctx->m.key_len);
}

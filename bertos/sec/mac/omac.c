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
 * Copyright 2006 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief OMAC implementation
 * \author Giovanni Bajo <rasky@develer.com>
 */

#include "omac.h"
#include <cfg/macros.h>
#include <sec/util.h>

static void omac_set_key(Mac *ctx_, const void* key, size_t len)
{
	OmacContext *ctx = (OmacContext *)ctx_;
	cipher_set_vkey(ctx->c, key, len);
}

static void omac_begin(Mac *ctx_)
{
	OmacContext *ctx = (OmacContext *)ctx_;

	ctx->acc = 0;

	memset(ctx->Y, 0, cipher_block_len(ctx->c));
	cipher_cbc_begin(ctx->c, ctx->Y);
}

static void omac_update(Mac *ctx_, const void *data_, size_t len)
{
	OmacContext *ctx = (OmacContext *)ctx_;
	size_t blen = cipher_block_len(ctx->c);
	const uint8_t *data = (const uint8_t *)data_;

	while (len)
	{
		ASSERT(ctx->acc <= blen);
		if (ctx->acc == blen)
		{
			cipher_cbc_encrypt(ctx->c, ctx->accum);
			ctx->acc = 0;
		}

		size_t L = MIN(len, blen - ctx->acc);
		memcpy(ctx->accum + ctx->acc, data, L);
		data += L;
		len -= L;
		ctx->acc += L;
	}
}

static void omac_shift_left(uint8_t *L, size_t len)
{
	int firstbit = L[0] >> 7;
	size_t i;

	for (i=0; i<len-1; ++i)
		L[i] = (L[i] << 1) | (L[i+1] >> 7);
	L[i] <<= 1;

	if (firstbit)
	{
		if (len == 16)
			L[i] ^= 0x87;
		else if (len == 8)
			L[i] ^= 0x1B;
		else
			ASSERT(0);
	}
}

static void omac_shift_right(uint8_t *L, size_t len)
{
	int firstbit = L[len-1] & 1;
	size_t i;

	for (i=len-1; i>0; --i)
		L[i] = (L[i] >> 1) | (L[i-1] << 7);
	L[i] >>= 1;

	if (firstbit)
	{
		L[i] |= 0x80;
		if (len == 16)
			L[len-1] ^= 0x43;
		else if (len == 8)
			L[len-1] ^= 0x0D;
		else
			ASSERT(0);
	}
}

static uint8_t *omac1_final(Mac *ctx_)
{
	OmacContext *ctx = (OmacContext *)ctx_;
	size_t blen = cipher_block_len(ctx->c);

	uint8_t L[blen];
	memset(L, 0, blen);
	cipher_ecb_encrypt(ctx->c, L);

	omac_shift_left(L, blen);
	if (ctx->acc < blen)
	{
		ctx->accum[ctx->acc++] = 0x80;
		memset(ctx->accum + ctx->acc, 0, blen - ctx->acc);
		omac_shift_left(L, blen);
	}

	xor_block(ctx->accum, ctx->accum, L, blen);
	cipher_cbc_encrypt(ctx->c, ctx->accum);
	return ctx->accum;
}

static uint8_t *omac2_final(Mac *ctx_)
{
	OmacContext *ctx = (OmacContext *)ctx_;
	size_t blen = cipher_block_len(ctx->c);

	uint8_t L[blen];
	memset(L, 0, blen);
	cipher_ecb_encrypt(ctx->c, L);

	if (ctx->acc < blen)
	{
		ctx->accum[ctx->acc++] = 0x80;
		memset(ctx->accum + ctx->acc, 0, blen - ctx->acc);
		omac_shift_right(L, blen);
	}
	else
		omac_shift_left(L, blen);

	xor_block(ctx->accum, ctx->accum, L, blen);
	cipher_cbc_encrypt(ctx->c, ctx->accum);
	return ctx->accum;
}


/****************************************************************************/

static void omac_init(OmacContext *ctx, BlockCipher *c)
{
	ctx->mac.set_key = omac_set_key;
	ctx->mac.begin = omac_begin;
	ctx->mac.update = omac_update;
	ctx->mac.key_len = cipher_key_len(c);
	ctx->mac.digest_len = cipher_block_len(c);
	ctx->c = c;

	ASSERT(cipher_block_len(c) == 8 || cipher_block_len(c) == 16);
	ASSERT(sizeof(ctx->Y) >= cipher_block_len(c));
	ASSERT(sizeof(ctx->accum) >= cipher_block_len(c));
}

void omac1_init(OmacContext *ctx, BlockCipher *c)
{
	omac_init(ctx, c);
	ctx->mac.final = omac1_final;
}

void omac2_init(OmacContext *ctx, BlockCipher *c)
{
	omac_init(ctx, c);
	ctx->mac.final = omac2_final;
}

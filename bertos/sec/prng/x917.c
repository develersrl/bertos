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
 * \brief ANSI X9.17 PRNG implementation
 * \author Giovanni Bajo <rasky@develer.com>
 *
 */

#include "x917.h"
#include <sec/util.h>
#include <drv/timer.h>
#include "hw/hw_timer.h"

static void x917_next(X917Context *ctx, BlockCipher *cipher, uint8_t *out)
{
	const size_t blen = cipher_block_len(cipher);

	union
	{
		uint8_t bytes[blen];
		struct
		{
			time_t t0;
			hptime_t t1;
		} data;
	} DT;

	ASSERT(sizeof(DT.bytes) >= sizeof(ticks_t) + sizeof(hptime_t));

	DT.data.t0 = timer_clock();
	DT.data.t1 = timer_hw_hpread();

	cipher_ecb_encrypt(cipher, DT.bytes);

	xor_block(out, DT.bytes, ctx->state, blen);
	cipher_ecb_encrypt(cipher, out);

	xor_block(ctx->state, DT.bytes, out, blen);
	cipher_ecb_encrypt(cipher, ctx->state);

	PURGE(DT.bytes);
}


static void x917_generate(PRNG *ctx_, uint8_t *data, size_t len)
{
	X917Context *ctx = (X917Context *)ctx_;
	BlockCipher *cipher = AES128_stackinit();

	const size_t blen = cipher_block_len(cipher);
	uint8_t temp[blen];

	ASSERT(len);
	ASSERT(sizeof(ctx->state) >= blen);
	ASSERT(sizeof(ctx->key) >= cipher_key_len(cipher));

	cipher_set_key(cipher, ctx->key);

	while (len)
	{
		size_t L = MIN(blen, len);
		x917_next(ctx, cipher, temp);
		memcpy(data, temp, L);
		len -= L;
		data += L;
	}
}

static void x917_reseed(PRNG *ctx_, const uint8_t *seed)
{
	// The X9.17 standard does not specify reseeding. To avoid external
	// dependencies, we implement it this way:
	//   * Generate a new random block, xor it with the first part
	//     of the seed, and use the result as new seed.
	//   * Generate and throw away a block to update the state.
	X917Context *ctx = (X917Context *)ctx_;

	size_t klen = sizeof(ctx->key);
	size_t blen = sizeof(ctx->state);

	uint8_t buf[klen];
	x917_generate(ctx_, buf, klen);

	xor_block(ctx->key, buf, seed, klen);
	xor_block(ctx->state, ctx->state, seed+klen, blen);

	PURGE(buf);
}

/*********************************************************************/

void x917_init(X917Context *ctx)
{
	ctx->rng.reseed = x917_reseed;
	ctx->rng.generate = x917_generate;
	ctx->rng.seed_len = sizeof(ctx->key) + sizeof(ctx->state);
}

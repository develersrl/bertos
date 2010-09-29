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
 * \brief Yarrow implementation
 * \author Giovanni Bajo <rasky@develer.com>
 *
 */

#include "yarrow.h"
#include <sec/hash/sha1.h>
#include <cfg/macros.h>
#include <string.h>

#define CONFIG_YARROW_GENERATOR_GATE             10

static void yarrow_generate(PRNG *ctx_, uint8_t *data, size_t len)
{
	YarrowContext *ctx = (YarrowContext *)ctx_;
	BlockCipher *cipher = 0;

	ASSERT(len != 0);

	do
	{
		if (ctx->lastidx == sizeof(ctx->last))
		{
			if (!cipher)
			{
				cipher = AES128_stackinit();
				ASSERT(sizeof(ctx->counter) == cipher_block_len(cipher));
				ASSERT(sizeof(ctx->curkey) == cipher_key_len(cipher));

				cipher_set_key(cipher, ctx->curkey);
				cipher_ctr_begin(cipher, ctx->counter);
			}

			cipher_ctr_step(cipher, ctx->last);

			if (ctx->curkey_gencount == CONFIG_YARROW_GENERATOR_GATE)
			{
				ASSERT(cipher_block_len(cipher) == cipher_key_len(cipher));
				cipher_set_key(cipher, ctx->last);
				ctx->curkey_gencount = 0;
				continue;
			}

			ctx->lastidx = 0;
			ctx->curkey_gencount++;
		}

		int n = MIN(len, 16U-ctx->lastidx);
		memcpy(data, ctx->last+ctx->lastidx, n);
		data += n;
		len -= n;
		ctx->lastidx += n;
	} while (len);
}

static void yarrow_reseed(PRNG *ctx_, const uint8_t *seed)
{
	YarrowContext *ctx = (YarrowContext *)ctx_;
	Hash *h = SHA1_stackinit();

	hash_begin(h);
	hash_update(h, seed, ctx->prng.seed_len);
	hash_update(h, ctx->curkey, sizeof(ctx->curkey));
	memcpy(ctx->curkey, hash_final(h), sizeof(ctx->curkey));

	// Reset the counter for the sequence
	memset(ctx->counter, 0, sizeof(ctx->counter));
}


/*********************************************************************/

void yarrow_init(YarrowContext *ctx)
{
	ctx->prng.reseed = yarrow_reseed;
	ctx->prng.generate = yarrow_generate;
	ctx->prng.seed_len = 16;
	ctx->prng.seeded = 0;

	ctx->lastidx = sizeof(ctx->last);
	ctx->curkey_gencount = 0;
	memset(ctx->curkey, 0, sizeof(ctx->curkey));

	ASSERT(sizeof(ctx->counter) == sizeof(ctx->last));
}

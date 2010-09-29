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
 * \brief Yarrow entropy pool
 * \author Giovanni Bajo <rasky@develer.com>
 *
 */

#include "yarrow_pool.h"
#include <cfg/macros.h>
#include <sec/hash/sha1.h>
#include <sec/util.h>
#include <string.h>

#define CONFIG_YARROW_POOL_FAST_RESEEDING_THRESHOLD   100
#define CONFIG_YARROW_POOL_SLOW_RESEEDING_THRESHOLD   160
#define CONFIG_YARROW_POOL_MAX_ENTROPY_DENSITY        50   // percent
#define CONFIG_YARROW_POOL_RESEED_ITERATIONS          80

static void yarrow_fast_reseed(YarrowPoolContext *ctx, uint8_t *out, size_t len)
{
	Hash *h = &ctx->pools[0].hash.h;
	size_t hlen = hash_digest_len(h);

	uint8_t v0[hlen];
	memcpy(v0, hash_final(h), hlen);

	uint8_t vcur[hlen];
	memcpy(vcur, v0, hlen);

	for (uint32_t i=1;i<=CONFIG_YARROW_POOL_RESEED_ITERATIONS; ++i) {
		hash_begin(h);
		hash_update(h, vcur, hlen);
		hash_update(h, v0, hlen);
		hash_update(h, &i, 4);
		memcpy(vcur, hash_final(h), hlen);
	}


	// FIXME: yarrow explains how to expand the hash digest if it's
	// smaller than the output size. This is not the case for now,
	// so we it's not implemented here.
	ASSERT(len < hlen);
	memcpy(out, vcur, len);

	// Reinitialize the fast pool
	hash_begin(h);
	for (int i=0; i<CONFIG_ENTROPY_NUM_SOURCES; ++i)
		ctx->pools[0].entropy[i] = 0;

	PURGE(v0);
	PURGE(vcur);
}

static void yarrow_slow_reseed(YarrowPoolContext *ctx, uint8_t *out, size_t len)
{
	uint8_t *data = hash_final(&ctx->pools[1].hash.h);

	hash_update(&ctx->pools[0].hash.h, data, hash_digest_len(&ctx->pools[1].hash.h));
	yarrow_fast_reseed(ctx, out, len);

	// Reinitialize the slow pool
	hash_begin(&ctx->pools[1].hash.h);
	for (int i=0; i<CONFIG_ENTROPY_NUM_SOURCES; ++i)
		ctx->pools[1].entropy[i] = 0;
}

static void yarrow_add_entropy(EntropyPool *ctx_, int source_idx, const uint8_t *data, size_t len, int entropy)
{
	YarrowPoolContext *ctx = (YarrowPoolContext *)ctx_;

	ASSERT(source_idx < CONFIG_ENTROPY_NUM_SOURCES);

	int curpool = ctx->sources[source_idx].curpool;
	ctx->sources[source_idx].curpool = 1 - ctx->sources[source_idx].curpool;

	// TODO: Yarrow also describes a statistical entropy estimator
	// Currently, we just use the provided entropy and the maximum
	// density.
	entropy = MIN((size_t)entropy, len*8*100/CONFIG_YARROW_POOL_MAX_ENTROPY_DENSITY);

	hash_update(&ctx->pools[curpool].hash.h, data, len);
	ctx->pools[curpool].entropy[source_idx] += entropy;
}

static bool yarrow_fast_reseeding_ready(YarrowPoolContext *ctx)
{
	for (int i=0; i<CONFIG_ENTROPY_NUM_SOURCES; ++i)
		if (ctx->pools[0].entropy[i] >=
		        CONFIG_YARROW_POOL_FAST_RESEEDING_THRESHOLD)
			return 1;
	return 0;
}

static bool yarrow_slow_reseeding_ready(YarrowPoolContext *ctx)
{
	int count = 0;

	for (int i=0; i<CONFIG_ENTROPY_NUM_SOURCES; ++i)
		if (ctx->pools[1].entropy[i] >=
		        CONFIG_YARROW_POOL_SLOW_RESEEDING_THRESHOLD) {
			++count;
			if (count == 2)
				return 1;
		}

	return 0;
}

static bool yarrow_reseeding_ready(EntropyPool *ctx_)
{
	YarrowPoolContext *ctx = (YarrowPoolContext *)ctx_;
	return yarrow_fast_reseeding_ready(ctx) || yarrow_slow_reseeding_ready(ctx);
}

static void yarrow_reseed(EntropyPool *ctx_, uint8_t *out, size_t len)
{
	YarrowPoolContext *ctx = (YarrowPoolContext *)ctx_;

	if (yarrow_slow_reseeding_ready(ctx))
		yarrow_slow_reseed(ctx, out, len);
	else {
		ASSERT(yarrow_fast_reseeding_ready(ctx));
		yarrow_fast_reseed(ctx, out, len);
	}
}

/**********************************************************************/

void yarrowpool_init(YarrowPoolContext *ctx)
{
	ctx->e.add_entropy = yarrow_add_entropy;
	ctx->e.seeding_ready = yarrow_reseeding_ready;
	ctx->e.make_seed = yarrow_reseed;

	for (int i=0; i<2; ++i) {
		memset(ctx->pools[i].entropy, 0, sizeof(ctx->pools[i].entropy));
		SHA1_init(&ctx->pools[i].hash);
		hash_begin(&ctx->pools[i].hash.h);
	}

	memset(ctx->sources, 0, sizeof(ctx->sources));
}

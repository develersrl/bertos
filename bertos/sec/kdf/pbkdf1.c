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
 * \brief PBKDF1 implementation
 * \author Giovanni Bajo <rasky@develer.com>
 * 
 */

#include "pbkdf1.h"
#include <sec/hash.h>
#include <sec/util.h>


static void PBKDF1_begin(Kdf *ctx_, const char *pwd, size_t pwd_len,
					     const uint8_t *salt, size_t salt_len)
{
	PBKDF1_Context *ctx = (PBKDF1_Context *)ctx_;

	hash_begin(ctx->hash);
	hash_update(ctx->hash, pwd, pwd_len);
	hash_update(ctx->hash, salt, salt_len);
	
	ctx->kdf.to_read = 0;
	ctx->kdf.block = NULL;
}

static void PBKDF1_next(Kdf *ctx_)
{
	PBKDF1_Context *ctx = (PBKDF1_Context *)ctx_;
	
	// PBKDF1 will generate only one block of data (whose len depends
	// on the underlying hash function). After that, the generation stops
	// with an ASSERT. If you use PKBDF1, you are supposed to be aware
	// of this limit while designing your algorithm.
	ASSERT(ctx->kdf.block == NULL);

	int hlen = hash_digest_len(ctx->hash);
	uint8_t temp[hlen];
	uint8_t *final = hash_final(ctx->hash);
	
	for (uint32_t i=0; i<ctx->iterations-1; i++)
	{
		memcpy(temp, final, hlen);
		hash_begin(ctx->hash);
		hash_update(ctx->hash, temp, hlen);
		final = hash_final(ctx->hash);
	}

	PURGE(temp);

	ctx->kdf.to_read = ctx->kdf.block_len;
	ctx->kdf.block = final;
}

/**********************************************************************/

// Default iteration count. The RFC does not specify a "good" default
// value; it just says that this should be a high value to slow down
// computations. Since slowing down is not much of a concern for an
// embedded system, we settle for a value which is not too big.
#define PBKDF1_DEFAULT_ITERATIONS      100


void PBKDF1_init(PBKDF1_Context *ctx, Hash *h)
{
	ctx->hash = h;
	ctx->iterations = PBKDF1_DEFAULT_ITERATIONS;
	ctx->kdf.begin = PBKDF1_begin;
	ctx->kdf.next = PBKDF1_next;
	ctx->kdf.block_len = hash_digest_len(h);
}

void PBKDF1_set_iterations(Kdf *ctx_, uint32_t iterations)
{
	PBKDF1_Context *ctx = (PBKDF1_Context *)ctx_;
	ctx->iterations = iterations;
}

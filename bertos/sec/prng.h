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
 * \brief Generic interface for cryptographically-secure pseudo-RNG
 * \author Giovanni Bajo <rasky@develer.com>
 *
 */

#ifndef SEC_PRNG_H
#define SEC_PRNG_H

#include <cfg/compiler.h>
#include <cfg/debug.h>

typedef struct PRNG
{
    void (*reseed)(struct PRNG *ctx, const uint8_t *seed);
    void (*generate)(struct PRNG *ctx, uint8_t *data, size_t len);
    uint8_t seed_len;
	uint8_t seeded;
} PRNG;

/**
 * Feed a new seed into the PRNG.
 *
 * \note: Being a cryptographically-secure PRNG, the seed will be
 * mixed to the current state of the generator, so it is NOT possible
 * to generate the same sequence simply by using the same seed. If you
 * need such a property, use a normale PRGN like \a rand().
 */
INLINE void prng_reseed(PRNG *ctx, const uint8_t *seed)
{
    ASSERT(ctx->reseed);
    ctx->reseed(ctx, seed);
	ctx->seeded = 1;
}

/**
 * Get the length of the seed in bytes required by this generator.
 */
INLINE size_t prng_seed_len(PRNG *ctx)
{
    return ctx->seed_len;
}

/**
 * Generate some cryptographically-secure random bytes into the specified
 * buffer.
 */
INLINE void prng_generate(PRNG *ctx, uint8_t *data, size_t len)
{
    ASSERT(ctx->generate);
	ASSERT(ctx->seeded);
    ctx->generate(ctx, data, len);
}


#endif /* SEC_PRNG_H */

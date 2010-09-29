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
 * \brief Entropy pool generic interface
 * \author Giovanni Bajo <rasky@develer.com>
 *
 */

#ifndef SEC_ENTROPY_H
#define SEC_ENTROPY_H

#include <cfg/compiler.h>
#include <cfg/debug.h>

/**
 * Maximum number of different sources of entropy available in BeRTOS.
 *
 * Increasing this value will likely make entropy pools use more RAM for their operations,
 * so it should be kept to the minimum necessary for a given project.
 */
#define CONFIG_ENTROPY_NUM_SOURCES   8

typedef struct EntropyPool
{
    void (*add_entropy)(struct EntropyPool *ctx, int source_idx,
                        const uint8_t *data, size_t len,
                        int entropy);
    bool (*seeding_ready)(struct EntropyPool *ctx);
    void (*make_seed)(struct EntropyPool *ctx, uint8_t *out, size_t len);

} EntropyPool;


/**
 * Add some data samples containing entropy into the pool.
 * the samples are in the buffer pointed by \a data for a total of \a len
 * bytes. \a entropy is the number of bits of estimated entropy in the
 * samples. \a source_idx is the index of the entropy source.
 */
INLINE void entropy_add(EntropyPool *e, int source_idx,
                 const uint8_t *data, size_t len,
                 int entropy)
{
    ASSERT(e->add_entropy);
    e->add_entropy(e, source_idx, data, len, entropy);
}

/**
 * Check if the generator is ready to produce a new seed.
 */
INLINE bool entropy_seeding_ready(EntropyPool *ctx)
{
    ASSERT(ctx->seeding_ready);
    return ctx->seeding_ready(ctx);
}

/**
 * Generate a new seed of the specified length.
 *
 * \note This should not be abused to generate a very long seed, since the pool
 * cannot hold lots of entropy.
 */
INLINE void entropy_make_seed(EntropyPool *ctx, uint8_t *out, size_t len)
{
    ASSERT(ctx->make_seed);
    ctx->make_seed(ctx, out, len);
}

#endif /* SEC_ENTROPY_H */

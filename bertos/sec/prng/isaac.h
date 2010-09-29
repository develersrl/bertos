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
 * \brief ISAAC implementation
 * \author Giovanni Bajo <rasky@develer.com>
 *
 */

#ifndef SEC_PRNG_ISAAC_H
#define SEC_PRNG_ISAAC_H

#include <sec/prng.h>

/**
 * Size of the internal ISAAC state (in 32-bit words).
 *
 * ISAAC is known to generate unbiased data as follows:
 *   * 3 words: 2^37 unbiased values
 *   * 4 words: 2^45 unbiased values
 *   * 5 words: 2^53 unbiased values
 *   * 6 words: 2^61 unbiased values
 *   * 7 words: 2^69 unbiased values
 *   * 8 words: 2^77 unbiased values
 *
 * The period of the generator is usually much longer, but it is
 * obviously uninteresting for a CSPRNG.
 */
#define CONFIG_ISAAC_RANDSIZL   (3)
#define CONFIG_ISAAC_RANDSIZ    (1<<(CONFIG_ISAAC_RANDSIZL))

typedef struct IsaacContext
{
	PRNG prng;
	uint32_t randcnt;
	uint32_t randrsl[CONFIG_ISAAC_RANDSIZ];
	uint32_t randmem[CONFIG_ISAAC_RANDSIZ];
	uint32_t randa;
	uint32_t randb;
	uint32_t randc;
} IsaacContext;

void isaac_init(IsaacContext *ctx);

#define isaac_stackinit(...) \
	({ IsaacContext *ctx = alloca(sizeof(IsaacContext)); isaac_init(ctx , ##__VA_ARGS__); &ctx->prng; })


#endif /* SEC_PRNG_ISAAC_H */

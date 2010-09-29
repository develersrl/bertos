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
 * \brief High-level random number generation functions.
 * \author Giovanni Bajo <rasky@develer.com>
 *
 */

#include "random.h"
#include "random_p.h"

#include <cfg/macros.h>
#include <drv/timer.h>
#include <sec/random.h>
#include <sec/prng.h>
#include <sec/entropy.h>
#include <sec/util.h>
#include <sec/hash/sha1.h>
#include <sec/prng/isaac.h>
#include <sec/prng/x917.h>
#include <sec/prng/yarrow.h>
#include <sec/entropy/yarrow_pool.h>

/********************************************************************************/
/* Configuration of the random module                                           */
/********************************************************************************/

#define POOL_CONTEXT          PP_CAT(PP_CAT(POOL_NAMEU, CONFIG_RANDOM_POOL), Context)
#define POOL_INIT             PP_CAT(PP_CAT(POOL_NAMEL, CONFIG_RANDOM_POOL), _init)

#define EXTRACTOR_STACKINIT   PP_CAT(PP_CAT(EXTRACTOR_NAME, CONFIG_RANDOM_EXTRACTOR), _stackinit)

#define PRNG_CONTEXT          PP_CAT(PP_CAT(PRNG_NAMEU, CONFIG_RANDOM_PRNG), Context)
#define PRNG_INIT             PP_CAT(PP_CAT(PRNG_NAMEL, CONFIG_RANDOM_PRNG), _init)


/********************************************************************************/
/* Global state                                                                 */
/********************************************************************************/

#if CONFIG_RANDOM_POOL != POOL_NONE
static POOL_CONTEXT epool_ctx;
static EntropyPool * const epool = (EntropyPool *)&epool_ctx;
#endif

static PRNG_CONTEXT prng_ctx;
static PRNG * const prng = (PRNG*)&prng_ctx;

static bool initialized = 0;


/********************************************************************************/
/* Code                                                                         */
/********************************************************************************/

/*
 * Reseed the PRNG if there is enough entropy available at this time.
 *
 * Some designs (eg: fortuna) suggest to artificially limit the frequency of
 * this operation to something like 0.1s, to avoid attacks that try to exhaust
 * the entropy pool.
 *
 * We don't believe such attacks are available in an embedded system (as an attacker
 * does not have a way to ask random numbers from the pool) but we will play safe
 * here in case eg. the user does something wrong.
 */
static void optional_reseeding(void)
{
#if CONFIG_RANDOM_POOL != POOL_NONE
	static ticks_t last_reseed = -1000;

	// We don't allow more than 10 reseedings per second
	// (as suggested by Fortuna)
	ticks_t current = timer_clock();
	if (ticks_to_ms(current - last_reseed) < 100)
		return;

	if (entropy_seeding_ready(epool))
	{
		uint8_t seed[prng_seed_len(prng)];

		entropy_make_seed(epool, seed, sizeof(seed));
		prng_reseed(prng, seed);

		last_reseed = current;
		PURGE(seed);
	}
#endif
}


/*
 * Perform the initial seeding of the PRNG.
 *
 * At startup, we want to immediately seed the PRNG to a point where it can
 * generate safe-enough random numbers. To do this, we rely on a hw-dependent
 * function to pull entropy from available hw sources, and then feed it
 * through an extractor (if any is configured).
 */
static void initial_seeding(void)
{
#if CONFIG_RANDOM_POOL != POOL_NONE

	// We feed entropy into the pool, until it is ready to perform a seeding.
	do
	{
		uint8_t buf[16];
		random_pull_entropy(buf, sizeof(buf));
		entropy_add(epool, 0, buf, sizeof(buf), sizeof(buf)*8);
	} while (!entropy_seeding_ready(epool));

	optional_reseeding();

#elif CONFIG_RANDOM_EXTRACTOR != EXTRACTOR_NONE

	uint8_t seed[prng_seed_len(prng)];
	Hash *h = EXTRACTOR_STACKINIT();

	// "Randomness Extraction and Key Derivation Using the CBC, Cascade and
	// HMAC Modes" by Yevgeniy Dodis et al. suggests that an padded cascaded hash
	// function with N bits of output must have at least 2n bits of min-entropy as input
	// to be a randomness extractor (it generates an output that is computationally
	// indistiguishable from an uniform distribution).
	size_t hlen = hash_digest_len(h);
	uint8_t buf[hlen*2];
	size_t sidx = 0;

	while (sidx < sizeof(seed))
	{
		size_t cnt = MIN(sizeof(seed) - sidx, hlen);

		random_pull_entropy(buf, sizeof(buf));

		hash_begin(h);
		hash_update(h, buf, sizeof(buf));
		memcpy(seed+sidx, hash_final(h), cnt);
		sidx += cnt;
	}

	prng_reseed(prng, seed);
	PURGE(buf);
	PURGE(seed);

#else

	// If we have been configured without a proper randomness extractor, we do not
	// have many solutions but feeding the entropy bits directly to the PRNG, and
	// hoping for the best.
	uint8_t seed[prng_seed_len(prng)];
	random_pull_entropy(seed, sizeof(seed));
	prng_reseed(prng, seed);
	PURGE(seed);

#endif
}

void random_init(void)
{
#if CONFIG_RANDOM_POOL != POOL_NONE
	POOL_INIT(&epool_ctx);
#endif
	PRNG_INIT(&prng_ctx);

	initialized = 1;
	initial_seeding();
}

void random_gen(uint8_t *out, size_t len)
{
	ASSERT(initialized);

	optional_reseeding();
	prng_generate(prng, out, len);
}

#if CONFIG_RANDOM_POOL != POOL_NONE

void random_add_entropy(enum EntropySource source_idx,
                        const uint8_t *data, size_t len,
                        int entropy)
{
	entropy_add(epool, source_idx, data, len, entropy);
}

#endif

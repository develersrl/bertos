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
 * \brief Internal function definitions for random
 * \author Giovanni Bajo <rasky@develer.com>
 *
 */

#ifndef SEC_RANDOM_P_H
#define SEC_RANDOM_P_H

#include <cfg/compiler.h>
#include <sec/random.h>

/********************************************************************************/
/* Configuration of the random module                                           */
/********************************************************************************/

#define POOL_NONE       0
#define POOL_YARROW     1
#define POOL_NAMEU1     YarrowPool
#define POOL_NAMEL1     yarrowpool

#define PRNG_ISAAC      1
#define PRNG_X917       2
#define PRNG_YARROW     3
#define PRNG_NAMEU1     Isaac
#define PRNG_NAMEL1     isaac
#define PRNG_NAMEU2     X917
#define PRNG_NAMEL2     x917
#define PRNG_NAMEU3     Yarrow
#define PRNG_NAMEL3     yarrow

#define EXTRACTOR_NONE  0
#define EXTRACTOR_SHA1  1
#define EXTRACTOR_NAME1 SHA1

#if RANDOM_SECURITY_LEVEL == RANDOM_SECURITY_STRONG
	#define CONFIG_RANDOM_POOL          POOL_YARROW
	#define CONFIG_RANDOM_EXTRACTOR     EXTRACTOR_NONE   // not required with a pool
	#define CONFIG_RANDOM_PRNG          PRNG_YARROW
#elif RANDOM_SECURITY_LEVEL == RANDOM_SECURITY_MEDIUM
	#define CONFIG_RANDOM_POOL          POOL_NONE
	#define CONFIG_RANDOM_EXTRACTOR     EXTRACTOR_SHA1
	#define CONFIG_RANDOM_PRNG          PRNG_X917
#elif RANDOM_SECURITY_LEVEL == RANDOM_SECURITY_MINIMUM
	#define CONFIG_RANDOM_POOL          POOL_NONE
	#define CONFIG_RANDOM_EXTRACTOR     EXTRACTOR_NONE
	#define CONFIG_RANDOM_PRNG          PRNG_ISAAC
#else
	#error Unsupported random security level value
#endif

/***************************************************************************/
/* Internal functions used by BeRTOS drivers to push data into             */
/* the entropy pool                                                        */
/***************************************************************************/

#if CONFIG_RANDOM_POOL != POOL_NONE

enum EntropySource
{
	ENTROPY_SOURCE_IRQ,
	ENTROPY_SOURCE_ADC,
};

/*
 * Add entropy to the global entropy pool.
 */
void random_add_entropy(enum EntropySource source_idx,
					    const uint8_t *data, size_t len,
						int entropy);


/*
 * Add entropy to the global interrupt pool based on the IRQ
 * call time.
 *
 * This function can be called from interrupt handlers that are
 * triggered at unpredictable intervals (so it should not be
 * called from clock-driven interrupts like ADC, PWM, etc.).
 *
 */
void random_add_entropy_irq(int irq);

#endif

/*
 * This hardware-dependent function can be used to pull raw
 * entropy from a hardware source at startup only. It is used
 * for initial seeding of the random generator and should not
 * be used in different situations.
 */
void random_pull_entropy(uint8_t *entropy, size_t len);

#endif /* SEC_RANDOM_P_H */

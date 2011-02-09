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
 * $WIZ$ module_name = "random"
 * $WIZ$ module_configuration = "bertos/cfg/cfg_random.h"
 * $WIZ$ module_depends = "isaac", "cipher", "sha1", "yarrow", "yarrow_pool", "x917", "aes"
 * $WIZ$ module_supports = "stm32 or lm3s"
 */

#ifndef SEC_RANDOM_H
#define SEC_RANDOM_H

#include "cfg/cfg_random.h"
#include <cfg/compiler.h>

/**
 * \name Security level definition
 *
 *
 * Configure the security level required by the application.
 *
 * Application developers are suggested to keep the strongest
 * setting (default) unless there are memory or code size issues.
 *
 * Available settings are:
 *
 *   * \a RANDOM_SECURITY_STRONG: The random library will use
 *     an entropy pool, automatically feeded by drivers, to gather
 *     entropy from hardware sources. Data from the pool will
 *     be used to reseed a secure random number generator. Moreover,
 *     the generator will be automatically initialised
 *     with enough entropy to generate safe random numbers even
 *     immediately after hw reset.
 *     The overall structure is the same as used by modern
 *	   desktop PCs for generating secure random numbers.
 *
 *  * \a RANDOM_SECURITY_MEDIUM: This intermediate settings will
 *     avoid usage of an entropy pool, to reduce memory and code
 *     usage. The security of this settings relies only on the
 *     good behaviour of the random number generator (even though
 *     it will be well-seeded at startup).
 *
 *  * \a RANDOM_SECURITY_MINIMUM: This is the lighter setting that
 *     allows minimal memory and code usage, and it suggested only
 *     for extremely constrained systems, that only generates few
 *     random numbers. Even if the generator is still secure on
 *     paper, its seeding will not be safe (though still entropic
 *     to allow different sequences to be generated after each reset).
 *
 * $WIZ$ random_level = "RANDOM_SECURITY_MINIMUM", "RANDOM_SECURITY_MEDIUM", "RANDOM_SECURITY_STRONG"
 * \{
 */
#define RANDOM_SECURITY_MINIMUM        0
#define RANDOM_SECURITY_MEDIUM         1
#define RANDOM_SECURITY_STRONG         2
/** \} */

void random_init(void);

void random_gen(uint8_t *out, size_t len);

INLINE uint8_t random_gen8(void)
{
	uint8_t x;
	random_gen(&x, 1);
	return x;
}

INLINE uint16_t random_gen16(void)
{
	uint8_t x;
	random_gen(&x, 2);
	return x;
}

INLINE uint32_t random_gen32(void)
{
	uint8_t x;
	random_gen(&x, 4);
	return x;
}

#endif /* SEC_RANDOM_H */

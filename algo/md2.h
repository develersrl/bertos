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
 * Copyright 2007 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief MD2 Message-Digest algorithm.
 *
 * The algorithm takes as input a message of arbitrary length and produces
 * as output a 128-bit message digest of the input.
 * It is conjectured that it is computationally infeasible to produce
 * two messages having the same message digest, or to produce any
 * message having a given prespecified target message digest.
 *
 *
 *
 * \version $Id$
 * \author Daniele Basile <asterix@develer.com>
 */

/*#*
 *#* $Log$
 *#* Revision 1.7  2007/02/15 13:29:49  asterix
 *#* Add MD2_DIGEST_LEN macro.
 *#*
 *#* Revision 1.6  2007/02/02 18:15:31  asterix
 *#* Add function MD2_test. Fix bug in md2_update function.
 *#*
 *#* Revision 1.5  2007/02/02 15:37:45  asterix
 *#* Change md2_end prototype. Remove a unneeded memcpy in md2_end. Add comments.
 *#*
 *#* Revision 1.4  2007/01/31 13:53:36  asterix
 *#* Define COMPUTE_ARRAY_LEN.
 *#*
 *#* Revision 1.3  2007/01/31 11:16:48  asterix
 *#* Defined constants for algorithm compute
 *#*
 *#* Revision 1.2  2007/01/30 17:31:44  asterix
 *#* Add function prototypes.
 *#*
 *#* Revision 1.1  2007/01/30 15:53:26  batt
 *#* Add first md2 skel.
 *#*
 *#*/

#ifndef ALGOS_MD2_H
#define ALGOS_MD2_H

#include <cfg/compiler.h>
#include <appconfig.h>

#define NUM_COMPUTE_ROUNDS 18                        ///< Number of compute rounds.
#define COMPUTE_ARRAY_LEN  CONFIG_MD2_BLOCK_LEN * 3     ///< Lenght of compute array.
#define MD2_DIGEST_LEN CONFIG_MD2_BLOCK_LEN
/**
 * Context for MD2 computation.
 */
typedef struct Md2Context
{
	uint8_t buffer[CONFIG_MD2_BLOCK_LEN];   ///< Input buffer.
	uint8_t state[CONFIG_MD2_BLOCK_LEN];    ///< Current state buffer.
	uint8_t checksum[CONFIG_MD2_BLOCK_LEN]; ///< Checksum.
	size_t counter;                         ///< Counter of remaining bytes.

} Md2Context;

void md2_init(Md2Context *context);
void md2_update(Md2Context *context, const void *block_in, size_t block_len);
uint8_t *md2_end(Md2Context *context);
bool md2_test(void);

#endif /* ALGOS_MD2_H */

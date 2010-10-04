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
 * Copyright 2006 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief AES Advanced Encryption Standard implementation
 *
 * \author Giovanni Bajo <rasky@develer.com>
 *
 * $WIZ$ module_name = "aes"
 */


#include "aes.h"
#include <cfg/compiler.h>
#include <cpu/byteorder.h>

#include <string.h>

// AES only supports Nb=4
#define Nb 4			// number of columns in the state & expanded key

typedef struct
{
	BlockCipher c;
	uint8_t num_rounds;
	int8_t key_status;
	uint8_t _dummy1;
	uint8_t _dummy2;
	uint8_t expkey[0];
} AES_Context;


#if CPU_REG_BITS == 32

// 32-bit optimized implementation
#include "aes_f32.h"

#else

// Full 8-bit implementation
#include "aes_f8.h"

#endif


/******************************************************************************/

void AES128_init(AES128_Context *aes_)
{
	AES_Context *aes = (AES_Context *)aes_;
	aes->c.set_key = AES_expandKey;
	aes->c.enc_block = AES_encrypt;
	aes->c.dec_block = AES_decrypt;
	aes->c.block_len = Nb*4;
	aes->c.key_len = 16;
	aes->num_rounds = 10;
}

void AES192_init(AES192_Context *aes_)
{
	AES_Context *aes = (AES_Context *)aes_;
	aes->c.set_key = AES_expandKey;
	aes->c.enc_block = AES_encrypt;
	aes->c.dec_block = AES_decrypt;
	aes->c.block_len = Nb*4;
	aes->c.key_len = 24;
	aes->num_rounds = 12;
}

void AES256_init(AES256_Context *aes_)
{
	AES_Context *aes = (AES_Context *)aes_;
	aes->c.set_key = AES_expandKey;
	aes->c.enc_block = AES_encrypt;
	aes->c.dec_block = AES_decrypt;
	aes->c.block_len = Nb*4;
	aes->c.key_len = 32;
	aes->num_rounds = 14;
}

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
 * \brief Generic interface for symmetric block ciphers.
 * \author Giovanni Bajo <rasky@develer.com>
 *
 */

#include "cipher.h"
#include <sec/util.h>

void cipher_cbc_encrypt(BlockCipher *c, void *block)
{
	xor_block(c->buf, c->buf, block, c->block_len);
	c->enc_block(c, c->buf);
	memcpy(block, c->buf, c->block_len);
}

void cipher_cbc_decrypt(BlockCipher *c, void *block)
{
	uint8_t temp[c->block_len];
	memcpy(temp, block, c->block_len);

	c->dec_block(c, block);
	xor_block(block, block, c->buf, c->block_len);

	memcpy(c->buf, temp, c->block_len);
}

static void ctr_increment(void *buf, size_t len)
{
	uint8_t *data = (uint8_t*)buf;
	while (len--)
		if (LIKELY(++data[len] != 0))
			return;
}

void cipher_ctr_step(BlockCipher *c, void *block)
{
	memcpy(block, c->buf, c->block_len);
	c->enc_block(c, block);
	ctr_increment(c->buf, c->block_len);
}

void cipher_ctr_encrypt(BlockCipher *c, void *block)
{
	uint8_t temp[c->block_len];

	cipher_ctr_step(c, temp);
	xor_block(block, block, temp, c->block_len);

	PURGE(temp);
}

void cipher_ctr_decrypt(BlockCipher *c, void *block)
{
	cipher_ctr_encrypt(c, block);
}

static void ofb_step(BlockCipher *c)
{
	c->enc_block(c, c->buf);
}

void cipher_ofb_encrypt(BlockCipher *c, void *block)
{
	ofb_step(c);
	xor_block(block, block, c->buf, c->block_len);
}

void cipher_ofb_decrypt(BlockCipher *c, void *block)
{
	cipher_ofb_encrypt(c, block);
}

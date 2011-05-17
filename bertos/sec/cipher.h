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

#ifndef SEC_CIPHER_H
#define SEC_CIPHER_H

#include <cfg/compiler.h>
#include <cfg/debug.h>

typedef struct BlockCipher
{
	void (*set_key)(struct BlockCipher *c, const void *key, size_t len);
	void (*enc_block)(struct BlockCipher *c, void *block);
	void (*dec_block)(struct BlockCipher *c, void *block);

	void *buf;
	uint8_t key_len;
	uint8_t block_len;
} BlockCipher;


/**
 * Return the key length (in bytes).
 *
 * In case of ciphers that allow a variabile key size with a fixed state
 * (eg: Blowfish), this returns the preferred key length.
 */
INLINE size_t cipher_key_len(BlockCipher *c)
{
	return c->key_len;
}

/**
 * Return the block length (in bytes)
 */
INLINE size_t cipher_block_len(BlockCipher *c)
{
	return c->block_len;
}

/**
 * Set the current key used by the cipher.
 *
 * \note the buffer pointed by \a key is not modified and it is
 * not needed anymore after this call returns. Its lenght must match
 * the value returned by \a cipher_key_len().
 */
INLINE void cipher_set_key(BlockCipher *c, const void *key)
{
	ASSERT(c->set_key);
	c->set_key(c, key, c->key_len);
}

/**
 * Set the current key (of variable size) used by the cipher.
 *
 * This function is useful for ciphers that allow a variable size for the key
 * (even with a fixed state). For all the other ciphers, the length must
 * match the value returned by \a cipher_key_len().
 *
 * \note the buffer pointed by \a key is not modified and it is
 * not needed anymore after this call returns.
 */
INLINE void cipher_set_vkey(BlockCipher *c, const void *key, size_t len)
{
	ASSERT(c->set_key);
	c->set_key(c, key, len);
}

/*********************************************************************************/
/* ECB mode                                                                      */
/*********************************************************************************/

/**
 * Encrypt a block (in-place) using the current key in ECB mode.
 */
INLINE void cipher_ecb_encrypt(BlockCipher *c, void *block)
{
	ASSERT(c->enc_block);
	c->enc_block(c, block);
}

/**
 * Decrypt a block (in-place) using the current key in ECB mode.
 */
INLINE void cipher_ecb_decrypt(BlockCipher *c, void *block)
{
	ASSERT(c->dec_block);
	c->dec_block(c, block);
}


/*********************************************************************************/
/* CBC mode                                                                      */
/*********************************************************************************/

/**
 * Initialize CBC by setting the IV.
 *
 * \note the memory pointed by \a iv will be used and modified by the CBC
 * functions. It is caller's responsibility to keep it available until there is
 * no more CBC work to do.
 */
INLINE void cipher_cbc_begin(BlockCipher *c, void *iv)
{
	c->buf = iv;
}

/**
 * Encrypt a block (in-place) using the current key in CBC mode.
 */
void cipher_cbc_encrypt(BlockCipher *c, void *block);

/**
 * Decrypt a block (in-place) using the current key in CBC mode.
 */
void cipher_cbc_decrypt(BlockCipher *c, void *block);



/*********************************************************************************/
/* CTR mode                                                                      */
/*********************************************************************************/

/**
 * Initialize CTR by setting the counter.
 *
 * \note the memory pointed by \a counter will be used and modified (incremented)
 * by the CTR functions. It is caller's responsibility to keep it available until
 * there is no more CTR work to do.
 */
INLINE void cipher_ctr_begin(BlockCipher *c, void *counter)
{
	c->buf = counter;
}

/**
 * Encrypt a block (in-place) using the current key in CTR mode.
 */
void cipher_ctr_encrypt(BlockCipher *c, void *block);

/**
 * Decrypt a block (in-place) using the current key in CTR mode.
 */
void cipher_ctr_decrypt(BlockCipher *c, void *block);

/**
 * Generate the crypted stream block in CTR mode for the current
 * counter, and then bump it.
 *
 * This function is basically the core CTR operation, without the final
 * XOR pass with the plaintext or ciphertext. For normal CTR usage,
 * you never need to call it.
 */
void cipher_ctr_step(BlockCipher *c, void *block);


/*********************************************************************************/
/* OFB mode                                                                      */
/*********************************************************************************/

/**
 * Initialize OFB by setting the IV.
 *
 * \note the memory pointed by \a iv will be used and modified by the OFB
 * functions. It is caller's responsibility to keep it available until there is
 * no more OFB work to do.
 */
INLINE void cipher_ofb_begin(BlockCipher *c, void *iv)
{
	c->buf = iv;
}

/**
 * Encrypt a block (in-place) using the current key in OFB mode.
 */
void cipher_ofb_encrypt(BlockCipher *c, void *block);

/**
 * Decrypt a block (in-place) using the current key in OFB mode.
 */
void cipher_ofb_decrypt(BlockCipher *c, void *block);


#endif /* SEC_CIPHER_H */

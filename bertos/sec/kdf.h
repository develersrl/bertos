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
 * \brief Generic interface for key derivation functions.
 * \author Giovanni Bajo <rasky@develer.com>
 * 
 */

#ifndef SEC_KDF_H
#define SEC_KDF_H

#include <cfg/compiler.h>
#include <cfg/macros.h>
#include <cfg/debug.h>
#include <string.h>

typedef struct Kdf
{
	const uint8_t* block;
	size_t block_len;
	size_t to_read;
		
	void (*begin)(struct Kdf *kdf, const char *pwd, size_t pwd_len,
				  const uint8_t *salt, size_t salt_len);
	void (*next)(struct Kdf *kdf);
} Kdf;

/**
 * Initialize the key derivation function, with the specified \a password and \a salt.
 */
INLINE void kdf_begin(Kdf *kdf, const char *pwd, size_t pwd_len,
					  const uint8_t *salt, size_t salt_len)
{
	ASSERT(kdf->begin);
	kdf->begin(kdf, pwd, pwd_len, salt, salt_len);
}

/**
 * Extract \a len derived bytes into the \a out buffer.
 * 
 * After having initialized the derivation function iwth \a kdf_begin, you can call
 * this function multiple times, to produce several batches of derived data.
 * 
 * \note Some algorithms implementing this API might have a maximum length of bytes
 * that can be derived, after which they will simply ASSERT().
 */
INLINE void kdf_read(Kdf *kdf, uint8_t *out, size_t len)
{
	do
	{
		if (kdf->to_read == 0)
		{
			kdf->next(kdf);
			ASSERT(kdf->to_read != 0);
		}

		size_t c = MIN(len, kdf->to_read);
		memcpy(out, kdf->block, c);
		len -= c;
		out += c;
		kdf->block += c;
		kdf->to_read -= c;
	} while (len);
}

#endif /* SEC_KDF_H */

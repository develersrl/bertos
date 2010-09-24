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
 * \brief Generic interface for hashing algorithms.
 * \author Giovanni Bajo <rasky@develer.com>
 * 
 */

#ifndef SEC_HASH_H
#define SEC_HASH_H

#include <cfg/compiler.h>
#include <cfg/debug.h>

typedef struct Hash
{
	void (*begin)(struct Hash *h);
	void (*update)(struct Hash *h, const void *data, size_t len);
	uint8_t* (*final)(struct Hash *h);
	uint8_t digest_len;
	uint8_t block_len;
} Hash;

/**
 * Initialize a hash computation.
 */
INLINE void hash_begin(Hash *h)
{
	ASSERT(h->begin);
	h->begin(h);
}

/**
 * Add some data to the computation.
 */
INLINE void hash_update(Hash *h, const void* data, size_t len)
{
	ASSERT(h->update);
	h->update(h, data, len);
}

/**
 * Finalize the hash computation and return the digest.
 * 
 * \note This function must be called exactly once per each computation.
 * Calling it twice leads to undefined behaviour.
 * 
 * \note The pointer returned is within the hash context structure \a h, so it
 * has the same lifetime as the hash instance. The data will be invalidated 
 * as soon as \a hash_begin is called again on the same instance.
 */
INLINE uint8_t* hash_final(Hash *h)
{
	ASSERT(h->final);
	return h->final(h);
}

/**
 * Return the digest length in bytes.
 */
INLINE int hash_digest_len(Hash *h)
{
	return h->digest_len;
}

/*
 * Return the internal block length in bytes.
 * 
 * Hash functions operate on a fixed-size block. This information is useful
 * for composite functions like HMAC to adjust their internal operations.
 */
INLINE int hash_block_len(Hash *h)
{
	return h->block_len;
}

#endif /* SEC_HASH_H */

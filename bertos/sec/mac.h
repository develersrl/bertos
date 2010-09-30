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
 * \brief Generic interface for message authentication algorithms.
 * \author Giovanni Bajo <rasky@develer.com>
 *
 */

#ifndef SEC_MAC_H
#define SEC_MAC_H

#include <cfg/compiler.h>
#include <cfg/debug.h>

typedef struct Mac {
	uint8_t digest_len;
	uint8_t key_len;

	void (*set_key)(struct Mac *m, const void *key, size_t len);
	void (*begin)(struct Mac *m);
	void (*update)(struct Mac *m, const void *data, size_t len);
	uint8_t* (*final)(struct Mac *m);
} Mac;

INLINE void mac_set_key(Mac *m, const uint8_t* key, size_t len)
{
	ASSERT(m->set_key);
	m->set_key(m, key, len);
}

INLINE void mac_begin(Mac *m)
{
	ASSERT(m->begin);
	m->begin(m);
}

INLINE void mac_update(Mac *m, const void *data, size_t len)
{
	ASSERT(m->update);
	m->update(m, data, len);
}

INLINE uint8_t* mac_final(Mac *m)
{
	ASSERT(m->final);
	return m->final(m);
}

INLINE size_t mac_digest_len(Mac *m)
{
	return m->digest_len;
}

INLINE size_t mac_key_len(Mac *m)
{
	return m->key_len;
}

#endif

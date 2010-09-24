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
 * \brief HMAC (RFC 2104) implementation
 * \author Giovanni Bajo <rasky@develer.com>
 * 
 */

#ifndef SEC_MAC_HMAC_H
#define SEC_MAC_HMAC_H

#include <sec/mac.h>
#include <sec/hash.h>

typedef struct HMAC_Context
{
	Mac m;
	Hash *h;	
	uint8_t key[64];
} HMAC_Context;

void HMAC_init(HMAC_Context* hmac, Hash *h);

int HMAC_testSetup(void);
int HMAC_testRun(void);
int HMAC_testTearDown(void);

#endif /* SEC_MAC_HMAC_H */

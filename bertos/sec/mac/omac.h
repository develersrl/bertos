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
 * \brief OMAC implementation
 * \author Giovanni Bajo <rasky@develer.com>
 */

#ifndef SEC_MAC_OMAC_H
#define SEC_MAC_OMAC_H

#include <sec/mac.h>
#include <sec/cipher.h>

typedef struct OmacContext
{
	Mac mac;
	BlockCipher *c;
	uint8_t Y[16];
	uint8_t accum[16];
	uint8_t acc;
} OmacContext;

void omac1_init(OmacContext *ctx, BlockCipher *c);
void omac2_init(OmacContext *ctx, BlockCipher *c);

#define omac1_stackinit(...) \
	({ OmacContext *ctx = alloca(sizeof(OmacContext)); omac1_init(ctx, ##__VA_ARGS__); &ctx->mac; })
#define omac2_stackinit(...) \
	({ OmacContext *ctx = alloca(sizeof(OmacContext)); omac2_init(ctx, ##__VA_ARGS__); &ctx->mac; })

int omac_testSetup(void);
int omac_testTearDown(void);
int omac_testRun(void);

#endif /* SEC_MAC_OMAC_H */

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
 * \brief RIPEMD-160 Hashing algorithm.
 * \author Giovanni Bajo <rasky@develer.com>
 */

/*
 *
 *  RIPEMD160.c : RIPEMD-160 implementation
 *
 * Written in 2008 by Dwayne C. Litzenberger <dlitz@dlitz.net>
 *
 * ===================================================================
 * The contents of this file are dedicated to the public domain.  To
 * the extent that dedication to the public domain is not available,
 * everyone is granted a worldwide, perpetual, royalty-free,
 * non-exclusive license to exercise all rights associated with the
 * contents of this file for any purpose whatsoever.
 * No rights are reserved.
 * ===================================================================
 */

#include "ripemd.h"
#include <cfg/debug.h>
#include <cfg/compiler.h>
#include <cpu/byteorder.h>
#include <string.h>

#define RIPEMD160_DIGEST_SIZE   20


/* cyclic left-shift the 32-bit word n left by s bits */
#define ROL(s, n)  ROTL(n, s)

/* Initial values for the chaining variables.
 * This is just 0123456789ABCDEFFEDCBA9876543210F0E1D2C3 in little-endian. */
static const uint32_t initial_h[5] = { 0x67452301u, 0xEFCDAB89u, 0x98BADCFEu, 0x10325476u, 0xC3D2E1F0u };

/* Ordering of message words.  Based on the permutations rho(i) and pi(i), defined as follows:
 *
 *  rho(i) := { 7, 4, 13, 1, 10, 6, 15, 3, 12, 0, 9, 5, 2, 14, 11, 8 }[i]  0 <= i <= 15
 *
 *  pi(i) := 9*i + 5 (mod 16)
 *
 *  Line  |  Round 1  |  Round 2  |  Round 3  |  Round 4  |  Round 5
 * -------+-----------+-----------+-----------+-----------+-----------
 *  left  |    id     |    rho    |   rho^2   |   rho^3   |   rho^4
 *  right |    pi     |   rho pi  |  rho^2 pi |  rho^3 pi |  rho^4 pi
 */

/* Left line */
static const uint8_t RL[5][16] = {
    { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 },   /* Round 1: id */
    { 7, 4, 13, 1, 10, 6, 15, 3, 12, 0, 9, 5, 2, 14, 11, 8 },   /* Round 2: rho */
    { 3, 10, 14, 4, 9, 15, 8, 1, 2, 7, 0, 6, 13, 11, 5, 12 },   /* Round 3: rho^2 */
    { 1, 9, 11, 10, 0, 8, 12, 4, 13, 3, 7, 15, 14, 5, 6, 2 },   /* Round 4: rho^3 */
    { 4, 0, 5, 9, 7, 12, 2, 10, 14, 1, 3, 8, 11, 6, 15, 13 }    /* Round 5: rho^4 */
};

/* Right line */
static const uint8_t RR[5][16] = {
    { 5, 14, 7, 0, 9, 2, 11, 4, 13, 6, 15, 8, 1, 10, 3, 12 },   /* Round 1: pi */
    { 6, 11, 3, 7, 0, 13, 5, 10, 14, 15, 8, 12, 4, 9, 1, 2 },   /* Round 2: rho pi */
    { 15, 5, 1, 3, 7, 14, 6, 9, 11, 8, 12, 2, 10, 0, 4, 13 },   /* Round 3: rho^2 pi */
    { 8, 6, 4, 1, 3, 11, 15, 0, 5, 12, 2, 13, 9, 7, 10, 14 },   /* Round 4: rho^3 pi */
    { 12, 15, 10, 4, 1, 5, 8, 7, 6, 2, 13, 14, 0, 3, 9, 11 }    /* Round 5: rho^4 pi */
};

/*
 * Shifts - Since we don't actually re-order the message words according to
 * the permutations above (we could, but it would be slower), these tables
 * come with the permutations pre-applied.
 */

/* Shifts, left line */
static const uint8_t SL[5][16] = {
    { 11, 14, 15, 12, 5, 8, 7, 9, 11, 13, 14, 15, 6, 7, 9, 8 }, /* Round 1 */
    { 7, 6, 8, 13, 11, 9, 7, 15, 7, 12, 15, 9, 11, 7, 13, 12 }, /* Round 2 */
    { 11, 13, 6, 7, 14, 9, 13, 15, 14, 8, 13, 6, 5, 12, 7, 5 }, /* Round 3 */
    { 11, 12, 14, 15, 14, 15, 9, 8, 9, 14, 5, 6, 8, 6, 5, 12 }, /* Round 4 */
    { 9, 15, 5, 11, 6, 8, 13, 12, 5, 12, 13, 14, 11, 8, 5, 6 }  /* Round 5 */
};

/* Shifts, right line */
static const uint8_t SR[5][16] = {
    { 8, 9, 9, 11, 13, 15, 15, 5, 7, 7, 8, 11, 14, 14, 12, 6 }, /* Round 1 */
    { 9, 13, 15, 7, 12, 8, 9, 11, 7, 7, 12, 7, 6, 15, 13, 11 }, /* Round 2 */
    { 9, 7, 15, 11, 8, 6, 6, 14, 12, 13, 5, 14, 13, 13, 7, 5 }, /* Round 3 */
    { 15, 5, 8, 11, 14, 14, 6, 14, 6, 9, 12, 9, 12, 5, 15, 8 }, /* Round 4 */
    { 8, 5, 12, 9, 12, 5, 14, 6, 8, 13, 6, 5, 15, 13, 11, 11 }  /* Round 5 */
};

/* Boolean functions */

#define F1(x, y, z) ((x) ^ (y) ^ (z))
#define F2(x, y, z) (((x) & (y)) | (~(x) & (z)))
#define F3(x, y, z) (((x) | ~(y)) ^ (z))
#define F4(x, y, z) (((x) & (z)) | ((y) & ~(z)))
#define F5(x, y, z) ((x) ^ ((y) | ~(z)))

/* Round constants, left line */
static const uint32_t KL[5] = {
    0x00000000u,    /* Round 1: 0 */
    0x5A827999u,    /* Round 2: floor(2**30 * sqrt(2)) */
    0x6ED9EBA1u,    /* Round 3: floor(2**30 * sqrt(3)) */
    0x8F1BBCDCu,    /* Round 4: floor(2**30 * sqrt(5)) */
    0xA953FD4Eu     /* Round 5: floor(2**30 * sqrt(7)) */
};

/* Round constants, right line */
static const uint32_t KR[5] = {
    0x50A28BE6u,    /* Round 1: floor(2**30 * cubert(2)) */
    0x5C4DD124u,    /* Round 2: floor(2**30 * cubert(3)) */
    0x6D703EF3u,    /* Round 3: floor(2**30 * cubert(5)) */
    0x7A6D76E9u,    /* Round 4: floor(2**30 * cubert(7)) */
    0x00000000u     /* Round 5: 0 */
};

static void ripemd160_init(Hash *h)
{
	RIPEMD_Context *self = (RIPEMD_Context *)h;
	
    memcpy(self->h, initial_h, RIPEMD160_DIGEST_SIZE);
    memset(&self->buf, 0, sizeof(self->buf));
    self->length = 0;
    self->bufpos = 0;
}

static inline void byteswap_digest(uint32_t *p)
{
    unsigned int i;

    for (i = 0; i < 4; i++) {
        p[0] = SWAB32(p[0]);
        p[1] = SWAB32(p[1]);
        p[2] = SWAB32(p[2]);
        p[3] = SWAB32(p[3]);
		p += 4;
    }
}

/* The RIPEMD160 compression function.  Operates on self->buf */
static void ripemd160_compress(RIPEMD_Context *self)
{
    uint8_t w, round;
    uint32_t T;
    uint32_t AL, BL, CL, DL, EL;    /* left line */
    uint32_t AR, BR, CR, DR, ER;    /* right line */

    /* Sanity check */
    ASSERT(self->bufpos == 64);

    /* Byte-swap the buffer if we're on a big-endian machine */
#if CPU_BYTE_ORDER == CPU_BIG_ENDIAN
    byteswap_digest(self->buf.w);
#endif

    /* Load the left and right lines with the initial state */
    AL = AR = self->h[0];
    BL = BR = self->h[1];
    CL = CR = self->h[2];
    DL = DR = self->h[3];
    EL = ER = self->h[4];

    /* Round 1 */
    round = 0;
    for (w = 0; w < 16; w++) { /* left line */
        T = ROL(SL[round][w], AL + F1(BL, CL, DL) + self->buf.w[RL[round][w]] + KL[round]) + EL;
        AL = EL; EL = DL; DL = ROL(10, CL); CL = BL; BL = T;
    }
    for (w = 0; w < 16; w++) { /* right line */
        T = ROL(SR[round][w], AR + F5(BR, CR, DR) + self->buf.w[RR[round][w]] + KR[round]) + ER;
        AR = ER; ER = DR; DR = ROL(10, CR); CR = BR; BR = T;
    }

    /* Round 2 */
    round++;
    for (w = 0; w < 16; w++) { /* left line */
        T = ROL(SL[round][w], AL + F2(BL, CL, DL) + self->buf.w[RL[round][w]] + KL[round]) + EL;
        AL = EL; EL = DL; DL = ROL(10, CL); CL = BL; BL = T;
    }
    for (w = 0; w < 16; w++) { /* right line */
        T = ROL(SR[round][w], AR + F4(BR, CR, DR) + self->buf.w[RR[round][w]] + KR[round]) + ER;
        AR = ER; ER = DR; DR = ROL(10, CR); CR = BR; BR = T;
    }

    /* Round 3 */
    round++;
    for (w = 0; w < 16; w++) { /* left line */
        T = ROL(SL[round][w], AL + F3(BL, CL, DL) + self->buf.w[RL[round][w]] + KL[round]) + EL;
        AL = EL; EL = DL; DL = ROL(10, CL); CL = BL; BL = T;
    }
    for (w = 0; w < 16; w++) { /* right line */
        T = ROL(SR[round][w], AR + F3(BR, CR, DR) + self->buf.w[RR[round][w]] + KR[round]) + ER;
        AR = ER; ER = DR; DR = ROL(10, CR); CR = BR; BR = T;
    }

    /* Round 4 */
    round++;
    for (w = 0; w < 16; w++) { /* left line */
        T = ROL(SL[round][w], AL + F4(BL, CL, DL) + self->buf.w[RL[round][w]] + KL[round]) + EL;
        AL = EL; EL = DL; DL = ROL(10, CL); CL = BL; BL = T;
    }
    for (w = 0; w < 16; w++) { /* right line */
        T = ROL(SR[round][w], AR + F2(BR, CR, DR) + self->buf.w[RR[round][w]] + KR[round]) + ER;
        AR = ER; ER = DR; DR = ROL(10, CR); CR = BR; BR = T;
    }

    /* Round 5 */
    round++;
    for (w = 0; w < 16; w++) { /* left line */
        T = ROL(SL[round][w], AL + F5(BL, CL, DL) + self->buf.w[RL[round][w]] + KL[round]) + EL;
        AL = EL; EL = DL; DL = ROL(10, CL); CL = BL; BL = T;
    }
    for (w = 0; w < 16; w++) { /* right line */
        T = ROL(SR[round][w], AR + F1(BR, CR, DR) + self->buf.w[RR[round][w]] + KR[round]) + ER;
        AR = ER; ER = DR; DR = ROL(10, CR); CR = BR; BR = T;
    }

    /* Final mixing stage */
    T = self->h[1] + CL + DR;
    self->h[1] = self->h[2] + DL + ER;
    self->h[2] = self->h[3] + EL + AR;
    self->h[3] = self->h[4] + AL + BR;
    self->h[4] = self->h[0] + BL + CR;
    self->h[0] = T;

    /* Clear the buffer and wipe the temporary variables */
    T = AL = BL = CL = DL = EL = AR = BR = CR = DR = ER = 0;
    memset(&self->buf, 0, sizeof(self->buf));
    self->bufpos = 0;
}

static void ripemd160_update(Hash *h, const void *data, size_t length)
{
	RIPEMD_Context *self = (RIPEMD_Context *)h;
	const uint8_t *p = (const uint8_t *)data;
    unsigned int bytes_needed;

    /* Some assertions */
    ASSERT(p != NULL);

    /* We never leave a full buffer */
    ASSERT(self->bufpos < 64);

    while (length > 0) {
        /* Figure out how many bytes we need to fill the internal buffer. */
        bytes_needed = 64 - self->bufpos;

        if (length >= bytes_needed) {
            /* We have enough bytes, so copy them into the internal buffer and run
             * the compression function. */
            memcpy(&self->buf.b[self->bufpos], p, bytes_needed);
            self->bufpos += bytes_needed;
            self->length += bytes_needed << 3;    /* length is in bits */
            p += bytes_needed;
            ripemd160_compress(self);
            length -= bytes_needed;
            continue;
        }

        /* We do not have enough bytes to fill the internal buffer.
         * Copy what's there and return. */
        memcpy(&self->buf.b[self->bufpos], p, length);
        self->bufpos += length;
        self->length += length << 3;    /* length is in bits */
        return;
    }
}

static uint8_t* ripemd160_digest(Hash *h)
{
	RIPEMD_Context *self = (RIPEMD_Context *)h;
	
    /* Append the padding */
    self->buf.b[self->bufpos++] = 0x80;

    if (self->bufpos > 56) {
        self->bufpos = 64;
        ripemd160_compress(self);
    }

    /* Append the length */
    self->buf.w[14] = cpu_to_le32((uint32_t)(self->length & 0xFFFFffffu));
    self->buf.w[15] = cpu_to_le32((uint32_t)((self->length >> 32) & 0xFFFFffffu));

	self->bufpos = 64;
    ripemd160_compress(self);

    /* Copy the final state into the output buffer */
#if CPU_BYTE_ORDER == CPU_BIG_ENDIAN
    byteswap_digest(self->h);
#endif

	return (uint8_t*)&self->h;
}

/**************************************************************************************/


void RIPEMD_init(RIPEMD_Context *ctx)
{
	ctx->hash.begin = ripemd160_init;
	ctx->hash.update = ripemd160_update;
	ctx->hash.final = ripemd160_digest;
	ctx->hash.digest_len = RIPEMD160_DIGEST_SIZE;
	ctx->hash.block_len = 64;
}

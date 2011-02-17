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
 */

/*****************************************************************************/
// ENCRYPTION TABLES
/*****************************************************************************/

static const uint32_t TE0[256] =
{
    be32_to_cpu(0xc66363a5UL), be32_to_cpu(0xf87c7c84UL), be32_to_cpu(0xee777799UL), be32_to_cpu(0xf67b7b8dUL),
    be32_to_cpu(0xfff2f20dUL), be32_to_cpu(0xd66b6bbdUL), be32_to_cpu(0xde6f6fb1UL), be32_to_cpu(0x91c5c554UL),
    be32_to_cpu(0x60303050UL), be32_to_cpu(0x02010103UL), be32_to_cpu(0xce6767a9UL), be32_to_cpu(0x562b2b7dUL),
    be32_to_cpu(0xe7fefe19UL), be32_to_cpu(0xb5d7d762UL), be32_to_cpu(0x4dababe6UL), be32_to_cpu(0xec76769aUL),
    be32_to_cpu(0x8fcaca45UL), be32_to_cpu(0x1f82829dUL), be32_to_cpu(0x89c9c940UL), be32_to_cpu(0xfa7d7d87UL),
    be32_to_cpu(0xeffafa15UL), be32_to_cpu(0xb25959ebUL), be32_to_cpu(0x8e4747c9UL), be32_to_cpu(0xfbf0f00bUL),
    be32_to_cpu(0x41adadecUL), be32_to_cpu(0xb3d4d467UL), be32_to_cpu(0x5fa2a2fdUL), be32_to_cpu(0x45afafeaUL),
    be32_to_cpu(0x239c9cbfUL), be32_to_cpu(0x53a4a4f7UL), be32_to_cpu(0xe4727296UL), be32_to_cpu(0x9bc0c05bUL),
    be32_to_cpu(0x75b7b7c2UL), be32_to_cpu(0xe1fdfd1cUL), be32_to_cpu(0x3d9393aeUL), be32_to_cpu(0x4c26266aUL),
    be32_to_cpu(0x6c36365aUL), be32_to_cpu(0x7e3f3f41UL), be32_to_cpu(0xf5f7f702UL), be32_to_cpu(0x83cccc4fUL),
    be32_to_cpu(0x6834345cUL), be32_to_cpu(0x51a5a5f4UL), be32_to_cpu(0xd1e5e534UL), be32_to_cpu(0xf9f1f108UL),
    be32_to_cpu(0xe2717193UL), be32_to_cpu(0xabd8d873UL), be32_to_cpu(0x62313153UL), be32_to_cpu(0x2a15153fUL),
    be32_to_cpu(0x0804040cUL), be32_to_cpu(0x95c7c752UL), be32_to_cpu(0x46232365UL), be32_to_cpu(0x9dc3c35eUL),
    be32_to_cpu(0x30181828UL), be32_to_cpu(0x379696a1UL), be32_to_cpu(0x0a05050fUL), be32_to_cpu(0x2f9a9ab5UL),
    be32_to_cpu(0x0e070709UL), be32_to_cpu(0x24121236UL), be32_to_cpu(0x1b80809bUL), be32_to_cpu(0xdfe2e23dUL),
    be32_to_cpu(0xcdebeb26UL), be32_to_cpu(0x4e272769UL), be32_to_cpu(0x7fb2b2cdUL), be32_to_cpu(0xea75759fUL),
    be32_to_cpu(0x1209091bUL), be32_to_cpu(0x1d83839eUL), be32_to_cpu(0x582c2c74UL), be32_to_cpu(0x341a1a2eUL),
    be32_to_cpu(0x361b1b2dUL), be32_to_cpu(0xdc6e6eb2UL), be32_to_cpu(0xb45a5aeeUL), be32_to_cpu(0x5ba0a0fbUL),
    be32_to_cpu(0xa45252f6UL), be32_to_cpu(0x763b3b4dUL), be32_to_cpu(0xb7d6d661UL), be32_to_cpu(0x7db3b3ceUL),
    be32_to_cpu(0x5229297bUL), be32_to_cpu(0xdde3e33eUL), be32_to_cpu(0x5e2f2f71UL), be32_to_cpu(0x13848497UL),
    be32_to_cpu(0xa65353f5UL), be32_to_cpu(0xb9d1d168UL), be32_to_cpu(0x00000000UL), be32_to_cpu(0xc1eded2cUL),
    be32_to_cpu(0x40202060UL), be32_to_cpu(0xe3fcfc1fUL), be32_to_cpu(0x79b1b1c8UL), be32_to_cpu(0xb65b5bedUL),
    be32_to_cpu(0xd46a6abeUL), be32_to_cpu(0x8dcbcb46UL), be32_to_cpu(0x67bebed9UL), be32_to_cpu(0x7239394bUL),
    be32_to_cpu(0x944a4adeUL), be32_to_cpu(0x984c4cd4UL), be32_to_cpu(0xb05858e8UL), be32_to_cpu(0x85cfcf4aUL),
    be32_to_cpu(0xbbd0d06bUL), be32_to_cpu(0xc5efef2aUL), be32_to_cpu(0x4faaaae5UL), be32_to_cpu(0xedfbfb16UL),
    be32_to_cpu(0x864343c5UL), be32_to_cpu(0x9a4d4dd7UL), be32_to_cpu(0x66333355UL), be32_to_cpu(0x11858594UL),
    be32_to_cpu(0x8a4545cfUL), be32_to_cpu(0xe9f9f910UL), be32_to_cpu(0x04020206UL), be32_to_cpu(0xfe7f7f81UL),
    be32_to_cpu(0xa05050f0UL), be32_to_cpu(0x783c3c44UL), be32_to_cpu(0x259f9fbaUL), be32_to_cpu(0x4ba8a8e3UL),
    be32_to_cpu(0xa25151f3UL), be32_to_cpu(0x5da3a3feUL), be32_to_cpu(0x804040c0UL), be32_to_cpu(0x058f8f8aUL),
    be32_to_cpu(0x3f9292adUL), be32_to_cpu(0x219d9dbcUL), be32_to_cpu(0x70383848UL), be32_to_cpu(0xf1f5f504UL),
    be32_to_cpu(0x63bcbcdfUL), be32_to_cpu(0x77b6b6c1UL), be32_to_cpu(0xafdada75UL), be32_to_cpu(0x42212163UL),
    be32_to_cpu(0x20101030UL), be32_to_cpu(0xe5ffff1aUL), be32_to_cpu(0xfdf3f30eUL), be32_to_cpu(0xbfd2d26dUL),
    be32_to_cpu(0x81cdcd4cUL), be32_to_cpu(0x180c0c14UL), be32_to_cpu(0x26131335UL), be32_to_cpu(0xc3ecec2fUL),
    be32_to_cpu(0xbe5f5fe1UL), be32_to_cpu(0x359797a2UL), be32_to_cpu(0x884444ccUL), be32_to_cpu(0x2e171739UL),
    be32_to_cpu(0x93c4c457UL), be32_to_cpu(0x55a7a7f2UL), be32_to_cpu(0xfc7e7e82UL), be32_to_cpu(0x7a3d3d47UL),
    be32_to_cpu(0xc86464acUL), be32_to_cpu(0xba5d5de7UL), be32_to_cpu(0x3219192bUL), be32_to_cpu(0xe6737395UL),
    be32_to_cpu(0xc06060a0UL), be32_to_cpu(0x19818198UL), be32_to_cpu(0x9e4f4fd1UL), be32_to_cpu(0xa3dcdc7fUL),
    be32_to_cpu(0x44222266UL), be32_to_cpu(0x542a2a7eUL), be32_to_cpu(0x3b9090abUL), be32_to_cpu(0x0b888883UL),
    be32_to_cpu(0x8c4646caUL), be32_to_cpu(0xc7eeee29UL), be32_to_cpu(0x6bb8b8d3UL), be32_to_cpu(0x2814143cUL),
    be32_to_cpu(0xa7dede79UL), be32_to_cpu(0xbc5e5ee2UL), be32_to_cpu(0x160b0b1dUL), be32_to_cpu(0xaddbdb76UL),
    be32_to_cpu(0xdbe0e03bUL), be32_to_cpu(0x64323256UL), be32_to_cpu(0x743a3a4eUL), be32_to_cpu(0x140a0a1eUL),
    be32_to_cpu(0x924949dbUL), be32_to_cpu(0x0c06060aUL), be32_to_cpu(0x4824246cUL), be32_to_cpu(0xb85c5ce4UL),
    be32_to_cpu(0x9fc2c25dUL), be32_to_cpu(0xbdd3d36eUL), be32_to_cpu(0x43acacefUL), be32_to_cpu(0xc46262a6UL),
    be32_to_cpu(0x399191a8UL), be32_to_cpu(0x319595a4UL), be32_to_cpu(0xd3e4e437UL), be32_to_cpu(0xf279798bUL),
    be32_to_cpu(0xd5e7e732UL), be32_to_cpu(0x8bc8c843UL), be32_to_cpu(0x6e373759UL), be32_to_cpu(0xda6d6db7UL),
    be32_to_cpu(0x018d8d8cUL), be32_to_cpu(0xb1d5d564UL), be32_to_cpu(0x9c4e4ed2UL), be32_to_cpu(0x49a9a9e0UL),
    be32_to_cpu(0xd86c6cb4UL), be32_to_cpu(0xac5656faUL), be32_to_cpu(0xf3f4f407UL), be32_to_cpu(0xcfeaea25UL),
    be32_to_cpu(0xca6565afUL), be32_to_cpu(0xf47a7a8eUL), be32_to_cpu(0x47aeaee9UL), be32_to_cpu(0x10080818UL),
    be32_to_cpu(0x6fbabad5UL), be32_to_cpu(0xf0787888UL), be32_to_cpu(0x4a25256fUL), be32_to_cpu(0x5c2e2e72UL),
    be32_to_cpu(0x381c1c24UL), be32_to_cpu(0x57a6a6f1UL), be32_to_cpu(0x73b4b4c7UL), be32_to_cpu(0x97c6c651UL),
    be32_to_cpu(0xcbe8e823UL), be32_to_cpu(0xa1dddd7cUL), be32_to_cpu(0xe874749cUL), be32_to_cpu(0x3e1f1f21UL),
    be32_to_cpu(0x964b4bddUL), be32_to_cpu(0x61bdbddcUL), be32_to_cpu(0x0d8b8b86UL), be32_to_cpu(0x0f8a8a85UL),
    be32_to_cpu(0xe0707090UL), be32_to_cpu(0x7c3e3e42UL), be32_to_cpu(0x71b5b5c4UL), be32_to_cpu(0xcc6666aaUL),
    be32_to_cpu(0x904848d8UL), be32_to_cpu(0x06030305UL), be32_to_cpu(0xf7f6f601UL), be32_to_cpu(0x1c0e0e12UL),
    be32_to_cpu(0xc26161a3UL), be32_to_cpu(0x6a35355fUL), be32_to_cpu(0xae5757f9UL), be32_to_cpu(0x69b9b9d0UL),
    be32_to_cpu(0x17868691UL), be32_to_cpu(0x99c1c158UL), be32_to_cpu(0x3a1d1d27UL), be32_to_cpu(0x279e9eb9UL),
    be32_to_cpu(0xd9e1e138UL), be32_to_cpu(0xebf8f813UL), be32_to_cpu(0x2b9898b3UL), be32_to_cpu(0x22111133UL),
    be32_to_cpu(0xd26969bbUL), be32_to_cpu(0xa9d9d970UL), be32_to_cpu(0x078e8e89UL), be32_to_cpu(0x339494a7UL),
    be32_to_cpu(0x2d9b9bb6UL), be32_to_cpu(0x3c1e1e22UL), be32_to_cpu(0x15878792UL), be32_to_cpu(0xc9e9e920UL),
    be32_to_cpu(0x87cece49UL), be32_to_cpu(0xaa5555ffUL), be32_to_cpu(0x50282878UL), be32_to_cpu(0xa5dfdf7aUL),
    be32_to_cpu(0x038c8c8fUL), be32_to_cpu(0x59a1a1f8UL), be32_to_cpu(0x09898980UL), be32_to_cpu(0x1a0d0d17UL),
    be32_to_cpu(0x65bfbfdaUL), be32_to_cpu(0xd7e6e631UL), be32_to_cpu(0x844242c6UL), be32_to_cpu(0xd06868b8UL),
    be32_to_cpu(0x824141c3UL), be32_to_cpu(0x299999b0UL), be32_to_cpu(0x5a2d2d77UL), be32_to_cpu(0x1e0f0f11UL),
    be32_to_cpu(0x7bb0b0cbUL), be32_to_cpu(0xa85454fcUL), be32_to_cpu(0x6dbbbbd6UL), be32_to_cpu(0x2c16163aUL),
};

static const uint8_t TE4[256] =
{
	0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5,
	0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
	0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0,
	0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
	0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc,
	0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
	0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a,
	0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
	0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0,
	0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
	0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b,
	0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
	0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85,
	0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
	0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5,
	0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
	0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17,
	0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
	0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88,
	0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
	0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c,
	0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
	0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9,
	0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
	0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6,
	0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
	0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e,
	0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
	0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94,
	0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
	0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68,
	0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
};

static const uint8_t rcon[10] =
{
	0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36,
	/* for 128-bit blocks, Rijndael never uses more than 10 rcon values */
};

#if CPU_BYTE_ORDER == CPU_LITTLE_ENDIAN

#define Te0(x) TE0[x&0xFF]
#define Te1(x) ROTL(TE0[(x>>8)&0xFF], 8)
#define Te2(x) ROTL(TE0[(x>>16)&0xFF], 16)
#define Te3(x) ROTL(TE0[x>>24], 24)

#define Te4_0(x)  ((uint32_t)TE4[x>>24] << 24)
#define Te4_1(x)  ((uint32_t)TE4[(x>>16)&0xFF] << 16)
#define Te4_2(x)  ((uint32_t)TE4[(x>>8)&0xFF] << 8)
#define Te4_3(x)  ((uint32_t)TE4[x&0xFF])

#define RCON(x)   ((uint32_t)rcon[x])

#define TeKeyMix(x)  \
	(((uint32_t)TE4[(x>>8)&0xFF]) ^ \
    ((uint32_t)TE4[(x>>16)&0xFF] << 8) ^ \
    ((uint32_t)TE4[x>>24] << 16) ^ \
    ((uint32_t)TE4[x&0xFF]) << 24)

#define TeKeyMix2(x) TeKeyMix(ROTL(x, 8))

#else

#define Te0(x) TE0[x>>24]
#define Te1(x) ROTR(TE0[(x>>16)&0xFF], 8)
#define Te2(x) ROTR(TE0[(x>>8)&0xFF], 16)
#define Te3(x) ROTR(TE0[x&0xFF], 24)

#define Te4_0(x)  ((uint32_t)TE4[x&0xFF])
#define Te4_1(x)  ((uint32_t)TE4[(x>>8)&0xFF] << 8)
#define Te4_2(x)  ((uint32_t)TE4[(x>>16)&0xFF] << 16)
#define Te4_3(x)  ((uint32_t)TE4[x>>24] << 24)

#define TeKeyMix(x)  \
	((TE4[(x>>16)&0xFF] << 24) ^ \
    (TE4[(x>>8)&0xFF] << 16) ^ \
    (TE4[x&0xFF] << 8) ^ \
    (TE4[x>>24]))

#define TeKeyMix2(x) TeKeyMix(ROTR(x, 8))

#define RCON(x)   (((uint32_t)rcon[x])<<24)

#endif


/*****************************************************************************/
// DECRYPTION TABLES
/*****************************************************************************/

static const uint32_t TD0[256] =
{
    be32_to_cpu(0x51f4a750UL), be32_to_cpu(0x7e416553UL), be32_to_cpu(0x1a17a4c3UL), be32_to_cpu(0x3a275e96UL),
    be32_to_cpu(0x3bab6bcbUL), be32_to_cpu(0x1f9d45f1UL), be32_to_cpu(0xacfa58abUL), be32_to_cpu(0x4be30393UL),
    be32_to_cpu(0x2030fa55UL), be32_to_cpu(0xad766df6UL), be32_to_cpu(0x88cc7691UL), be32_to_cpu(0xf5024c25UL),
    be32_to_cpu(0x4fe5d7fcUL), be32_to_cpu(0xc52acbd7UL), be32_to_cpu(0x26354480UL), be32_to_cpu(0xb562a38fUL),
    be32_to_cpu(0xdeb15a49UL), be32_to_cpu(0x25ba1b67UL), be32_to_cpu(0x45ea0e98UL), be32_to_cpu(0x5dfec0e1UL),
    be32_to_cpu(0xc32f7502UL), be32_to_cpu(0x814cf012UL), be32_to_cpu(0x8d4697a3UL), be32_to_cpu(0x6bd3f9c6UL),
    be32_to_cpu(0x038f5fe7UL), be32_to_cpu(0x15929c95UL), be32_to_cpu(0xbf6d7aebUL), be32_to_cpu(0x955259daUL),
    be32_to_cpu(0xd4be832dUL), be32_to_cpu(0x587421d3UL), be32_to_cpu(0x49e06929UL), be32_to_cpu(0x8ec9c844UL),
    be32_to_cpu(0x75c2896aUL), be32_to_cpu(0xf48e7978UL), be32_to_cpu(0x99583e6bUL), be32_to_cpu(0x27b971ddUL),
    be32_to_cpu(0xbee14fb6UL), be32_to_cpu(0xf088ad17UL), be32_to_cpu(0xc920ac66UL), be32_to_cpu(0x7dce3ab4UL),
    be32_to_cpu(0x63df4a18UL), be32_to_cpu(0xe51a3182UL), be32_to_cpu(0x97513360UL), be32_to_cpu(0x62537f45UL),
    be32_to_cpu(0xb16477e0UL), be32_to_cpu(0xbb6bae84UL), be32_to_cpu(0xfe81a01cUL), be32_to_cpu(0xf9082b94UL),
    be32_to_cpu(0x70486858UL), be32_to_cpu(0x8f45fd19UL), be32_to_cpu(0x94de6c87UL), be32_to_cpu(0x527bf8b7UL),
    be32_to_cpu(0xab73d323UL), be32_to_cpu(0x724b02e2UL), be32_to_cpu(0xe31f8f57UL), be32_to_cpu(0x6655ab2aUL),
    be32_to_cpu(0xb2eb2807UL), be32_to_cpu(0x2fb5c203UL), be32_to_cpu(0x86c57b9aUL), be32_to_cpu(0xd33708a5UL),
    be32_to_cpu(0x302887f2UL), be32_to_cpu(0x23bfa5b2UL), be32_to_cpu(0x02036abaUL), be32_to_cpu(0xed16825cUL),
    be32_to_cpu(0x8acf1c2bUL), be32_to_cpu(0xa779b492UL), be32_to_cpu(0xf307f2f0UL), be32_to_cpu(0x4e69e2a1UL),
    be32_to_cpu(0x65daf4cdUL), be32_to_cpu(0x0605bed5UL), be32_to_cpu(0xd134621fUL), be32_to_cpu(0xc4a6fe8aUL),
    be32_to_cpu(0x342e539dUL), be32_to_cpu(0xa2f355a0UL), be32_to_cpu(0x058ae132UL), be32_to_cpu(0xa4f6eb75UL),
    be32_to_cpu(0x0b83ec39UL), be32_to_cpu(0x4060efaaUL), be32_to_cpu(0x5e719f06UL), be32_to_cpu(0xbd6e1051UL),
    be32_to_cpu(0x3e218af9UL), be32_to_cpu(0x96dd063dUL), be32_to_cpu(0xdd3e05aeUL), be32_to_cpu(0x4de6bd46UL),
    be32_to_cpu(0x91548db5UL), be32_to_cpu(0x71c45d05UL), be32_to_cpu(0x0406d46fUL), be32_to_cpu(0x605015ffUL),
    be32_to_cpu(0x1998fb24UL), be32_to_cpu(0xd6bde997UL), be32_to_cpu(0x894043ccUL), be32_to_cpu(0x67d99e77UL),
    be32_to_cpu(0xb0e842bdUL), be32_to_cpu(0x07898b88UL), be32_to_cpu(0xe7195b38UL), be32_to_cpu(0x79c8eedbUL),
    be32_to_cpu(0xa17c0a47UL), be32_to_cpu(0x7c420fe9UL), be32_to_cpu(0xf8841ec9UL), be32_to_cpu(0x00000000UL),
    be32_to_cpu(0x09808683UL), be32_to_cpu(0x322bed48UL), be32_to_cpu(0x1e1170acUL), be32_to_cpu(0x6c5a724eUL),
    be32_to_cpu(0xfd0efffbUL), be32_to_cpu(0x0f853856UL), be32_to_cpu(0x3daed51eUL), be32_to_cpu(0x362d3927UL),
    be32_to_cpu(0x0a0fd964UL), be32_to_cpu(0x685ca621UL), be32_to_cpu(0x9b5b54d1UL), be32_to_cpu(0x24362e3aUL),
    be32_to_cpu(0x0c0a67b1UL), be32_to_cpu(0x9357e70fUL), be32_to_cpu(0xb4ee96d2UL), be32_to_cpu(0x1b9b919eUL),
    be32_to_cpu(0x80c0c54fUL), be32_to_cpu(0x61dc20a2UL), be32_to_cpu(0x5a774b69UL), be32_to_cpu(0x1c121a16UL),
    be32_to_cpu(0xe293ba0aUL), be32_to_cpu(0xc0a02ae5UL), be32_to_cpu(0x3c22e043UL), be32_to_cpu(0x121b171dUL),
    be32_to_cpu(0x0e090d0bUL), be32_to_cpu(0xf28bc7adUL), be32_to_cpu(0x2db6a8b9UL), be32_to_cpu(0x141ea9c8UL),
    be32_to_cpu(0x57f11985UL), be32_to_cpu(0xaf75074cUL), be32_to_cpu(0xee99ddbbUL), be32_to_cpu(0xa37f60fdUL),
    be32_to_cpu(0xf701269fUL), be32_to_cpu(0x5c72f5bcUL), be32_to_cpu(0x44663bc5UL), be32_to_cpu(0x5bfb7e34UL),
    be32_to_cpu(0x8b432976UL), be32_to_cpu(0xcb23c6dcUL), be32_to_cpu(0xb6edfc68UL), be32_to_cpu(0xb8e4f163UL),
    be32_to_cpu(0xd731dccaUL), be32_to_cpu(0x42638510UL), be32_to_cpu(0x13972240UL), be32_to_cpu(0x84c61120UL),
    be32_to_cpu(0x854a247dUL), be32_to_cpu(0xd2bb3df8UL), be32_to_cpu(0xaef93211UL), be32_to_cpu(0xc729a16dUL),
    be32_to_cpu(0x1d9e2f4bUL), be32_to_cpu(0xdcb230f3UL), be32_to_cpu(0x0d8652ecUL), be32_to_cpu(0x77c1e3d0UL),
    be32_to_cpu(0x2bb3166cUL), be32_to_cpu(0xa970b999UL), be32_to_cpu(0x119448faUL), be32_to_cpu(0x47e96422UL),
    be32_to_cpu(0xa8fc8cc4UL), be32_to_cpu(0xa0f03f1aUL), be32_to_cpu(0x567d2cd8UL), be32_to_cpu(0x223390efUL),
    be32_to_cpu(0x87494ec7UL), be32_to_cpu(0xd938d1c1UL), be32_to_cpu(0x8ccaa2feUL), be32_to_cpu(0x98d40b36UL),
    be32_to_cpu(0xa6f581cfUL), be32_to_cpu(0xa57ade28UL), be32_to_cpu(0xdab78e26UL), be32_to_cpu(0x3fadbfa4UL),
    be32_to_cpu(0x2c3a9de4UL), be32_to_cpu(0x5078920dUL), be32_to_cpu(0x6a5fcc9bUL), be32_to_cpu(0x547e4662UL),
    be32_to_cpu(0xf68d13c2UL), be32_to_cpu(0x90d8b8e8UL), be32_to_cpu(0x2e39f75eUL), be32_to_cpu(0x82c3aff5UL),
    be32_to_cpu(0x9f5d80beUL), be32_to_cpu(0x69d0937cUL), be32_to_cpu(0x6fd52da9UL), be32_to_cpu(0xcf2512b3UL),
    be32_to_cpu(0xc8ac993bUL), be32_to_cpu(0x10187da7UL), be32_to_cpu(0xe89c636eUL), be32_to_cpu(0xdb3bbb7bUL),
    be32_to_cpu(0xcd267809UL), be32_to_cpu(0x6e5918f4UL), be32_to_cpu(0xec9ab701UL), be32_to_cpu(0x834f9aa8UL),
    be32_to_cpu(0xe6956e65UL), be32_to_cpu(0xaaffe67eUL), be32_to_cpu(0x21bccf08UL), be32_to_cpu(0xef15e8e6UL),
    be32_to_cpu(0xbae79bd9UL), be32_to_cpu(0x4a6f36ceUL), be32_to_cpu(0xea9f09d4UL), be32_to_cpu(0x29b07cd6UL),
    be32_to_cpu(0x31a4b2afUL), be32_to_cpu(0x2a3f2331UL), be32_to_cpu(0xc6a59430UL), be32_to_cpu(0x35a266c0UL),
    be32_to_cpu(0x744ebc37UL), be32_to_cpu(0xfc82caa6UL), be32_to_cpu(0xe090d0b0UL), be32_to_cpu(0x33a7d815UL),
    be32_to_cpu(0xf104984aUL), be32_to_cpu(0x41ecdaf7UL), be32_to_cpu(0x7fcd500eUL), be32_to_cpu(0x1791f62fUL),
    be32_to_cpu(0x764dd68dUL), be32_to_cpu(0x43efb04dUL), be32_to_cpu(0xccaa4d54UL), be32_to_cpu(0xe49604dfUL),
    be32_to_cpu(0x9ed1b5e3UL), be32_to_cpu(0x4c6a881bUL), be32_to_cpu(0xc12c1fb8UL), be32_to_cpu(0x4665517fUL),
    be32_to_cpu(0x9d5eea04UL), be32_to_cpu(0x018c355dUL), be32_to_cpu(0xfa877473UL), be32_to_cpu(0xfb0b412eUL),
    be32_to_cpu(0xb3671d5aUL), be32_to_cpu(0x92dbd252UL), be32_to_cpu(0xe9105633UL), be32_to_cpu(0x6dd64713UL),
    be32_to_cpu(0x9ad7618cUL), be32_to_cpu(0x37a10c7aUL), be32_to_cpu(0x59f8148eUL), be32_to_cpu(0xeb133c89UL),
    be32_to_cpu(0xcea927eeUL), be32_to_cpu(0xb761c935UL), be32_to_cpu(0xe11ce5edUL), be32_to_cpu(0x7a47b13cUL),
    be32_to_cpu(0x9cd2df59UL), be32_to_cpu(0x55f2733fUL), be32_to_cpu(0x1814ce79UL), be32_to_cpu(0x73c737bfUL),
    be32_to_cpu(0x53f7cdeaUL), be32_to_cpu(0x5ffdaa5bUL), be32_to_cpu(0xdf3d6f14UL), be32_to_cpu(0x7844db86UL),
    be32_to_cpu(0xcaaff381UL), be32_to_cpu(0xb968c43eUL), be32_to_cpu(0x3824342cUL), be32_to_cpu(0xc2a3405fUL),
    be32_to_cpu(0x161dc372UL), be32_to_cpu(0xbce2250cUL), be32_to_cpu(0x283c498bUL), be32_to_cpu(0xff0d9541UL),
    be32_to_cpu(0x39a80171UL), be32_to_cpu(0x080cb3deUL), be32_to_cpu(0xd8b4e49cUL), be32_to_cpu(0x6456c190UL),
    be32_to_cpu(0x7bcb8461UL), be32_to_cpu(0xd532b670UL), be32_to_cpu(0x486c5c74UL), be32_to_cpu(0xd0b85742UL),
};

static const uint8_t TD4[256] =
{
    0x52, 0x09, 0x6a, 0xd5,
    0x30, 0x36, 0xa5, 0x38,
    0xbf, 0x40, 0xa3, 0x9e,
    0x81, 0xf3, 0xd7, 0xfb,
    0x7c, 0xe3, 0x39, 0x82,
    0x9b, 0x2f, 0xff, 0x87,
    0x34, 0x8e, 0x43, 0x44,
    0xc4, 0xde, 0xe9, 0xcb,
    0x54, 0x7b, 0x94, 0x32,
    0xa6, 0xc2, 0x23, 0x3d,
    0xee, 0x4c, 0x95, 0x0b,
    0x42, 0xfa, 0xc3, 0x4e,
    0x08, 0x2e, 0xa1, 0x66,
    0x28, 0xd9, 0x24, 0xb2,
    0x76, 0x5b, 0xa2, 0x49,
    0x6d, 0x8b, 0xd1, 0x25,
    0x72, 0xf8, 0xf6, 0x64,
    0x86, 0x68, 0x98, 0x16,
    0xd4, 0xa4, 0x5c, 0xcc,
    0x5d, 0x65, 0xb6, 0x92,
    0x6c, 0x70, 0x48, 0x50,
    0xfd, 0xed, 0xb9, 0xda,
    0x5e, 0x15, 0x46, 0x57,
    0xa7, 0x8d, 0x9d, 0x84,
    0x90, 0xd8, 0xab, 0x00,
    0x8c, 0xbc, 0xd3, 0x0a,
    0xf7, 0xe4, 0x58, 0x05,
    0xb8, 0xb3, 0x45, 0x06,
    0xd0, 0x2c, 0x1e, 0x8f,
    0xca, 0x3f, 0x0f, 0x02,
    0xc1, 0xaf, 0xbd, 0x03,
    0x01, 0x13, 0x8a, 0x6b,
    0x3a, 0x91, 0x11, 0x41,
    0x4f, 0x67, 0xdc, 0xea,
    0x97, 0xf2, 0xcf, 0xce,
    0xf0, 0xb4, 0xe6, 0x73,
    0x96, 0xac, 0x74, 0x22,
    0xe7, 0xad, 0x35, 0x85,
    0xe2, 0xf9, 0x37, 0xe8,
    0x1c, 0x75, 0xdf, 0x6e,
    0x47, 0xf1, 0x1a, 0x71,
    0x1d, 0x29, 0xc5, 0x89,
    0x6f, 0xb7, 0x62, 0x0e,
    0xaa, 0x18, 0xbe, 0x1b,
    0xfc, 0x56, 0x3e, 0x4b,
    0xc6, 0xd2, 0x79, 0x20,
    0x9a, 0xdb, 0xc0, 0xfe,
    0x78, 0xcd, 0x5a, 0xf4,
    0x1f, 0xdd, 0xa8, 0x33,
    0x88, 0x07, 0xc7, 0x31,
    0xb1, 0x12, 0x10, 0x59,
    0x27, 0x80, 0xec, 0x5f,
    0x60, 0x51, 0x7f, 0xa9,
    0x19, 0xb5, 0x4a, 0x0d,
    0x2d, 0xe5, 0x7a, 0x9f,
    0x93, 0xc9, 0x9c, 0xef,
    0xa0, 0xe0, 0x3b, 0x4d,
    0xae, 0x2a, 0xf5, 0xb0,
    0xc8, 0xeb, 0xbb, 0x3c,
    0x83, 0x53, 0x99, 0x61,
    0x17, 0x2b, 0x04, 0x7e,
    0xba, 0x77, 0xd6, 0x26,
    0xe1, 0x69, 0x14, 0x63,
    0x55, 0x21, 0x0c, 0x7d,
};


#if CPU_BYTE_ORDER == CPU_LITTLE_ENDIAN

#define XTd0(x) TD0[x]
#define XTd1(x) ROTL(TD0[x], 8)
#define XTd2(x) ROTL(TD0[x], 16)
#define XTd3(x) ROTL(TD0[x], 24)

#define Td0(x)  XTd0(x&0xff)
#define Td1(x)  XTd1((x>>8)&0xff)
#define Td2(x)  XTd2((x>>16)&0xff)
#define Td3(x)  XTd3(x>>24)

#define TdMix_0(x)  TE4[x&0xFF]
#define TdMix_1(x)  TE4[(x>>8)&0xFF]
#define TdMix_2(x)  TE4[(x>>16)&0xFF]
#define TdMix_3(x)  TE4[x>>24]

#define Td4_0(x)  ((uint32_t)TD4[x&0xFF])
#define Td4_1(x)  ((uint32_t)TD4[(x>>8)&0xFF] << 8)
#define Td4_2(x)  ((uint32_t)TD4[(x>>16)&0xFF] << 16)
#define Td4_3(x)  ((uint32_t)TD4[x>>24] << 24)

#else

#define XTd0(x) TD0[x]
#define XTd1(x) ROTR(TD0[x], 8)
#define XTd2(x) ROTR(TD0[x], 16)
#define XTd3(x) ROTR(TD0[x], 24)

#define Td0(x)  XTd0(x>>24)
#define Td1(x)  XTd1((x>>16)&0xff)
#define Td2(x)  XTd2((x>>8)&0xff)
#define Td3(x)  XTd3(x&0xff)

#define TdMix_0(x)  TE4[x>>24]
#define TdMix_1(x)  TE4[(x>>16)&0xFF]
#define TdMix_2(x)  TE4[(x>>8)&0xFF]
#define TdMix_3(x)  TE4[x&0xFF]

#define Td4_0(x)  ((uint32_t)TD4[x>>24] << 24)
#define Td4_1(x)  ((uint32_t)TD4[(x>>16)&0xFF] << 16)
#define Td4_2(x)  ((uint32_t)TD4[(x>>8)&0xFF] << 8)
#define Td4_3(x)  ((uint32_t)TD4[x&0xFF])

#endif


/*****************************************************************************/
// CODE
/*****************************************************************************/

static void lazy_expandKeyEnc128(uint32_t *k)
{
	int i = 0;
	while (1)
	{
		k[4] = k[0] ^ TeKeyMix(k[3]) ^ RCON(i);
		k[5] = k[1] ^ k[4];
		k[6] = k[2] ^ k[5];
		k[7] = k[3] ^ k[6];
		if (++i == 10)
		   break;
		k += 4;
	}
}

static void lazy_expandKeyEnc192(uint32_t *k)
{
	int i = 0;
	while (1)
	{
		k[ 6] = k[ 0] ^ TeKeyMix(k[5]) ^ RCON(i);
		k[ 7] = k[ 1] ^ k[ 6];
		k[ 8] = k[ 2] ^ k[ 7];
		k[ 9] = k[ 3] ^ k[ 8];
		if (++i == 8)
			break;
		k[10] = k[ 4] ^ k[ 9];
		k[11] = k[ 5] ^ k[10];
		k += 6;
	}
}

static void lazy_expandKeyEnc256(uint32_t *k)
{
	int i = 0;
	for (;;)
	{
		k[ 8] = k[ 0] ^ TeKeyMix(k[7]) ^ RCON(i);
		k[ 9] = k[ 1] ^ k[ 8];
		k[10] = k[ 2] ^ k[ 9];
		k[11] = k[ 3] ^ k[10];
		if (++i == 7)
			break;
		k[12] = k[ 4] ^ TeKeyMix2(k[11]);
		k[13] = k[ 5] ^ k[12];
		k[14] = k[ 6] ^ k[13];
		k[15] = k[ 7] ^ k[14];
		k += 8;
	}
}

typedef void (*lazy_expand_handler)(uint32_t *);

static const lazy_expand_handler lazy_expandKeyEnc[3] =
{
	lazy_expandKeyEnc128, lazy_expandKeyEnc192, lazy_expandKeyEnc256,
};

static void lazy_expandKeyDec(uint32_t *k, int len)
{
	k += len-5;
	for (int i=0;i<len-8;++i)
	{
		k[0] = XTd0(TdMix_0(k[0])) ^ XTd1(TdMix_1(k[0])) ^ XTd2(TdMix_2(k[0])) ^ XTd3(TdMix_3(k[0]));
		--k;
	}
}

static void AES_expandKey(BlockCipher *c_, const void *key, size_t len)
{
	AES_Context *c = (AES_Context *)c_;
	ASSERT(len == c->c.key_len);

	memcpy (c->expkey, key, c->c.key_len);
	c->key_status = 0;
}

static void AES_encrypt(BlockCipher *c_, void *block)
{
	AES_Context *c = (AES_Context *)c_;
	uint32_t *k = c->expkey;
	uint32_t t0, t1, t2, t3, s0, s1, s2, s3;
	int Nr = c->num_rounds;

	if (c->key_status <= 0)
	{
		lazy_expandKeyEnc[(Nr-10U)/2](k);
		c->key_status = 1;
	}

	s0 = ((uint32_t*)block)[0];
	s1 = ((uint32_t*)block)[1];
	s2 = ((uint32_t*)block)[2];
	s3 = ((uint32_t*)block)[3];

	s0 ^= k[0];
	s1 ^= k[1];
	s2 ^= k[2];
	s3 ^= k[3];

	int r = 0;
	while (1)
	{
		k += 4;
		t0 = Te0(s0)^Te1(s1)^Te2(s2)^Te3(s3)^k[0];
		t1 = Te0(s1)^Te1(s2)^Te2(s3)^Te3(s0)^k[1];
		t2 = Te0(s2)^Te1(s3)^Te2(s0)^Te3(s1)^k[2];
		t3 = Te0(s3)^Te1(s0)^Te2(s1)^Te3(s2)^k[3];
		if (r == Nr-2)
			break;
		s0 = t0; s1 = t1; s2 = t2; s3 = t3;
		++r;
	}
	k += 4;

	s0 = Te4_3(t0)^Te4_2(t1)^Te4_1(t2)^Te4_0(t3)^k[0];
	s1 = Te4_3(t1)^Te4_2(t2)^Te4_1(t3)^Te4_0(t0)^k[1];
	s2 = Te4_3(t2)^Te4_2(t3)^Te4_1(t0)^Te4_0(t1)^k[2];
	s3 = Te4_3(t3)^Te4_2(t0)^Te4_1(t1)^Te4_0(t2)^k[3];

	((uint32_t*)block)[0] = s0;
	((uint32_t*)block)[1] = s1;
	((uint32_t*)block)[2] = s2;
	((uint32_t*)block)[3] = s3;
}


static void AES_decrypt(BlockCipher *c_, void *block)
{
	AES_Context *c = (AES_Context *)c_;
	uint32_t *k = c->expkey;
	uint32_t t0, t1, t2, t3, s0, s1, s2, s3;
	uint8_t Nr = c->num_rounds;
	int klen = (Nr+1)*4;

	if (c->key_status >= 0)
	{
		if (c->key_status == 0)
			lazy_expandKeyEnc[(Nr-10U)/2](k);
		lazy_expandKeyDec(k, klen);
		c->key_status = -1;
	}

	k += klen-4;

	s0 = ((uint32_t*)block)[0] ^ k[0];
	s1 = ((uint32_t*)block)[1] ^ k[1];
	s2 = ((uint32_t*)block)[2] ^ k[2];
	s3 = ((uint32_t*)block)[3] ^ k[3];

	int r = 0;
	while (1)
	{
		k -= 4;

		t0 = Td0(s0)^Td1(s3)^Td2(s2)^Td3(s1)^k[0];
		t1 = Td0(s1)^Td1(s0)^Td2(s3)^Td3(s2)^k[1];
		t2 = Td0(s2)^Td1(s1)^Td2(s0)^Td3(s3)^k[2];
		t3 = Td0(s3)^Td1(s2)^Td2(s1)^Td3(s0)^k[3];

		if (r == Nr-2)
			break;
		s0 = t0; s1 = t1; s2 = t2; s3 = t3;
		++r;
	}

	k -= 4;
	s0 = Td4_0(t0)^Td4_1(t3)^Td4_2(t2)^Td4_3(t1)^k[0];
	s1 = Td4_0(t1)^Td4_1(t0)^Td4_2(t3)^Td4_3(t2)^k[1];
	s2 = Td4_0(t2)^Td4_1(t1)^Td4_2(t0)^Td4_3(t3)^k[2];
	s3 = Td4_0(t3)^Td4_1(t2)^Td4_2(t1)^Td4_3(t0)^k[3];

	((uint32_t*)block)[0] = s0;
	((uint32_t*)block)[1] = s1;
	((uint32_t*)block)[2] = s2;
	((uint32_t*)block)[3] = s3;
}

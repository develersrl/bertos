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
 * Copyright 2004 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 * \brief Functions to convert integers to/from host byte-order.
 *
 * \version $Id$
 * \author Bernie Innocenti <bernie@codewiz.org>
 * \author Stefano Fedrigo <aleph@develer.com>
 */

#ifndef MWARE_BYTEORDER_H
#define MWARE_BYTEORDER_H

#include <cfg/compiler.h>
#include <cpu/attr.h>

/**
 * Swap upper and lower bytes in a 16-bit value.
 */
INLINE uint16_t swab16(uint16_t x)
{
	return    ((x & (uint16_t)0x00FFU) << 8)
		| ((x & (uint16_t)0xFF00U) >> 8);
}

/**
 * Reverse bytes in a 32-bit value (e.g.: 0x12345678 -> 0x78563412).
 */
INLINE uint32_t swab32(uint32_t x)
{
	return    ((x & (uint32_t)0x000000FFUL) << 24)
		| ((x & (uint32_t)0x0000FF00UL) <<  8)
		| ((x & (uint32_t)0x00FF0000UL) >>  8)
		| ((x & (uint32_t)0xFF000000UL) >> 24);
}

/**
 * Reverse bytes in a 64-bit value.
 */
INLINE uint64_t swab64(uint64_t x)
{
	return (uint64_t)swab32(x >> 32)
		| ((uint64_t)swab32(x & 0xFFFFFFFFUL) << 32);
}

/**
 * Reverse bytes in a float value.
 */
INLINE float swab_float(float x)
{
	/* Avoid breaking strict aliasing rules.  */
	char *cx = (char *)(&x);
	STATIC_ASSERT(sizeof(float) == 4);
	#define BYTEORDER_SWAP(a, b) ((a) ^= (b) ^= (a) ^= (b))
	BYTEORDER_SWAP(cx[0], cx[3]);
	BYTEORDER_SWAP(cx[1], cx[2]);
	#undef BYTEORDER_SWAP
	return x;
}

INLINE uint16_t cpu_to_be16(uint16_t x)
{
	return (CPU_BYTE_ORDER == CPU_LITTLE_ENDIAN) ? swab16(x) : x;
}

INLINE uint16_t cpu_to_le16(uint16_t x)
{
	return (CPU_BYTE_ORDER == CPU_BIG_ENDIAN) ? swab16(x) : x;
}

INLINE uint32_t cpu_to_be32(uint32_t x)
{
	return (CPU_BYTE_ORDER == CPU_LITTLE_ENDIAN) ? swab32(x) : x;
}

INLINE uint32_t cpu_to_le32(uint32_t x)
{
	return (CPU_BYTE_ORDER == CPU_BIG_ENDIAN) ? swab32(x) : x;
}

INLINE uint64_t cpu_to_be64(uint64_t x)
{
	return (CPU_BYTE_ORDER == CPU_LITTLE_ENDIAN) ? swab64(x) : x;
}

INLINE uint64_t cpu_to_le64(uint64_t x)
{
	return (CPU_BYTE_ORDER == CPU_BIG_ENDIAN) ? swab64(x) : x;
}

INLINE float cpu_to_be_float(float x)
{
	return (CPU_BYTE_ORDER == CPU_LITTLE_ENDIAN) ? swab_float(x) : x;
}

INLINE float cpu_to_le_float(float x)
{
	return (CPU_BYTE_ORDER == CPU_BIG_ENDIAN) ? swab_float(x) : x;
}

INLINE uint16_t be16_to_cpu(uint16_t x)
{
	return cpu_to_be16(x);
}

INLINE uint16_t le16_to_cpu(uint16_t x)
{
	return cpu_to_le16(x);
}

INLINE uint32_t be32_to_cpu(uint32_t x)
{
	return cpu_to_be32(x);
}

INLINE uint32_t le32_to_cpu(uint32_t x)
{
	return cpu_to_le32(x);
}

INLINE uint64_t be64_to_cpu(uint64_t x)
{
	return cpu_to_be64(x);
}

INLINE uint64_t le64_to_cpu(uint64_t x)
{
	return cpu_to_le64(x);
}

INLINE float be_float_to_cpu(float x)
{
	return cpu_to_be_float(x);
}

INLINE float le_float_to_cpu(float x)
{
	return cpu_to_le_float(x);
}

INLINE uint16_t host_to_net16(uint16_t x)
{
	return cpu_to_be16(x);
}

INLINE uint16_t net_to_host16(uint16_t x)
{
	return be16_to_cpu(x);
}

INLINE uint32_t host_to_net32(uint32_t x)
{
	return cpu_to_be32(x);
}

INLINE uint32_t net_to_host32(uint32_t x)
{
	return be32_to_cpu(x);
}

INLINE uint64_t host_to_net64(uint64_t x)
{
	return cpu_to_be64(x);
}

INLINE uint64_t net_to_host64(uint64_t x)
{
	return be64_to_cpu(x);
}

INLINE float host_to_net_float(float x)
{
	return cpu_to_be_float(x);
}

INLINE float net_to_host_float(float x)
{
	return be_float_to_cpu(x);
}

#ifdef __cplusplus

/// Type generic byte swapping.
template<typename T>
INLINE T swab(T x);

template<> INLINE uint16_t swab(uint16_t x) { return swab16(x); }
template<> INLINE uint32_t swab(uint32_t x) { return swab32(x); }
template<> INLINE uint64_t swab(uint64_t x) { return swab64(x); }
template<> INLINE int16_t  swab(int16_t x)  { return static_cast<int16_t>(swab16(static_cast<uint16_t>(x))); }
template<> INLINE int32_t  swab(int32_t x)  { return static_cast<int32_t>(swab32(static_cast<uint32_t>(x))); }
template<> INLINE int64_t  swab(int64_t x)  { return static_cast<int64_t>(swab64(static_cast<uint64_t>(x))); }
template<> INLINE float    swab(float x)    { return swab_float(x); }

/// Type generic conversion from CPU byte order to big-endian byte order.
template<typename T>
INLINE T cpu_to_be(T x)
{
	return (CPU_BYTE_ORDER == CPU_LITTLE_ENDIAN) ? swab(x) : x;
}

/// Type generic conversion from CPU byte-order to little-endian.
template<typename T>
INLINE T cpu_to_le(T x)
{
	return (CPU_BYTE_ORDER == CPU_BIG_ENDIAN) ? swab(x) : x;
}

/// Type generic conversion from big endian byte-order to CPU byte order.
template<typename T>
INLINE T be_to_cpu(T x)
{
	return cpu_to_be(x);
}

/// Type generic conversion from little-endian byte order to CPU byte order.
template<typename T>
INLINE T le_to_cpu(T x)
{
	return cpu_to_le(x);
}

/// Type generic conversion from network byte order to host byte order.
template<typename T>
INLINE T net_to_host(T x)
{
	return be_to_cpu(x);
}

/// Type generic conversion from host byte order to network byte order.
template<typename T>
INLINE T host_to_net(T x)
{
	return net_to_host(x);
}

#endif /* __cplusplus */

#endif /* MWARE_BYTEORDER_H */

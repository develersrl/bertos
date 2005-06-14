/*!
 * \file
 * <!--
 * Copyright 2004 Develer S.r.l. (http://www.develer.com/)
 * This file is part of DevLib - See devlib/README for information.
 * -->
 *
 * \brief Functions to convert integers to/from host byte-order.
 *
 * \version $Id$
 *
 * \author Bernardo Innocenti <bernie@develer.com>
 * \author Stefano Fedrigo <aleph@develer.com>
 */

/*#*
 *#* $Log$
 *#* Revision 1.8  2005/06/14 06:16:03  bernie
 *#* Add all missing functions.
 *#*
 *#* Revision 1.7  2005/04/12 04:08:49  bernie
 *#* host_to_net(16|32)(), net_to_host(16|32)(): New functions.
 *#*
 *#* Revision 1.6  2005/04/11 19:10:28  bernie
 *#* Include top-level headers from cfg/ subdir.
 *#*
 *#* Revision 1.5  2004/08/25 14:12:09  rasky
 *#* Aggiornato il comment block dei log RCS
 *#*
 *#* Revision 1.4  2004/07/22 01:08:43  bernie
 *#* swab32(): Fix a very serious bug.
 *#*
 *#* Revision 1.3  2004/07/20 23:47:12  bernie
 *#* Finally remove redundant protos.
 *#*
 *#* Revision 1.2  2004/07/20 17:09:11  bernie
 *#* swab16(), swab32(), cpu_to_be32(), cpu_to_le32(): New functions.
 *#*
 *#* Revision 1.1  2004/07/20 16:26:15  bernie
 *#* Import byte-order macros into DevLib.
 *#*
 *#*/

#ifndef MWARE_BYTEORDER_H
#define MWARE_BYTEORDER_H

#include <cfg/compiler.h>
#include <cfg/cpu.h>

/*!
 * Swap upper and lower bytes in a 16-bit value.
 */
INLINE uint16_t swab16(uint16_t x)
{
	return    ((x & (uint16_t)0x00FFU) << 8)
		| ((x & (uint16_t)0xFF00U) >> 8);
}

/*!
 * Reverse bytes in a 32-bit value (e.g.: 0x12345678 -> 0x78563412).
 */
INLINE uint32_t swab32(uint32_t x)
{
	return    ((x & (uint32_t)0x000000FFUL) << 24)
		| ((x & (uint32_t)0x0000FF00UL) <<  8)
		| ((x & (uint32_t)0x00FF0000UL) >>  8)
		| ((x & (uint32_t)0xFF000000UL) >> 24);
}

/*!
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

INLINE float host_to_net_float(float x)
{
	return cpu_to_be_float(x);
}

INLINE float net_to_host_float(float x)
{
	return be_float_to_cpu(x);
}

#ifdef __cplusplus

//! Type generic byte swapping.
template<typename T>
INLINE T swab(T x);

template<> INLINE uint16_t swab(uint16_t x) { return swab16(x); }
template<> INLINE uint32_t swab(uint32_t x) { return swab32(x); }
template<> INLINE int16_t  swab(int16_t x)  { return static_cast<int16_t>(swab16(static_cast<uint16_t>(x))); }
template<> INLINE int32_t  swab(int32_t x)  { return static_cast<int32_t>(swab32(static_cast<uint32_t>(x))); }
template<> INLINE float    swab(float x)    { return swab_float(x); }

//! Type generic conversion from CPU byte order to big-endian byte order.
template<typename T>
INLINE T cpu_to_be(T x)
{
	return (CPU_BYTE_ORDER == CPU_LITTLE_ENDIAN) ? swab(x) : x;
}

//! Type generic conversion from CPU byte-order to little-endian.
template<typename T>
INLINE T cpu_to_le(T x)
{
	return (CPU_BYTE_ORDER == CPU_BIG_ENDIAN) ? swab(x) : x;
}

//! Type generic conversion from big endian byte-order to CPU byte order.
template<typename T>
INLINE T be_to_cpu(T x)
{
	return cpu_to_be(x);
}

//! Type generic conversion from little-endian byte order to CPU byte order.
template<typename T>
INLINE T le_to_cpu(T x)
{
	return cpu_to_le(x);
}

//! Type generic conversion from network byte order to host byte order.
template<typename T>
INLINE T net_to_host(T x)
{
	return be_to_cpu(x);
}

//! Type generic conversion from host byte order to network byte order.
template<typename T>
INLINE T host_to_net(T x)
{
	return net_to_host(x);
}

#endif /* __cplusplus */

#endif /* MWARE_BYTEORDER_H */

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

#include <compiler.h>
#include <cpu.h>

/*!
 * \brief Swap upper and lower bytes in a 16-bit value.
 */
INLINE uint16_t swab16(uint16_t x)
{
	return    ((x & (uint16_t)0x00FFU) << 8)
		| ((x & (uint16_t)0xFF00U) >> 8);
}

/*!
 * \brief Reverse bytes in a 32-bit value (e.g.: 0x12345678 -> 0x78563412).
 */
INLINE uint32_t swab32(uint32_t x)
{
	return    ((x & (uint32_t)0x000000FFUL) << 24)
		| ((x & (uint32_t)0x0000FF00UL) <<  8)
		| ((x & (uint32_t)0x00FF0000UL) >>  8)
		| ((x & (uint32_t)0xFF000000UL) >> 24);
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

#endif /* MWARE_BYTEORDER_H */

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

/*
 * $Log$
 * Revision 1.1  2004/07/20 16:26:15  bernie
 * Import byte-order macros into DevLib.
 *
 */

#ifndef MWARE_BYTEORDER_H
#define MWARE_BYTEORDER_H

#include <compiler.h>
#include <cpu.h>

INLINE uint16_t cpu_to_be16(uint16_t n);
INLINE uint16_t cpu_to_be16(uint16_t n)
{
	if (CPU_BYTE_ORDER == CPU_LITTLE_ENDIAN)
		n = n << 8 | n >> 8;

	return n;
}

INLINE uint16_t cpu_to_le16(uint16_t n);
INLINE uint16_t cpu_to_le16(uint16_t n)
{
	if (CPU_BYTE_ORDER == CPU_BIG_ENDIAN)
		n = n << 8 | n >> 8;

	return n;
}

#endif /* MWARE_BYTEORDER_H */

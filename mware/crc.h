/*!
 * \file
 * <!--
 * Copyright 2003,2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 1999 Bernardo Innocenti <bernie@develer.com>
 * All rights reserved.
 * -->
 *
 * \brief Definitions for CRC generator
 * \version $Id$
 *
 * \author Bernardo Innocenti <bernie@develer.com>
 */

/*
 * $Log$
 * Revision 1.1  2004/06/03 08:58:16  bernie
 * Import into DevLib
 *
 */
#ifndef CRC_H
#define CRC_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdint.h> // uint16_t
#include <stddef.h> // size_t

/* CRC table */
extern const uint16_t crc16tab[256];


/*!
 * updcrc macro derived from article Copyright (C) 1986 Stephen Satchell.
 * \note First argument must be in range 0 to 255.
 * \note Second argument is referenced twice.
 *
 * Programmers may incorporate any or all code into their programs,
 * giving proper credit within the source. Publication of the
 * source routines is permitted so long as proper credit is given
 * to Stephen Satchell, Satchell Evaluations and Chuck Forsberg,
 * Omen Technology.
 */
#define UPDCRC16(c, oldcrc) (crc16tab[((oldcrc) >> 8) ^ ((unsigned char)(c))] ^ ((oldcrc) << 8))

/*!
 * This function implements the CRC 16 calculation on a buffer.
 *
 * \param crc  Current CRC16 value.
 * \param buf  The buffer to perform CRC calculation on.
 * \param len  The length of the Buffer.
 *
 * \return The updated CRC 16 value.
 */
extern uint16_t crc16(uint16_t crc, const void *buf, size_t len);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* CRC_H */

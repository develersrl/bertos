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
 * Copyright 2003, 2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 1999 Bernardo Innocenti <bernie@develer.com>
 * This file is part of DevLib - See README.devlib for information.
 * -->
 *
 * \brief XModem-CRC16 algorithm (interface)
 *
 * \note This algorithm is incompatible with the CCITT-CRC16.
 *
 * This code is based on the article Copyright 1986 Stephen Satchell.
 *
 * Programmers may incorporate any or all code into their programs,
 * giving proper credit within the source. Publication of the
 * source routines is permitted so long as proper credit is given
 * to Stephen Satchell, Satchell Evaluations and Chuck Forsberg,
 * Omen Technology.
 *
 * \version $Id$
 * \author Bernardo Innocenti <bernie@develer.com>
 */

/*#*
 *#* $Log$
 *#* Revision 1.1  2007/06/07 09:09:41  batt
 *#* Move crc routines to algos/.
 *#*
 *#* Revision 1.7  2006/07/19 12:56:27  bernie
 *#* Convert to new Doxygen style.
 *#*
 *#* Revision 1.6  2005/11/04 16:20:02  bernie
 *#* Fix reference to README.devlib in header.
 *#*
 *#* Revision 1.5  2005/04/11 19:10:28  bernie
 *#* Include top-level headers from cfg/ subdir.
 *#*
 *#* Revision 1.4  2004/08/25 14:12:09  rasky
 *#* Aggiornato il comment block dei log RCS
 *#*
 *#* Revision 1.3  2004/08/15 05:47:26  bernie
 *#* updcrc16(): inline version of UPDCRC16(); Cleanup documentation.
 *#*
 *#* Revision 1.2  2004/06/03 11:27:09  bernie
 *#* Add dual-license information.
 *#*
 *#* Revision 1.1  2004/06/03 08:58:16  bernie
 *#* Import into DevLib
 *#*
 *#*/
#ifndef CRC_H
#define CRC_H

#include <cfg/compiler.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/* CRC table */
extern const uint16_t crc16tab[256];


/**
 * \brief Compute the updated CRC16 value for one octet (macro version)
 *
 * \note This version is only intended for old/broken compilers.
 *       Use the inline function in new code.
 *
 * \param c New octet (range 0-255)
 * \param oldcrc Previous CRC16 value (referenced twice, beware of side effects)
 */
#define UPDCRC16(c, oldcrc) (crc16tab[((oldcrc) >> 8) ^ ((unsigned char)(c))] ^ ((oldcrc) << 8))


#ifdef INLINE
/**
 * \brief Compute the updated CRC16 value for one octet (macro version)
 */
INLINE uint16_t updcrc16(uint8_t c, uint16_t oldcrc)
{
	return crc16tab[(oldcrc >> 8) ^ c] ^ (oldcrc << 8);
}
#endif // INLINE


/**
 * This function implements the CRC 16 calculation on a buffer.
 *
 * \param crc  Current CRC16 value.
 * \param buf  The buffer to perform CRC calculation on.
 * \param len  The length of the Buffer.
 *
 * \return The updated CRC16 value.
 */
extern uint16_t crc16(uint16_t crc, const void *buf, size_t len);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* CRC_H */

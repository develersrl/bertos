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
 * Copyright 1999 Bernie Innocenti <bernie@codewiz.org>
 *
 * -->
 *
 * \brief Cyclic Redundancy Check 16 (CRC). This algorithm is the one used by the XMODEM protocol.
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
 * \author Bernie Innocenti <bernie@codewiz.org>
 *
 * $WIZ$ module_name = "crc8"
 */

#ifndef CRC8_H
#define CRC8_H

#include "cfg/cfg_arch.h"

#include <cfg/compiler.h>

uint8_t crc8(uint8_t * data, size_t len);

#define CRC8INIT    0x00
#define CRC8POLY    0x18
//0X18 = X^8+X^5+X^4+X^0

int crc_testSetup(void);
int crc_testRun(void);
int crc_testTearDown(void);


#endif /* CRC8_H */

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
 * Copyright 2007 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \version $Id:$
 *
 * \author Francesco Sacchi <batt@develer.com>
 *
 * \brief BattFS: a filesystem for embedded platforms (interface).
 */

#ifndef FS_BATTFS_H
#define FS_BATTFS_H

#include <cfg/compiler.h> // uintXX_t; STATIC_ASSERT

/**
 * BattFS page header.
 * \note fields order is chosen to comply with alignment.
 */
typedef struct BattFsPageHeader
{
	uint16_t filled;      ///< filled bytes in page.
	uint16_t page_off;    ///< Page offset inside file.
	uint8_t  inode;       ///< File inode (file identifier).
	uint8_t  seqnum;      ///< bit[1:0]: Page sequence number; bit[7:2] unused for now, must be set to 1.
	uint16_t crc;         ///< CRC of the page header.
} BattFsPageHeader;

STATIC_ASSERT(sizeof(BattFsPageHeader) == 8);

/**
 * Reset page sequence number of struct \a m to default value (0xFF).
 */
#define RESET_SEQ(m) ((m).seqnum = 0xFF)

/**
 * Get page sequence number from struct \a m.
 */
#define SEQ(m) ((m).seqnum & 0x03)

/**
 * Set sequence number of struct \a m to \a d.
 */
#define SET_SEQ(m, d) ((m).seqnum = ((m).seqnum & 0xFC) | ((d) & 0x03))

/**
 * Increment sequence number of struct \a m.
 */
#define INC_SEQ(m) SET_SEQ((m), SEQ(m) + 1)


#endif /* FS_BATTFS_H */

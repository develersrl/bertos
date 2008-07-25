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
 * Copyright 2008 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \author Andrea Grandi <andrea@develer.com>
 *
 * \brief Tag protocol. (interface).
 */

#ifndef NET_KEYTAG_H
#define NET_KEYTAG_H

#include <kern/kfile.h>
#include <drv/ser.h>

/**
 * Starting communication char (STX).
 */
#define TAG_STX 0x02

/**
 * Ending communication char (ETX).
 */
#define TAG_ETX 0x03

/**
 * Max buffer lenght
 */
#define TAG_MAX_LEN 14


/**
 * Max number of chars to print in the communication serial
 */
#define TAG_MAX_PRINT_CHARS 12

/**
 * Structure of a Tag packet
 */
typedef struct TagPacket
{
	KFile *tag;                 ///< Tag communication channel
	KFile *host;                ///< Host communication channel
	bool sync;                  ///< Status flag: true if we find an STX
	uint16_t len;               ///< Packet lenght
	uint8_t buf[TAG_MAX_LEN];   ///< Reception buffer
} TagPacket;

void keytag_init(struct TagPacket *pkt, struct KFile *comm, struct KFile *tag);
void keytag_poll(struct TagPacket *pkt);

#endif /* NET_TAG_H */

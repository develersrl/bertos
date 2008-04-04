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
 * \brief Tag protocol (protocol).
 *
 * TAG protocol is decribed in this way:
 * <pre>
 * ---------------------------------------------------
 * |STX (0x02)|data...(10 HEX chars)|CR|LF|ETX (0x03)|
 * ---------------------------------------------------
 * </pre>
 */

#include "keytag.h"

#include <kern/kfile.h>

#include <drv/timer.h>
#include <drv/ser.h>

#include <cfg/macros.h>
#include <cfg/debug.h>

static void keytag_clearPkt(struct TagPacket *pkt)
{
	pkt->sync = false;
	pkt->len = 0;
}

	void keytag_init(struct TagPacket *pkt)
	{
		keytag_clearPkt(pkt);
	}

	void keytag_poll(struct TagPacket *pkt)
{
	int c;

	/* Get all chars from buffer */
	while ((c = kfile_getc(&pkt->tag_ser->fd)) != EOF)
	{
		/* Search for STX char in received chars */
		if (c == TAG_STX)
		{
			/* When STX is found a new packet begins */
			if (pkt->sync)
				kprintf("TAG double sync!\n");
			keytag_clearPkt(pkt);
			pkt->sync = true;
		}
		else if (pkt->sync)
		{
			/* Check for end of packet */
			if (c == TAG_ETX)
			{
				pkt->buf[TAG_MAX_PRINT_CHARS] = '\x0';
				/* Write read TAG on communication serial */
				kfile_printf(&pkt->comm_ser->fd, "tag %s", pkt->buf);
				pkt->sync = false;
			}
			else
			{
				/* Check for buffer overflow */
				if (pkt->len >= TAG_MAX_LEN)
				{
					kprintf("TAG buffer overflow\n");
					pkt->sync = false;
				}
				else
				{
					/* Add every char after STX to tag reading buffer */
					if (pkt->sync)
					{
						pkt->buf[pkt->len] = c;
						pkt->len++;
					}
				}
			}
		}
	}
}

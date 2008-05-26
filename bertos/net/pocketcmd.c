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
 * -->
 *
 * \brief pocketBus protocol Command layer implementation.
 *
 * This module implements command layer over pocketBus
 * protocol.
 * Payload packets received by pocketBus are first checked for
 * address matching.
 * If a packet is addressed to us we look for a suitable
 * callback function to call.
 *
 * The received payload format is as follows:
 * <pre>
 * +----------------------------------------+
 * |  CMD |            DATA                 |
 * +----------------------------------------+
 * |      |                                 |
 * +  2B  +           0..N Byte             +
 * </pre>
 *
 * The CMD ID used is the same supplied by the master when
 * the command was sent.
 *
 * \version $Id: pocketcmd.c 16587 2007-10-02 14:31:02Z batt $
 *
 * \author Francesco Sacchi <batt@develer.com>
 */

#include "pocketcmd.h"
#include "pocketbus.h"

#include <cfg/macros.h>
#include <cfg/debug.h>
#include <cfg/module.h>

#include <drv/timer.h>

#include <mware/byteorder.h>

#include <string.h>

/**
 * pocketBus Command poll function.
 * Call it to read and process pocketBus commands.
 */
void pocketcmd_poll(struct PocketCmdCtx *ctx)
{
	PocketMsg msg;

	/* Try to read a packet from pocketBus */
	while (pocketbus_recv(ctx->bus_ctx, &msg))
	{
		/* Check address */
		if (msg.addr == ctx->addr ||
		    msg.addr == POCKETBUS_BROADCAST_ADDR)
		{
			const PocketCmdHdr *hdr = (const PocketCmdHdr *)msg.payload;
			pocketcmd_t cmd = be16_to_cpu(hdr->cmd);

			/* We're no longer waiting for a reply (in case we were) */
			if (cmd == ctx->waiting)
				ctx->waiting = PKTCMD_NULL;

			/* Check for command callback */
			pocketcmd_hook_t callback = ctx->search(cmd);

			/* Call it if exists */
			if (callback)
			{
				PocketCmdMsg cmd_msg;

				cmd_msg.cmd_ctx = ctx;
				cmd_msg.cmd = cmd;
				cmd_msg.len = msg.len - sizeof(PocketCmdHdr);
				cmd_msg.buf = msg.payload + sizeof(PocketCmdHdr);

				callback(&cmd_msg);
			}
		}
	}
}

/**
 * Send command \a cmd to/from slave adding \a len arguments in \a buf.
 * Address used is contained in \a ctx->addr .
 * If we are master and the message has a reply, you must set \a wait_reply to true.
 * \return true if all is ok, false if we are already waiting a replay from another slave.
 */
bool pocketcmd_send(struct PocketCmdCtx *ctx, pocketcmd_t cmd, const void *buf, size_t len, bool wait_reply)
{
	/* Check if we are waiting a reply from someone */
	if (ctx->waiting != PKTCMD_NULL)
	{
		/* Check is reply timeout is elapsed */
		if (timer_clock() - ctx->reply_timer < ms_to_ticks(PKTCMD_REPLY_TIMEOUT))
		{
			TRACEMSG("Pkt discard! waiting cmd[%04X]\n", ctx->waiting);
			return false;
		}
		else
		{
			TRACEMSG("Timeout waiting cmd[%04X]\n", ctx->waiting);
			ctx->waiting = PKTCMD_NULL;
		}
	}

	/* Endianess! */
	cmd = cpu_to_be16(cmd);

	/* Send packet */
	pocketbus_begin(ctx->bus_ctx, ctx->addr);
	pocketbus_write(ctx->bus_ctx, &cmd, sizeof(cmd));
	pocketbus_write(ctx->bus_ctx, buf, len);
	pocketbus_end(ctx->bus_ctx);

	if (wait_reply)
	{
		ctx->waiting = cmd;
		ctx->reply_timer = timer_clock();
	}
	return true;
}

/**
 * Init pocketBus command layer.
 * \a ctx is pocketBus command layer context.
 * \a bus_ctx is pocketBus context.
 * \a addr is slave address (see pocketcmd_setAddr for details.)
 * \a search is the lookup function used to search command ID callbacks.
 */
void pocketcmd_init(struct PocketCmdCtx *ctx, struct PocketBusCtx *bus_ctx, pocketbus_addr_t addr, pocketcmd_lookup_t search)
{
	ASSERT(ctx);
	ASSERT(bus_ctx);
	ASSERT(search);
	MOD_CHECK(timer);

	memset(ctx, 0, sizeof(*ctx));
	ctx->bus_ctx = bus_ctx;
	ctx->search = search;
	pocketcmd_setAddr(ctx, addr);
}

/**
 * Helper function used to reply to master with an ACK.
 */
void pocketcmd_replyAck(struct PocketCmdMsg *msg)
{
	uint8_t ack[] = { POCKETBUS_ACK };

	pocketcmd_slaveReply(msg->cmd_ctx, msg->cmd, ack, sizeof(ack));
}

/**
 * Helper function used to reply to master with a NAK.
 */
void pocketcmd_replyNak(struct PocketCmdMsg *msg)
{
	uint8_t nak[] = { POCKETBUS_NAK };

	pocketcmd_slaveReply(msg->cmd_ctx, msg->cmd, nak, sizeof(nak));
}


/**
 * \file
 * <!--
 * Copyright 2007 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 * \version $Id: pocketcmd.h 20030 2007-12-04 16:16:09Z batt $
 *
 * \author Francesco Sacchi <batt@develer.com>
 *
 * \brief pocketBus protocol command layer interface.
 */

#ifndef NET_POCKETCMD_H
#define NET_POCKETCMD_H

#include "pocketbus.h"
#include <cfg/compiler.h>

#define PKTCMD_NULL 0 ///< pocketBus Null command

#define PKTCMD_REPLY_TIMEOUT 50 ///< Command replay timeout in milliseconds

typedef uint16_t pocketcmd_t; ///< Type for Command IDs

/**
 * Header for transmitted pocketBus Commands.
 */
typedef struct PocketCmdHdr
{
	pocketcmd_t cmd; ///< command ID
} PocketCmdHdr;

/**
 * This ensure that endianess convertion functions work on
 * the right data size.
 * \{
 */
STATIC_ASSERT(sizeof(pocketcmd_t) == sizeof(uint16_t));
/*\}*/

/* fwd declaration */
struct PocketCmdCtx;

/**
 * pocketBus command message structure.
 */
typedef struct PocketCmdMsg
{
	struct PocketCmdCtx *cmd_ctx; ///< command context
	pocketcmd_t cmd;              ///< command id
	pocketbus_len_t len;          ///< optional arg length
	const uint8_t *buf;           ///< optional arguments
} PocketCmdMsg;

/**
 * Type for command hooks.
 */
typedef void (*pocketcmd_hook_t)(struct PocketCmdMsg *cmd_msg);

/**
 * Type for lookup function hooks.
 */
typedef pocketcmd_hook_t (*pocketcmd_lookup_t)(pocketcmd_t cmd);

/**
 * pocketBus context for command layer communications.
 */
typedef struct PocketCmdCtx
{
	struct PocketBusCtx *bus_ctx; ///< pocketBus context
	pocketbus_addr_t addr;        ///< Our address
	pocketcmd_lookup_t search;    ///< Lookup function used to search for command callbacks
	pocketcmd_t waiting;          ///< The command ID we are waiting for or PKTCMD_NULL.
	ticks_t reply_timer;          ///< For waiting_reply
} PocketCmdCtx;

/**
 * Set slave address \a addr for pocketBus command layer.
 * If we are a slave this is *our* address.
 * If we are the master this is the slave address to send messages to.
 */
INLINE void pocketcmd_setAddr(struct PocketCmdCtx *ctx, pocketbus_addr_t addr)
{
	ctx->addr = addr;
}

void pocketcmd_init(struct PocketCmdCtx *ctx, struct PocketBusCtx *bus_ctx, pocketbus_addr_t addr, pocketcmd_lookup_t search);
void pocketcmd_poll(struct PocketCmdCtx *ctx);
bool pocketcmd_send(struct PocketCmdCtx *ctx, pocketcmd_t cmd, const void *buf, size_t len, bool has_replay);
void pocketcmd_replyNak(struct PocketCmdMsg *msg);
void pocketcmd_replyAck(struct PocketCmdMsg *msg);

/**
 * Helper function used by master to send a command to slave \a addr.
 */
INLINE bool pocketcmd_masterSend(struct PocketCmdCtx *ctx, pocketbus_addr_t addr, pocketcmd_t cmd, const void *buf, size_t len)
{
	pocketcmd_setAddr(ctx, addr);
	return pocketcmd_send(ctx, cmd, buf, len, true);
}

/**
 * Helper function used by slave to reply to a master command.
 */
INLINE bool pocketcmd_slaveReply(struct PocketCmdCtx *ctx, pocketcmd_t cmd, const void *buf, size_t len)
{
	return pocketcmd_send(ctx, cmd, buf, len, false);
}


#endif /* NET_POCKETCMD_H */

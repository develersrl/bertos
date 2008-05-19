/**
 * \file
 * <!--
 * Copyright 2007 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 * \version $Id: pocketbus.h 20131 2007-12-13 17:39:55Z batt $
 *
 * \author Francesco Sacchi <batt@develer.com>
 *
 * \brief pocketBus protocol interface.
 */

#ifndef NET_POCKETBUS_H
#define NET_POCKETBUS_H

#include <algo/rotating_hash.h>
#include <drv/ser.h>
#include <kern/kfile.h>
#include <cfg/compiler.h>
#include "appconfig.h" //for CONFIG_POCKETBUS_BUFLEN

/**
 * pocketBus special characters definitions.
 * \{
 */
#define POCKETBUS_STX 0x02 //ASCII STX
#define POCKETBUS_ETX 0x03 //ASCII ETX
#define POCKETBUS_ESC 0x1B //ASCII ESC
#define POCKETBUS_ACK 0x06 //ASCII ACK
#define POCKETBUS_NAK 0x15 //ASCII NAK
/*\}*/

#define POCKETBUS_BROADCAST_ADDR 0xFFFF ///< pocketBus broadcast address

/**
 * Type for pocketBus length.
 */
typedef uint16_t pocketbus_len_t;

/**
 * Type for pocketBus addresses.
 */
typedef uint16_t pocketbus_addr_t;

/**
 * Header of pocketBus messages.
 */
typedef struct PocketBusHdr
{
	#define POCKETBUS_VER 1 
	uint8_t ver;   ///< packet version
	pocketbus_addr_t addr; ///< slave address
} PocketBusHdr;

/**
 * pocketBus context structure.
 */
typedef struct PocketBusCtx
{
	struct KFile *fd;   ///< File descriptor
	bool sync;           ///< Status flag: true if we have received an STX, false otherwise
	bool escape;         ///< Status flag: true if we are in escape mode, false otherwise
	rotating_t in_cks;   ///< Checksum computation for received data.
	rotating_t out_cks;  ///< Checksum computation for transmitted data.
	pocketbus_len_t len; ///< Received length
	uint8_t buf[CONFIG_POCKETBUS_BUFLEN]; ///< receiving Buffer
} PocketBusCtx;

/**
 * Structure holding pocketBus message parameters.
 */
typedef struct PocketMsg
{
	struct PocketBusCtx *ctx; ///< pocketBus message context
	pocketbus_addr_t addr;    ///< address for received packet
	pocketbus_len_t len;      ///< payload length 
	const uint8_t *payload;   ///< payload data
} PocketMsg;

/**
 * This ensure that endianess convertion functions work on
 * the right data size.
 * \{
 */
STATIC_ASSERT(sizeof(pocketbus_addr_t) == sizeof(uint16_t));
STATIC_ASSERT(sizeof(rotating_t) == sizeof(uint16_t));
/*\}*/

void pocketbus_putchar(struct PocketBusCtx *ctx, uint8_t c);
void pocketbus_begin(struct PocketBusCtx *ctx, pocketbus_addr_t addr);
void pocketbus_write(struct PocketBusCtx *ctx, const void *_data, size_t len);
void pocketbus_end(struct PocketBusCtx *ctx);

void pocketbus_send(struct PocketBusCtx *ctx, pocketbus_addr_t addr, const void *data, size_t len);
bool pocketbus_recv(struct PocketBusCtx *ctx, struct PocketMsg *msg);
void pocketbus_init(struct PocketBusCtx *ctx, struct KFile *fd);

#endif /* NET_POCKETBUS_H */

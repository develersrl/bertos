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
  * Copyright 2009,2010,2011 Develer S.r.l. (http://www.develer.com/)
  * All Rights Reserved.
  * -->
  *
  * \brief EMAC driver for AT91SAM family with Davicom 9161A phy, interface.
  *
  * \author Daniele Basile <asterix@develer.com>
  * \author Andrea Righi <arighi@develer.com>
  * \author Stefano Fedrigo <aleph@develer.com>
  */

#ifndef ETH_SAM3_H
#define ETH_SAM3_H

#include <cpu/types.h>

#define EMAC_TX_BUFSIZ          1518  //!!! Don't change this
#define EMAC_TX_BUFFERS         1     //!!! Don't change this
#define EMAC_TX_DESCRIPTORS     EMAC_TX_BUFFERS

#define EMAC_RX_BUFFERS         32    //!!! Don't change this
#define EMAC_RX_BUFSIZ          128   //!!! Don't change this
#define EMAC_RX_DESCRIPTORS	EMAC_RX_BUFFERS

// Flag to manage local tx buffer
#define TXS_USED            0x80000000  //Used buffer.
#define TXS_WRAP            0x40000000  //Last descriptor.
#define TXS_ERROR           0x20000000  //Retry limit exceeded.
#define TXS_UNDERRUN        0x10000000  //Transmit underrun.
#define TXS_NO_BUFFER       0x08000000  //Buffer exhausted.
#define TXS_NO_CRC          0x00010000  //CRC not appended.
#define TXS_LAST_BUFF       0x00008000  //Last buffer of frame.
#define TXS_LENGTH_FRAME    0x000007FF  // Length of frame including FCS.

// Flag to manage local rx buffer
#define RXBUF_OWNERSHIP     0x00000001
#define RXBUF_WRAP          0x00000002

#define BUF_ADDRMASK        0xFFFFFFFC

#define RXS_BROADCAST_ADDR  0x80000000  // Broadcast address detected.
#define RXS_MULTICAST_HASH  0x40000000  // Multicast hash match.
#define RXS_UNICAST_HASH    0x20000000  // Unicast hash match.
#define RXS_EXTERNAL_ADDR   0x10000000  // External address match.
#define RXS_SA1_ADDR        0x04000000  // Specific address register 1 match.
#define RXS_SA2_ADDR        0x02000000  // Specific address register 2 match.
#define RXS_SA3_ADDR        0x01000000  // Specific address register 3 match.
#define RXS_SA4_ADDR        0x00800000  // Specific address register 4 match.
#define RXS_TYPE_ID         0x00400000  // Type ID match.
#define RXS_VLAN_TAG        0x00200000  // VLAN tag detected.
#define RXS_PRIORITY_TAG    0x00100000  // Priority tag detected.
#define RXS_VLAN_PRIORITY   0x000E0000  // VLAN priority.
#define RXS_CFI_IND         0x00010000  // Concatenation format indicator.
#define RXS_EOF             0x00008000  // End of frame.
#define RXS_SOF             0x00004000  // Start of frame.
#define RXS_RBF_OFFSET      0x00003000  // Receive buffer offset mask.
#define RXS_LENGTH_FRAME    0x000007FF  // Length of frame including FCS.

#define EMAC_RSR_BITS	(BV(EMAC_BNA) | BV(EMAC_REC) | BV(EMAC_OVR))
#define EMAC_TSR_BITS	(BV(EMAC_UBR) | BV(EMAC_COL) | BV(EMAC_RLES) | \
			BV(EMAC_BEX) | BV(EMAC_COMP) | BV(EMAC_UND))

typedef struct BufDescriptor
{
	volatile uint32_t addr;
	volatile uint32_t stat;
} BufDescriptor;

#endif /* ETH_SAM3_H */

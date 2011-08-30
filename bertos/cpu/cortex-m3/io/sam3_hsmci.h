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
 * Copyright 2011 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \author Daniele Basile <asterix@develer.com>
 *
 *  SAM3X DMAC register definitions.
 */

#ifndef SAM3_HSMCI_H
#define SAM3_HSMCI_H

#include <cfg/compiler.h>


#define HSMCI_BASE         0x40000000U


#define HSMCI_CR            (*((reg32_t *)(HSMCI_BASE + 0x000))) ///< (Hsmci Offset: 0x00) Control Register
#define HSMCI_MR            (*((reg32_t *)(HSMCI_BASE + 0x004))) ///< (Hsmci Offset: 0x04) Mode Register
#define HSMCI_DTOR          (*((reg32_t *)(HSMCI_BASE + 0x008))) ///< (Hsmci Offset: 0x08) Data Timeout Register
#define HSMCI_SDCR          (*((reg32_t *)(HSMCI_BASE + 0x00C))) ///< (Hsmci Offset: 0x0C) SD/SDIO Card Register
#define HSMCI_ARGR          (*((reg32_t *)(HSMCI_BASE + 0x010))) ///< (Hsmci Offset: 0x10) Argument Register
#define HSMCI_CMDR          (*((reg32_t *)(HSMCI_BASE + 0x014))) ///< (Hsmci Offset: 0x14) Command Register
#define HSMCI_BLKR          (*((reg32_t *)(HSMCI_BASE + 0x018))) ///< (Hsmci Offset: 0x18) Block Register
#define HSMCI_CSTOR         (*((reg32_t *)(HSMCI_BASE + 0x01C))) ///< (Hsmci Offset: 0x1C) Completion Signal Timeout Register
#define HSMCI_RSPR          (*((reg32_t *)(HSMCI_BASE + 0x020))) ///< (Hsmci Offset: 0x20) Response Register
#define HSMCI_RSPR1         (*((reg32_t *)(HSMCI_BASE + 0x024))) ///< (Hsmci Offset: 0x24) Response Register
#define HSMCI_RSPR2         (*((reg32_t *)(HSMCI_BASE + 0x028))) ///< (Hsmci Offset: 0x28) Response Register
#define HSMCI_RSPR3         (*((reg32_t *)(HSMCI_BASE + 0x02C))) ///< (Hsmci Offset: 0x2C) Response Register
#define HSMCI_RDR           (*((reg32_t *)(HSMCI_BASE + 0x030))) ///< (Hsmci Offset: 0x30) Receive Data Register
#define HSMCI_TDR           (*((reg32_t *)(HSMCI_BASE + 0x034))) ///< (Hsmci Offset: 0x34) Transmit Data Register
#define HSMCI_SR            (*((reg32_t *)(HSMCI_BASE + 0x040))) ///< (Hsmci Offset: 0x40) Status Register
#define HSMCI_IER           (*((reg32_t *)(HSMCI_BASE + 0x044))) ///< (Hsmci Offset: 0x44) Interrupt Enable Register
#define HSMCI_IDR           (*((reg32_t *)(HSMCI_BASE + 0x048))) ///< (Hsmci Offset: 0x48) Interrupt Disable Register
#define HSMCI_IMR           (*((reg32_t *)(HSMCI_BASE + 0x04C))) ///< (Hsmci Offset: 0x4C) Interrupt Mask Register
#define HSMCI_DMA           (*((reg32_t *)(HSMCI_BASE + 0x050))) ///< (Hsmci Offset: 0x50) DMA Configuration Register
#define HSMCI_CFG           (*((reg32_t *)(HSMCI_BASE + 0x054))) ///< (Hsmci Offset: 0x54) Configuration Register
#define HSMCI_WPMR          (*((reg32_t *)(HSMCI_BASE + 0x0E4))) ///< (Hsmci Offset: 0xE4) Write Protection Mode Register
#define HSMCI_WPSR          (*((reg32_t *)(HSMCI_BASE + 0x0E8))) ///< (Hsmci Offset: 0xE8) Write Protection Status Register
#define HSMCI_FIFO          (*((reg32_t *)(HSMCI_BASE + 0x200))) ///< (Hsmci Offset: 0x200) FIFO Memory Aperture0


/* HSMCI_CR : (HSMCI Offset: 0x00) Control Register */
#define HSMCI_CR_MCIEN     0 ///< (HSMCI_CR) Multi-Media Interface Enable.
#define HSMCI_CR_MCIDIS    1 ///< (HSMCI_CR) Multi-Media Interface Disable.
#define HSMCI_CR_PWSEN     2 ///< (HSMCI_CR) Power Save Mode Enable.
#define HSMCI_CR_PWSDIS    3 ///< (HSMCI_CR) Power Save Mode Disable.
#define HSMCI_CR_SWRST     7 ///< (HSMCI_CR) Software Reset.

/* HSMCI_MR : (HSMCI Offset: 0x04) Mode Register */
#define HSMCI_MR_CLKDIV_MASK     0xff ///< (HSMCI_MR) Clock Divider
#define HSMCI_MR_PWSDIV_SHIFT       8 ///< (HSMCI_MR) Power Saving Divider
#define HSMCI_MR_PWSDIV_MASK     (0x7u << HSMCI_MR_PWSDIV_SHIFT) ///< (HSMCI_MR) Power Saving Divider
#define HSMCI_MR_RDPROOF           11 ///< (HSMCI_MR)
#define HSMCI_MR_WRPROOF           12 ///< (HSMCI_MR)
#define HSMCI_MR_FBYTE             13 ///< (HSMCI_MR) Force Byte Transfer
#define HSMCI_MR_PADV              14 ///< (HSMCI_MR) Padding Value
#define HSMCI_MR_BLKLEN_SHIFT      16
#define HSMCI_MR_BLKLEN_MASK   (0xffffu << HSMCI_MR_BLKLEN_SHIFT) ///< (HSMCI_MR) Data Block Length


/* HSMCI_DTOR : (HSMCI Offset: 0x08) Data Timeout Register */
#define   HSMCI_DTOR_DTOCYC_MASK        0xfu ///< (HSMCI_DTOR) Data Timeout Cycle Number
#define   HSMCI_DTOR_DTOMUL_SHIFT          4
#define   HSMCI_DTOR_DTOMUL_MASK       (0x7u << HSMCI_DTOR_DTOMUL_SHIFT) ///< (HSMCI_DTOR) Data Timeout Multiplier
#define   HSMCI_DTOR_DTOMUL_1          (0x0u << HSMCI_DTOR_DTOMUL_SHIFT) ///< (HSMCI_DTOR) DTOCYC
#define   HSMCI_DTOR_DTOMUL_16         (0x1u << HSMCI_DTOR_DTOMUL_SHIFT) ///< (HSMCI_DTOR) DTOCYC x 16
#define   HSMCI_DTOR_DTOMUL_128        (0x2u << HSMCI_DTOR_DTOMUL_SHIFT) ///< (HSMCI_DTOR) DTOCYC x 128
#define   HSMCI_DTOR_DTOMUL_256        (0x3u << HSMCI_DTOR_DTOMUL_SHIFT) ///< (HSMCI_DTOR) DTOCYC x 256
#define   HSMCI_DTOR_DTOMUL_1024       (0x4u << HSMCI_DTOR_DTOMUL_SHIFT) ///< (HSMCI_DTOR) DTOCYC x 1024
#define   HSMCI_DTOR_DTOMUL_4096       (0x5u << HSMCI_DTOR_DTOMUL_SHIFT) ///< (HSMCI_DTOR) DTOCYC x 4096
#define   HSMCI_DTOR_DTOMUL_65536      (0x6u << HSMCI_DTOR_DTOMUL_SHIFT) ///< (HSMCI_DTOR) DTOCYC x 65536
#define   HSMCI_DTOR_DTOMUL_1048576    (0x7u << HSMCI_DTOR_DTOMUL_SHIFT) ///< (HSMCI_DTOR) DTOCYC x 1048576



/* HSMCI_SDCR : (HSMCI Offset: 0x0C) SD/SDIO Card Register */
#define   HSMCI_SDCR_SDCSEL_MASK       0x3  ///< (HSMCI_SDCR) SDCard/SDIO Slot
#define   HSMCI_SDCR_SDCSEL_SLOTA      0x0  ///< (HSMCI_SDCR) Slot A is selected.
#define   HSMCI_SDCR_SDCSEL_SLOTB      0x1  ///< (HSMCI_SDCR) SDCARD/SDIO Slot B selected
#define   HSMCI_SDCR_SDCSEL_SLOTC      0x2  ///< (HSMCI_SDCR) -
#define   HSMCI_SDCR_SDCSEL_SLOTD      0x3  ///< (HSMCI_SDCR) -
#define   HSMCI_SDCR_SDCBUS_SHIFT        6
#define   HSMCI_SDCR_SDCBUS_MASK       (0x3u << HSMCI_SDCR_SDCBUS_SHIFT) ///< (HSMCI_SDCR) SDCard/SDIO Bus Width
#define   HSMCI_SDCR_SDCBUS_1          (0x0u << HSMCI_SDCR_SDCBUS_SHIFT) ///< (HSMCI_SDCR) 1 bit
#define   HSMCI_SDCR_SDCBUS_4          (0x2u << HSMCI_SDCR_SDCBUS_SHIFT) ///< (HSMCI_SDCR) 4 bit
#define   HSMCI_SDCR_SDCBUS_8          (0x3u << HSMCI_SDCR_SDCBUS_SHIFT) ///< (HSMCI_SDCR) 8 bit

/* HSMCI_CMDR : (HSMCI Offset: 0x14) Command Register */
#define   HSMCI_CMDR_CMDNB_MASK         0x3f ///< (HSMCI_CMDR) Command Number
#define   HSMCI_CMDR_RSPTYP_SHIFT          6
#define   HSMCI_CMDR_RSPTYP_MASK        (0x3 << HSMCI_CMDR_RSPTYP_SHIFT) ///< (HSMCI_CMDR) Response Type
#define   HSMCI_CMDR_RSPTYP_NORESP       0x0 ///< (HSMCI_CMDR) No response.
#define   HSMCI_CMDR_RSPTYP_48_BIT      (0x1 << HSMCI_CMDR_RSPTYP_SHIFT) ///< (HSMCI_CMDR) 48-bit response.
#define   HSMCI_CMDR_RSPTYP_136_BIT     (0x2 << HSMCI_CMDR_RSPTYP_SHIFT) ///< (HSMCI_CMDR) 136-bit response.
#define   HSMCI_CMDR_RSPTYP_R1B         (0x3 << HSMCI_CMDR_RSPTYP_SHIFT) ///< (HSMCI_CMDR) R1b response type
#define   HSMCI_CMDR_SPCMD_SHIFT           8
#define   HSMCI_CMDR_SPCMD_MASK         (0x7 << HSMCI_CMDR_SPCMD_SHIFT) ///< (HSMCI_CMDR) Special Command
#define   HSMCI_CMDR_SPCMD_STD             0 ///< (HSMCI_CMDR) Not a special CMD.
#define   HSMCI_CMDR_SPCMD_INIT         (0x1 << HSMCI_CMDR_SPCMD_SHIFT) ///< (HSMCI_CMDR) Initialization CMD: 74 clock cycles for initialization sequence.
#define   HSMCI_CMDR_SPCMD_SYNC         (0x2 << HSMCI_CMDR_SPCMD_SHIFT) ///< (HSMCI_CMDR) Synchronized CMD: Wait for the end of the current data block transfer before sending the pending command.
#define   HSMCI_CMDR_SPCMD_CE_ATA       (0x3 << HSMCI_CMDR_SPCMD_SHIFT) ///< (HSMCI_CMDR) CE-ATA Completion Signal disable Command. The host cancels the ability for the device to return a command completion signal on the command line.
#define   HSMCI_CMDR_SPCMD_IT_CMD       (0x4 << HSMCI_CMDR_SPCMD_SHIFT) ///< (HSMCI_CMDR) Interrupt command: Corresponds to the Interrupt Mode (CMD40).
#define   HSMCI_CMDR_SPCMD_IT_RESP      (0x5 << HSMCI_CMDR_SPCMD_SHIFT) ///< (HSMCI_CMDR) Interrupt response: Corresponds to the Interrupt Mode (CMD40).
#define   HSMCI_CMDR_SPCMD_BOR          (0x6 << HSMCI_CMDR_SPCMD_SHIFT) ///< (HSMCI_CMDR) Boot Operation Request. Start a boot operation mode, the host processor can read boot data from the MMC device directly.
#define   HSMCI_CMDR_SPCMD_EBO          (0x7 << HSMCI_CMDR_SPCMD_SHIFT) ///< (HSMCI_CMDR) End Boot Operation. This command allows the host processor to terminate the boot operation mode.
#define   HSMCI_CMDR_OPDCMD               11 ///< (HSMCI_CMDR) Open Drain Command
#define   HSMCI_CMDR_MAXLAT               12 ///< (HSMCI_CMDR) Max Latency for Command to Response
#define   HSMCI_CMDR_TRCMD_SHIFT          16
#define   HSMCI_CMDR_TRCMD_MASK        (0x3 << HSMCI_CMDR_TRCMD_SHIFT) ///< (HSMCI_CMDR) Transfer Command
#define   HSMCI_CMDR_TRCMD_NO_DATA         0 ///< (HSMCI_CMDR) No data transfer
#define   HSMCI_CMDR_TRCMD_START_DATA  (0x1 << HSMCI_CMDR_TRCMD_SHIFT) ///< (HSMCI_CMDR) Start data transfer
#define   HSMCI_CMDR_TRCMD_STOP_DATA   (0x2 << HSMCI_CMDR_TRCMD_SHIFT) ///< (HSMCI_CMDR) Stop data transfer
#define   HSMCI_CMDR_TRDIR                18 ///< (HSMCI_CMDR) Transfer Direction
#define   HSMCI_CMDR_TRTYP_SHIFT          19
#define   HSMCI_CMDR_TRTYP_MASK        (0x7 << HSMCI_CMDR_TRTYP_SHIFT) ///< (HSMCI_CMDR) Transfer Type
#define   HSMCI_CMDR_TRTYP_SINGLE          0 ///< (HSMCI_CMDR) MMC/SDCard Single Block
#define   HSMCI_CMDR_TRTYP_MULTIPLE    (0x1 << HSMCI_CMDR_TRTYP_SHIFT) ///< (HSMCI_CMDR) MMC/SDCard Multiple Block
#define   HSMCI_CMDR_TRTYP_STREAM      (0x2 << HSMCI_CMDR_TRTYP_SHIFT) ///< (HSMCI_CMDR) MMC Stream
#define   HSMCI_CMDR_TRTYP_BYTE        (0x4 << HSMCI_CMDR_TRTYP_SHIFT) ///< (HSMCI_CMDR) SDIO Byte
#define   HSMCI_CMDR_TRTYP_BLOCK       (0x5 << HSMCI_CMDR_TRTYP_SHIFT) ///< (HSMCI_CMDR) SDIO Block
#define   HSMCI_CMDR_IOSPCMD_SHIFT        24
#define   HSMCI_CMDR_IOSPCMD_MASK      (0x3 << HSMCI_CMDR_IOSPCMD_SHIFT) ///< (HSMCI_CMDR) SDIO Special Command
#define   HSMCI_CMDR_IOSPCMD_STD       (0x0 << HSMCI_CMDR_IOSPCMD_SHIFT) ///< (HSMCI_CMDR) Not an SDIO Special Command
#define   HSMCI_CMDR_IOSPCMD_SUSPEND   (0x1 << HSMCI_CMDR_IOSPCMD_SHIFT) ///< (HSMCI_CMDR) SDIO Suspend Command
#define   HSMCI_CMDR_IOSPCMD_RESUME    (0x2 << HSMCI_CMDR_IOSPCMD_SHIFT) ///< (HSMCI_CMDR) SDIO Resume Command
#define   HSMCI_CMDR_ATACS                26 ///< (HSMCI_CMDR) ATA with Command Completion Signal
#define   HSMCI_CMDR_BOOT_ACK             27 ///< (HSMCI_CMDR) Boot Operation Acknowledge.


/* HSMCI_BLKR : (HSMCI Offset: 0x18) Block Register */
#define   HSMCI_BLKR_BCNT_MASK         0xffff ///< (HSMCI_BLKR) MMC/SDIO Block Count - SDIO Byte Count
#define   HSMCI_BLKR_BCNT_MULTIPLE     0x0 ///< (HSMCI_BLKR) MMC/SDCARD Multiple BlockFrom 1 to 65635: Value 0 corresponds to an infinite block transfer.
#define   HSMCI_BLKR_BCNT_BYTE         0x4 ///< (HSMCI_BLKR) SDIO ByteFrom 1 to 512 bytes: Value 0 corresponds to a 512-byte transfer.Values from 0x200 to 0xFFFF are forbidden.
#define   HSMCI_BLKR_BCNT_BLOCK        0x5 ///< (HSMCI_BLKR) SDIO BlockFrom 1 to 511 blocks: Value 0 corresponds to an infinite block transfer.Values from 0x200 to 0xFFFF are forbidden.
#define   HSMCI_BLKR_BLKLEN_MASK       0xffff0000 ///< (HSMCI_BLKR) Data Block Length
#define   HSMCI_BLKR_BLKLEN_SHIFT      16


/* HSMCI_CSTOR : (HSMCI Offset: 0x1C) Completion Signal Timeout Register */
#define   HSMCI_CSTOR_CSTOCYC_MASK     0xf ///< (HSMCI_CSTOR) Completion Signal Timeout Cycle Number
#define   HSMCI_CSTOR_CSTOMUL_SHIFT      4
#define   HSMCI_CSTOR_CSTOMUL_MASK    (0x7 << HSMCI_CSTOR_CSTOMUL_SHIFT) ///< (HSMCI_CSTOR) Completion Signal Timeout Multiplier
#define   HSMCI_CSTOR_CSTOMUL_1       (0x0 << HSMCI_CSTOR_CSTOMUL_SHIFT) ///< (HSMCI_CSTOR) CSTOCYC x 1
#define   HSMCI_CSTOR_CSTOMUL_16      (0x1 << HSMCI_CSTOR_CSTOMUL_SHIFT) ///< (HSMCI_CSTOR) CSTOCYC x 16
#define   HSMCI_CSTOR_CSTOMUL_128     (0x2 << HSMCI_CSTOR_CSTOMUL_SHIFT) ///< (HSMCI_CSTOR) CSTOCYC x 128
#define   HSMCI_CSTOR_CSTOMUL_256     (0x3 << HSMCI_CSTOR_CSTOMUL_SHIFT) ///< (HSMCI_CSTOR) CSTOCYC x 256
#define   HSMCI_CSTOR_CSTOMUL_1024    (0x4 << HSMCI_CSTOR_CSTOMUL_SHIFT) ///< (HSMCI_CSTOR) CSTOCYC x 1024
#define   HSMCI_CSTOR_CSTOMUL_4096    (0x5 << HSMCI_CSTOR_CSTOMUL_SHIFT) ///< (HSMCI_CSTOR) CSTOCYC x 4096
#define   HSMCI_CSTOR_CSTOMUL_65536   (0x6 << HSMCI_CSTOR_CSTOMUL_SHIFT) ///< (HSMCI_CSTOR) CSTOCYC x 65536
#define   HSMCI_CSTOR_CSTOMUL_1048576 (0x7 << HSMCI_CSTOR_CSTOMUL_SHIFT) ///< (HSMCI_CSTOR) CSTOCYC x 1048576

/* HSMCI_SR : (HSMCI Offset: 0x40) Status Register */
#define HSMCI_SR_CMDRDY 0 ///< (HSMCI_SR) Command Ready
#define HSMCI_SR_RXRDY 1 ///< (HSMCI_SR) Receiver Ready
#define HSMCI_SR_TXRDY 2 ///< (HSMCI_SR) Transmit Ready
#define HSMCI_SR_BLKE 3 ///< (HSMCI_SR) Data Block Ended
#define HSMCI_SR_DTIP 4 ///< (HSMCI_SR) Data Transfer in Progress
#define HSMCI_SR_NOTBUSY 5 ///< (HSMCI_SR) HSMCI Not Busy
#define HSMCI_SR_SDIOIRQforSlotA 8 ///< (HSMCI_SR)
#define HSMCI_SR_SDIOIRQforSlotB 9 ///< (HSMCI_SR)
#define HSMCI_SR_SDIOWAIT 12 ///< (HSMCI_SR) SDIO Read Wait Operation Status
#define HSMCI_SR_CSRCV 13 ///< (HSMCI_SR) CE-ATA Completion Signal Received
#define HSMCI_SR_RINDE 16 ///< (HSMCI_SR) Response Index Error
#define HSMCI_SR_RDIRE 17 ///< (HSMCI_SR) Response Direction Error
#define HSMCI_SR_RCRCE 18 ///< (HSMCI_SR) Response CRC Error
#define HSMCI_SR_RENDE 19 ///< (HSMCI_SR) Response End Bit Error
#define HSMCI_SR_RTOE 20 ///< (HSMCI_SR) Response Time-out Error
#define HSMCI_SR_DCRCE 21 ///< (HSMCI_SR) Data CRC Error
#define HSMCI_SR_DTOE 22 ///< (HSMCI_SR) Data Time-out Error
#define HSMCI_SR_CSTOE 23 ///< (HSMCI_SR) Completion Signal Time-out Error
#define HSMCI_SR_BLKOVRE 24 ///< (HSMCI_SR) DMA Block Overrun Error
#define HSMCI_SR_DMADONE 25 ///< (HSMCI_SR) DMA Transfer done
#define HSMCI_SR_FIFOEMPTY 26 ///< (HSMCI_SR) FIFO empty flag
#define HSMCI_SR_XFRDONE 27 ///< (HSMCI_SR) Transfer Done flag
#define HSMCI_SR_ACKRCV 28 ///< (HSMCI_SR) Boot Operation Acknowledge Received
#define HSMCI_SR_ACKRCVE 29 ///< (HSMCI_SR) Boot Operation Acknowledge Error
#define HSMCI_SR_OVRE 30 ///< (HSMCI_SR) Overrun
#define HSMCI_SR_UNRE 31 ///< (HSMCI_SR) Underrun


/* HSMCI_IER : (HSMCI Offset: 0x44) Interrupt Enable Register */
#define HSMCI_IER_CMDRDY 0 ///< (HSMCI_IER) Command Ready Interrupt Enable
#define HSMCI_IER_RXRDY 1 ///< (HSMCI_IER) Receiver Ready Interrupt Enable
#define HSMCI_IER_TXRDY 2 ///< (HSMCI_IER) Transmit Ready Interrupt Enable
#define HSMCI_IER_BLKE 3 ///< (HSMCI_IER) Data Block Ended Interrupt Enable
#define HSMCI_IER_DTIP 4 ///< (HSMCI_IER) Data Transfer in Progress Interrupt Enable
#define HSMCI_IER_NOTBUSY 5 ///< (HSMCI_IER) Data Not Busy Interrupt Enable
#define HSMCI_IER_SDIOIRQforSlotA 8 ///< (HSMCI_IER)
#define HSMCI_IER_SDIOIRQforSlotB 9 ///< (HSMCI_IER)
#define HSMCI_IER_SDIOWAIT 12 ///< (HSMCI_IER) SDIO Read Wait Operation Status Interrupt Enable
#define HSMCI_IER_CSRCV 13 ///< (HSMCI_IER) Completion Signal Received Interrupt Enable
#define HSMCI_IER_RINDE 16 ///< (HSMCI_IER) Response Index Error Interrupt Enable
#define HSMCI_IER_RDIRE 17 ///< (HSMCI_IER) Response Direction Error Interrupt Enable
#define HSMCI_IER_RCRCE 18 ///< (HSMCI_IER) Response CRC Error Interrupt Enable
#define HSMCI_IER_RENDE 19 ///< (HSMCI_IER) Response End Bit Error Interrupt Enable
#define HSMCI_IER_RTOE 20 ///< (HSMCI_IER) Response Time-out Error Interrupt Enable
#define HSMCI_IER_DCRCE 21 ///< (HSMCI_IER) Data CRC Error Interrupt Enable
#define HSMCI_IER_DTOE 22 ///< (HSMCI_IER) Data Time-out Error Interrupt Enable
#define HSMCI_IER_CSTOE 23 ///< (HSMCI_IER) Completion Signal Timeout Error Interrupt Enable
#define HSMCI_IER_BLKOVRE 24 ///< (HSMCI_IER) DMA Block Overrun Error Interrupt Enable
#define HSMCI_IER_DMADONE 25 ///< (HSMCI_IER) DMA Transfer completed Interrupt Enable
#define HSMCI_IER_FIFOEMPTY 26 ///< (HSMCI_IER) FIFO empty Interrupt enable
#define HSMCI_IER_XFRDONE 27 ///< (HSMCI_IER) Transfer Done Interrupt enable
#define HSMCI_IER_ACKRCV 28 ///< (HSMCI_IER) Boot Acknowledge Interrupt Enable
#define HSMCI_IER_ACKRCVE 29 ///< (HSMCI_IER) Boot Acknowledge Error Interrupt Enable
#define HSMCI_IER_OVRE 30 ///< (HSMCI_IER) Overrun Interrupt Enable
#define HSMCI_IER_UNRE 31 ///< (HSMCI_IER) Underrun Interrupt Enable


/* HSMCI_IDR : (HSMCI Offset: 0x48) Interrupt Disable Register */
#define HSMCI_IDR_CMDRDY 0 ///< (HSMCI_IDR) Command Ready Interrupt Disable
#define HSMCI_IDR_RXRDY 1 ///< (HSMCI_IDR) Receiver Ready Interrupt Disable
#define HSMCI_IDR_TXRDY 2 ///< (HSMCI_IDR) Transmit Ready Interrupt Disable
#define HSMCI_IDR_BLKE 3 ///< (HSMCI_IDR) Data Block Ended Interrupt Disable
#define HSMCI_IDR_DTIP 4 ///< (HSMCI_IDR) Data Transfer in Progress Interrupt Disable
#define HSMCI_IDR_NOTBUSY 5 ///< (HSMCI_IDR) Data Not Busy Interrupt Disable
#define HSMCI_IDR_SDIOIRQforSlotA 8 ///< (HSMCI_IDR)
#define HSMCI_IDR_SDIOIRQforSlotB 9 ///< (HSMCI_IDR)
#define HSMCI_IDR_SDIOWAIT 12 ///< (HSMCI_IDR) SDIO Read Wait Operation Status Interrupt Disable
#define HSMCI_IDR_CSRCV 13 ///< (HSMCI_IDR) Completion Signal received interrupt Disable
#define HSMCI_IDR_RINDE 16 ///< (HSMCI_IDR) Response Index Error Interrupt Disable
#define HSMCI_IDR_RDIRE 17 ///< (HSMCI_IDR) Response Direction Error Interrupt Disable
#define HSMCI_IDR_RCRCE 18 ///< (HSMCI_IDR) Response CRC Error Interrupt Disable
#define HSMCI_IDR_RENDE 19 ///< (HSMCI_IDR) Response End Bit Error Interrupt Disable
#define HSMCI_IDR_RTOE 20 ///< (HSMCI_IDR) Response Time-out Error Interrupt Disable
#define HSMCI_IDR_DCRCE 21 ///< (HSMCI_IDR) Data CRC Error Interrupt Disable
#define HSMCI_IDR_DTOE 22 ///< (HSMCI_IDR) Data Time-out Error Interrupt Disable
#define HSMCI_IDR_CSTOE 23 ///< (HSMCI_IDR) Completion Signal Time out Error Interrupt Disable
#define HSMCI_IDR_BLKOVRE 24 ///< (HSMCI_IDR) DMA Block Overrun Error Interrupt Disable
#define HSMCI_IDR_DMADONE 25 ///< (HSMCI_IDR) DMA Transfer completed Interrupt Disable
#define HSMCI_IDR_FIFOEMPTY 26 ///< (HSMCI_IDR) FIFO empty Interrupt Disable
#define HSMCI_IDR_XFRDONE 27 ///< (HSMCI_IDR) Transfer Done Interrupt Disable
#define HSMCI_IDR_ACKRCV 28 ///< (HSMCI_IDR) Boot Acknowledge Interrupt Disable
#define HSMCI_IDR_ACKRCVE 29 ///< (HSMCI_IDR) Boot Acknowledge Error Interrupt Disable
#define HSMCI_IDR_OVRE 30 ///< (HSMCI_IDR) Overrun Interrupt Disable
#define HSMCI_IDR_UNRE 31 ///< (HSMCI_IDR) Underrun Interrupt Disable

/* HSMCI_IMR : (HSMCI Offset: 0x4C) Interrupt Mask Register */
#define HSMCI_IMR_CMDRDY 0 ///< (HSMCI_IMR) Command Ready Interrupt Mask
#define HSMCI_IMR_RXRDY 1 ///< (HSMCI_IMR) Receiver Ready Interrupt Mask
#define HSMCI_IMR_TXRDY 2 ///< (HSMCI_IMR) Transmit Ready Interrupt Mask
#define HSMCI_IMR_BLKE 3 ///< (HSMCI_IMR) Data Block Ended Interrupt Mask
#define HSMCI_IMR_DTIP 4 ///< (HSMCI_IMR) Data Transfer in Progress Interrupt Mask
#define HSMCI_IMR_NOTBUSY 5 ///< (HSMCI_IMR) Data Not Busy Interrupt Mask
#define HSMCI_IMR_SDIOIRQforSlotA 8 ///< (HSMCI_IMR)
#define HSMCI_IMR_SDIOIRQforSlotB 9 ///< (HSMCI_IMR)
#define HSMCI_IMR_SDIOWAIT 12 ///< (HSMCI_IMR) SDIO Read Wait Operation Status Interrupt Mask
#define HSMCI_IMR_CSRCV 13 ///< (HSMCI_IMR) Completion Signal Received Interrupt Mask
#define HSMCI_IMR_RINDE 16 ///< (HSMCI_IMR) Response Index Error Interrupt Mask
#define HSMCI_IMR_RDIRE 17 ///< (HSMCI_IMR) Response Direction Error Interrupt Mask
#define HSMCI_IMR_RCRCE 18 ///< (HSMCI_IMR) Response CRC Error Interrupt Mask
#define HSMCI_IMR_RENDE 19 ///< (HSMCI_IMR) Response End Bit Error Interrupt Mask
#define HSMCI_IMR_RTOE 20 ///< (HSMCI_IMR) Response Time-out Error Interrupt Mask
#define HSMCI_IMR_DCRCE 21 ///< (HSMCI_IMR) Data CRC Error Interrupt Mask
#define HSMCI_IMR_DTOE 22 ///< (HSMCI_IMR) Data Time-out Error Interrupt Mask
#define HSMCI_IMR_CSTOE 23 ///< (HSMCI_IMR) Completion Signal Time-out Error Interrupt Mask
#define HSMCI_IMR_BLKOVRE 24 ///< (HSMCI_IMR) DMA Block Overrun Error Interrupt Mask
#define HSMCI_IMR_DMADONE 25 ///< (HSMCI_IMR) DMA Transfer Completed Interrupt Mask
#define HSMCI_IMR_FIFOEMPTY 26 ///< (HSMCI_IMR) FIFO Empty Interrupt Mask
#define HSMCI_IMR_XFRDONE 27 ///< (HSMCI_IMR) Transfer Done Interrupt Mask
#define HSMCI_IMR_ACKRCV 28 ///< (HSMCI_IMR) Boot Operation Acknowledge Received Interrupt Mask
#define HSMCI_IMR_ACKRCVE 29 ///< (HSMCI_IMR) Boot Operation Acknowledge Error Interrupt Mask
#define HSMCI_IMR_OVRE 30 ///< (HSMCI_IMR) Overrun Interrupt Mask
#define HSMCI_IMR_UNRE 31 ///< (HSMCI_IMR) Underrun Interrupt Mask


/* HSMCI_DMA : (HSMCI Offset: 0x50) DMA Configuration Register */
#define HSMCI_DMA_OFFSET_MASK    0x3 ///< (HSMCI_DMA) DMA Write Buffer Offset
#define HSMCI_DMA_CHKSIZE          4 ///< (HSMCI_DMA) DMA Channel Read and Write Chunk Size
#define HSMCI_DMA_DMAEN            8 ///< (HSMCI_DMA) DMA Hardware Handshaking Enable
#define HSMCI_DMA_ROPT            12 ///< (HSMCI_DMA) Read Optimization with padding


/* HSMCI_CFG : (HSMCI Offset: 0x54) Configuration Register */
#define HSMCI_CFG_FIFOMODE        0 ///< (HSMCI_CFG) HSMCI Internal FIFO control mode
#define HSMCI_CFG_FERRCTRL        4 ///< (HSMCI_CFG) Flow Error flag reset control mode
#define HSMCI_CFG_HSMODE          8 ///< (HSMCI_CFG) High Speed Mode
#define HSMCI_CFG_LSYNC          12 ///< (HSMCI_CFG) Synchronize on the last block

/* HSMCI_WPMR : (HSMCI Offset: 0xE4) Write Protection Mode Register */
#define HSMCI_WPMR_WP_EN          0 ///< (HSMCI_WPMR) Write Protection Enable
#define HSMCI_WPMR_WP_KEY_MASK    0xffffff00 ///< (HSMCI_WPMR) Write Protection Key password

/* HSMCI_WPSR : (HSMCI Offset: 0xE8) Write Protection Status Register */
#define HSMCI_WPSR_WP_VS_MASK     0xf ///< (HSMCI_WPSR) Write Protection Violation Status
#define HSMCI_WPSR_WP_VS_NONE     0x0 ///< (HSMCI_WPSR) No Write Protection Violation occurred since the last read of this register (WP_SR)
#define HSMCI_WPSR_WP_VS_WRITE    0x1 ///< (HSMCI_WPSR) Write Protection detected unauthorized attempt to write a control register had occurred (since the last read.)
#define HSMCI_WPSR_WP_VS_RESET    0x2 ///< (HSMCI_WPSR) Software reset had been performed while Write Protection was enabled (since the last read).
#define HSMCI_WPSR_WP_VS_BOTH     0x3 ///< (HSMCI_WPSR) Both Write Protection violation and software reset with Write Protection enabled have occurred since the last read.
#define HSMCI_WPSR_WP_VSRC_MASK   0xffff00 ///< (HSMCI_WPSR) Write Protection Violation SouRCe

#endif /* SAM3_HSMCI_H */

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
  * Copyright 2012 Develer S.r.l. (http://www.develer.com/)
  * All Rights Reserved.
  * -->
  *
  * \brief Ethernet driver for STM-32 CPU family.
  *
  * \author Mattia Barbon <mattia@develer.com>
  */

#ifndef ETH_STM32_H
#define ETH_STM32_H

#define MAC_RX_BUFFER      1600
#define MAC_TX_BUFFER      1600

#define MAC_TX_DESCRIPTORS 4
#define MAC_TX_BUFFERS     (MAC_TX_DESCRIPTORS * 2)

typedef struct stm32_eth
{
	reg32_t MACCR;
	reg32_t MACFFR;
	reg32_t MACHTHR;
	reg32_t MACHTLR;
	reg32_t MACMIIAR;
	reg32_t MACMIIDR;
	reg32_t MACFCR;
	reg32_t MACVLANTR;             /*    8 */
	uint32_t RESERVED0[2];
	reg32_t MACRWUFFR;             /*   11 */
	reg32_t MACPMTCSR;
	uint32_t RESERVED1[1];
	reg32_t MACDBGR;               /* added for STM32F2xx */
	reg32_t MACSR;                 /*   15 */
	reg32_t MACIMR;
	reg32_t MACA0HR;
	reg32_t MACA0LR;
	reg32_t MACA1HR;
	reg32_t MACA1LR;
	reg32_t MACA2HR;
	reg32_t MACA2LR;
	reg32_t MACA3HR;
	reg32_t MACA3LR;               /*   24 */
	uint32_t RESERVED2[40];
	reg32_t MMCCR;                 /*   65 */
	reg32_t MMCRIR;
	reg32_t MMCTIR;
	reg32_t MMCRIMR;
	reg32_t MMCTIMR;               /*   69 */
	uint32_t RESERVED3[14];
	reg32_t MMCTGFSCCR;            /*   84 */
	reg32_t MMCTGFMSCCR;
	uint32_t RESERVED4[5];
	reg32_t MMCTGFCR;
	uint32_t RESERVED5[10];
	reg32_t MMCRFCECR;
	reg32_t MMCRFAECR;
	uint32_t RESERVED6[10];
	reg32_t MMCRGUFCR;
	uint32_t RESERVED7[334];
	reg32_t PTPTSCR;
	reg32_t PTPSSIR;
	reg32_t PTPTSHR;
	reg32_t PTPTSLR;
	reg32_t PTPTSHUR;
	reg32_t PTPTSLUR;
	reg32_t PTPTSAR;
	reg32_t PTPTTHR;
	reg32_t PTPTTLR;
	reg32_t RESERVED8;
	reg32_t PTPTSSR;  /* added for STM32F2xx */
	uint32_t RESERVED9[565];
	reg32_t DMABMR;
	reg32_t DMATPDR;
	reg32_t DMARPDR;
	reg32_t DMARDLAR;
	reg32_t DMATDLAR;
	reg32_t DMASR;
	reg32_t DMAOMR;
	reg32_t DMAIER;
	reg32_t DMAMFBOCR;
	reg32_t DMARSWTR;  /* added for STM32F2xx */
	uint32_t RESERVED10[8];
	reg32_t DMACHTDR;
	reg32_t DMACHRDR;
	reg32_t DMACHTBAR;
	reg32_t DMACHRBAR;
} stm32_eth;

/* for MACCR */
#define MAC_RX_ENABLE         BV(2)
#define MAC_TX_ENABLE         BV(3)
#define MAC_FULL_DUPLEX       BV(11)
#define MAC_100_MBIT          BV(14)

/* for DMAOMR */
#define MAC_DMA_TSF           BV(21)
#define MAC_DMA_TX            BV(13)
#define MAC_DMA_RX            BV(1)

/* for DMASR and DMAIER */
#define MAC_DMA_TX_INT        BV(0)
#define MAC_DMA_TX_NOBUF_INT  BV(2)
#define MAC_DMA_RX_INT        BV(6)

/* enable abnormal and normal interrupts */
#define MAC_DMA_ABN_INT       BV(15)
#define MAC_DMA_NORM_INT      BV(16)

#define ETH    ((stm32_eth *) ETH_BASE)

/* DMA descriptors */
#define MAC_DESC0_DMA_OWN  BV(31) /* owned by DMA */

#define MAC_DESC0_TX_ES    BV(15) /* error summary */
#define MAC_DESC0_TX_EOR   BV(21) /* end of descriptor ring */
#define MAC_DESC0_TX_TCH   BV(20) /* second address chained */
#define MAC_DESC0_TX_LS    BV(29) /* marks last frame buffer */
#define MAC_DESC0_TX_FS    BV(28) /* marks first frame buffer */
#define MAC_DESC0_TX_IC    BV(30) /* generate interrupt on completion */

#define MAC_DESC1_RX_EOR   BV(15)

struct stm32_eth_dma_descr
{
	reg32_t descr0;
	reg32_t descr1;
	union
	{
		struct
		{
			reg32_t time_stamp_low;
			reg32_t time_stamp_high;
		};
		struct
		{
			void *buffer1;
			void *buffer2;
		};
	};
} ALIGNED(4);

/* PHY configuration */
#define DP83848_PHY_ADDRESS       0x01 /* For STM322xG-EVAL Board */

#define PHY_BCR 0
#define PHY_BSR 1
#define PHY_SR  16

#define PHY_SR_FULL_DUPLEX              BV(2)
#define PHY_SR_10_MBIT                  BV(1)

#define PHY_WRITE_TIMEOUT               ((uint32_t)0x0004FFFF)
#define PHY_READ_TIMEOUT                ((uint32_t)0x0004FFFF)

#define PHY_Reset                       ((uint16_t)0x8000)      /*!< PHY Reset */
#define PHY_Link_Up                     ((uint16_t)0x0004)      /*!< Valid link established */
#define PHY_Autonegotiation             ((uint16_t)0x1000)      /*!< Enable auto-negotiation function */
#define PHY_Autonegotiation_Complete    ((uint16_t)0x0020)      /*!< Auto-Negotiation process completed */

#define ETH_MACMIIAR_MR   ((uint32_t)0x000007C0)  /* MII register in the selected PHY */
#define ETH_MACMIIAR_CR   ((uint32_t)0x0000001C)  /* CR clock range: 6 cases */
#define ETH_MACMIIAR_CR_MASK ((uint32_t)0xFFFFFFE3)
#define ETH_MACMIIAR_MW   ((uint32_t)0x00000002)  /* MII write */
#define ETH_MACMIIAR_MB   ((uint32_t)0x00000001)  /* MII busy */
#define ETH_MACMIIAR_PA   ((uint32_t)0x0000F800)  /* Physical layer address */

#define ETH_MACMIIAR_CR_16   (2 << 2)
#define ETH_MACMIIAR_CR_26   (3 << 2)
#define ETH_MACMIIAR_CR_42   (0 << 2)
#define ETH_MACMIIAR_CR_62   (1 << 2)

#endif /* ETH_STM32_H */

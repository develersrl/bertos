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
 * -->
 *
 * \brief HSMCI driver implementation.
 *
 * \author Daniele Basile <asterix@develer.com>
 */


#include "hsmci_sam3.h"

#include <drv/timer.h>
#include <cpu/irq.h>
#include <drv/irq_cm3.h>

#include <io/cm3.h>

/** DMA Transfer Descriptor as well as Linked List Item */
typedef struct DmacDesc
{
    uint32_t src_addr;     /**< Source buffer address */
    uint32_t dst_addr;     /**< Destination buffer address */
    uint32_t ctrl_a;       /**< Control A register settings */
    uint32_t ctrl_b;       /**< Control B register settings */
    uint32_t dsc_addr;     /**< Next descriptor address */
} DmacDesc;




#define HSMCI_CLK_DIV(RATE)     ((CPU_FREQ / (RATE << 1)) - 1)


#define HSMCI_RESP_ERROR_MASK   (BV(HSMCI_SR_RINDE) | BV(HSMCI_SR_RDIRE) \
	  | BV(HSMCI_SR_RENDE)| BV(HSMCI_SR_RTOE))

#define HSMCI_DATA_ERROR_MASK   (BV(HSMCI_SR_DCRCE) | BV(HSMCI_SR_DTOE))

#define HSMCI_READY_MASK     (BV(HSMCI_SR_CMDRDY) | BV(HSMCI_SR_NOTBUSY))
#define HSMCI_WAIT()\
	do { \
		cpu_relax(); \
	} while (!(HSMCI_SR & BV(HSMCI_SR_CMDRDY)))


#define HSMCI_WAIT_DATA_RDY()\
	do { \
		cpu_relax(); \
	} while (!(HSMCI_SR & BV(HSMCI_SR_RXRDY)))

#define HSMCI_HW_INIT()  \
do { \
	PIOA_PDR = BV(19) | BV(20) | BV(21) | BV(22) | BV(23) | BV(24); \
	PIO_PERIPH_SEL(PIOA_BASE, BV(19) | BV(20) | BV(21) | BV(22) | BV(23) | BV(24), PIO_PERIPH_A); \
} while (0)


#define STROBE_ON()   PIOB_SODR = BV(13)
#define STROBE_OFF()  PIOB_CODR = BV(13)
#define STROBE_INIT() \
	do { \
		PIOB_OER = BV(13); \
		PIOB_PER = BV(13); \
	} while(0)

static DECLARE_ISR(hsmci_irq)
{
	uint32_t status = HSMCI_SR;
	if (status & BV(HSMCI_IER_DMADONE))
	{
	}
}


static DECLARE_ISR(dmac_irq)
{
	uint32_t stat = DMAC_EBCISR;

	if (stat & BV(DMAC_EBCISR_ERR3))
	{
		kprintf("err %08lx\n", stat);
	}
}

void hsmci_readResp(uint32_t *resp, size_t len)
{
	ASSERT(resp);

	for (size_t i = 0; i < len ; i++)
		resp[i] = HSMCI_RSPR;
}

bool hsmci_sendCmd(uint8_t index, uint32_t argument, uint32_t reply_type)
{
	STROBE_ON();
	HSMCI_WAIT();

	HSMCI_ARGR = argument;
	HSMCI_CMDR = index | reply_type | BV(HSMCI_CMDR_MAXLAT);

	uint32_t status = HSMCI_SR;
	while (!(status & BV(HSMCI_SR_CMDRDY)))
	{
		if (status & HSMCI_RESP_ERROR_MASK)
			return status;

		cpu_relax();

		status = HSMCI_SR;
	}

	STROBE_OFF();
	return 0;
}

INLINE void hsmci_setBlockSize(size_t blk_size)
{
	HSMCI_DMA |= BV(HSMCI_DMA_DMAEN);
	HSMCI_BLKR = blk_size << HSMCI_BLKR_BLKLEN_SHIFT;
}

void hsmci_prgTxDMA(const uint32_t *buf, size_t word_num, size_t blk_size)
{

	hsmci_setBlockSize(blk_size);

	DMAC_CHDR = BV(DMAC_CHDR_DIS0);

	DMAC_SADDR0 = (uint32_t)buf;
	DMAC_DADDR0 = (uint32_t)&HSMCI_TDR;
	DMAC_DSCR0 = 0;

	DMAC_CFG0 = BV(DMAC_CFG_DST_H2SEL) | DMAC_CFG_FIFOCFG_ALAP_CFG | (0x1 << DMAC_CFG_AHB_PROT_SHIFT);
	DMAC_CTRLA0 = (word_num & DMAC_CTRLA_BTSIZE_MASK) |
		DMAC_CTRLA_SRC_WIDTH_WORD | DMAC_CTRLA_DST_WIDTH_WORD;
	DMAC_CTRLB0 = (BV(DMAC_CTRLB_SRC_DSCR) | BV(DMAC_CTRLB_DST_DSCR) | DMAC_CTRLB_FC_MEM2PER_DMA_FC |
					DMAC_CTRLB_DST_INCR_FIXED | DMAC_CTRLB_SRC_INCR_INCREMENTING | BV(DMAC_CTRLB_IEN));

	ASSERT(!(DMAC_CHSR & BV(DMAC_CHSR_ENA0)));
	DMAC_CHER = BV(DMAC_CHER_ENA0);

}

void hsmci_prgRxDMA(uint32_t *buf, size_t word_num, size_t blk_size)
{
	hsmci_setBlockSize(blk_size);

	DMAC_CHDR = BV(DMAC_CHDR_DIS0);

	DMAC_SADDR0 = (uint32_t)&HSMCI_RDR;
	DMAC_DADDR0 = (uint32_t)buf;
	DMAC_DSCR0 = 0;

	DMAC_CFG0 = BV(DMAC_CFG_SRC_H2SEL) | DMAC_CFG_FIFOCFG_ALAP_CFG | (0x1 << DMAC_CFG_AHB_PROT_SHIFT);
	DMAC_CTRLA0 = (word_num & DMAC_CTRLA_BTSIZE_MASK) |
		DMAC_CTRLA_SRC_WIDTH_WORD | DMAC_CTRLA_DST_WIDTH_WORD;
	DMAC_CTRLB0 = (BV(DMAC_CTRLB_SRC_DSCR) | BV(DMAC_CTRLB_DST_DSCR) | DMAC_CTRLB_FC_PER2MEM_DMA_FC |
					DMAC_CTRLB_DST_INCR_INCREMENTING | DMAC_CTRLB_SRC_INCR_FIXED | BV(DMAC_CTRLB_IEN));

	ASSERT(!(DMAC_CHSR & BV(DMAC_CHSR_ENA0)));
	DMAC_CHER = BV(DMAC_CHER_ENA0);
}


void hsmci_waitTransfer(void)
{
	while (!(HSMCI_SR & BV(HSMCI_SR_XFRDONE)))
		cpu_relax();
}

void hsmci_setSpeed(uint32_t data_rate, int flag)
{
	if (flag)
		HSMCI_CFG |= BV(HSMCI_CFG_HSMODE);
	else
		HSMCI_CFG &= ~BV(HSMCI_CFG_HSMODE);

	HSMCI_MR = HSMCI_CLK_DIV(data_rate) | ((0x7u << HSMCI_MR_PWSDIV_SHIFT) & HSMCI_MR_PWSDIV_MASK);

	timer_delay(10);
}

void hsmci_init(Hsmci *hsmci)
{
	(void)hsmci;

	HSMCI_HW_INIT();
	STROBE_INIT();

	pmc_periphEnable(HSMCI_ID);
	HSMCI_CR = BV(HSMCI_CR_SWRST);
	HSMCI_CR = BV(HSMCI_CR_PWSDIS) | BV(HSMCI_CR_MCIDIS);
	HSMCI_IDR = 0xFFFFFFFF;

	HSMCI_DTOR = 0xFF | HSMCI_DTOR_DTOMUL_1048576;
	HSMCI_CSTOR = 0xFF | HSMCI_CSTOR_CSTOMUL_1048576;
	HSMCI_MR = HSMCI_CLK_DIV(HSMCI_INIT_SPEED) | ((0x7u << HSMCI_MR_PWSDIV_SHIFT) & HSMCI_MR_PWSDIV_MASK) | BV(HSMCI_MR_RDPROOF);
	HSMCI_CFG = BV(HSMCI_CFG_FIFOMODE) | BV(HSMCI_CFG_FERRCTRL);

	sysirq_setHandler(INT_HSMCI, hsmci_irq);
	HSMCI_CR = BV(HSMCI_CR_MCIEN);
	HSMCI_DMA = 0;

	//init DMAC
	DMAC_EBCIDR = 0x3FFFFF;
	DMAC_CHDR = 0x1F;


	pmc_periphEnable(DMAC_ID);
	DMAC_EN = BV(DMAC_EN_ENABLE);
	sysirq_setHandler(INT_DMAC, dmac_irq);

	DMAC_EBCIER = BV(DMAC_EBCIER_BTC0) | BV(DMAC_EBCIER_ERR0);
}

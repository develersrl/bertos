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



#define HSMCI_INIT_SPEED  400000
#define HSMCI_CLK_DIV     ((CPU_FREQ / (HSMCI_INIT_SPEED << 1)) - 1)

#define HSMCI_ERROR_MASK   (BV(HSMCI_SR_RINDE)    | \
							BV(HSMCI_SR_RDIRE)    | \
							BV(HSMCI_SR_RCRCE)    | \
							BV(HSMCI_SR_RENDE)    | \
							BV(HSMCI_SR_RTOE)     | \
							BV(HSMCI_SR_DCRCE)    | \
							BV(HSMCI_SR_DTOE)     | \
							BV(HSMCI_SR_CSTOE)    | \
							BV(HSMCI_SR_BLKOVRE)  | \
							BV(HSMCI_SR_ACKRCVE))


#define HSMCI_RESP_ERROR_MASK   (BV(HSMCI_SR_RINDE) | BV(HSMCI_SR_RDIRE) \
	  | BV(HSMCI_SR_RENDE)| BV(HSMCI_SR_RTOE))

#define HSMCI_READY_MASK     (BV(HSMCI_SR_CMDRDY) | BV(HSMCI_SR_NOTBUSY))
#define HSMCI_WAIT()\
	do { \
		cpu_relax(); \
	} while (!(HSMCI_SR & BV(HSMCI_SR_CMDRDY)))


#define HSMCI_WAIT_DATA_RDY()\
	do { \
		cpu_relax(); \
	} while (!(HSMCI_SR & BV(HSMCI_SR_RXRDY)))

#define HSMCI_ERROR()   (HSMCI_SR & HSMCI_ERROR_MASK)

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
	if (HSMCI_SR & BV(HSMCI_IER_RTOE))
	{
		HSMCI_ARGR = 0;
		HSMCI_CMDR = 0 | HSMCI_CMDR_RSPTYP_NORESP | BV(HSMCI_CMDR_OPDCMD);
	}
}

void hsmci_readResp(void *resp, size_t len)
{
	ASSERT(resp);
	uint32_t *r = (uint32_t *)resp;

	for (size_t i = 0; i < len ; i++)
		r[i] = HSMCI_RSPR;
}

bool hsmci_sendCmd(uint8_t index, uint32_t argument, uint32_t reply_type)
{
	STROBE_ON();
	HSMCI_WAIT();

	HSMCI_ARGR = argument;
	HSMCI_CMDR = index | reply_type | BV(HSMCI_CMDR_MAXLAT) | BV(HSMCI_CMDR_OPDCMD);

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

void hsmci_setBlkSize(size_t blk_size)
{
	HSMCI_DMA = BV(HSMCI_DMA_DMAEN);
	HSMCI_BLKR = (blk_size << HSMCI_BLKR_BLKLEN_SHIFT);
}

bool hsmci_read(uint32_t *buf, size_t word_num)
{
	ASSERT(buf);
	ASSERT(!(DMAC_CHSR & BV(DMAC_CHSR_ENA0)));

	kprintf("DMAC status %08lx channel st %08lx\n", DMAC_EBCISR, DMAC_CHSR);

	DMAC_SADDR0 = 0x40000200U;
	DMAC_DADDR0 = (uint32_t)buf;
	DMAC_DSCR0 = 0;

	DMAC_CTRLA0 = word_num | DMAC_CTRLA_SRC_WIDTH_WORD | DMAC_CTRLA_DST_WIDTH_WORD;
	DMAC_CTRLB0 = (BV(DMAC_CTRLB_SRC_DSCR) | DMAC_CTRLB_FC_PER2MEM_DMA_FC |
					DMAC_CTRLB_SRC_INCR_FIXED | DMAC_CTRLB_DST_INCR_INCREMENTING | BV(DMAC_CTRLB_IEN));

	ASSERT(!(DMAC_CHSR & BV(DMAC_CHSR_ENA0)));
	DMAC_CHER = BV(DMAC_CHER_ENA0);

	while (!(HSMCI_SR & BV(HSMCI_SR_XFRDONE)))
		cpu_relax();

	DMAC_CHDR = BV(DMAC_CHDR_DIS0);
	return 0;
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
	HSMCI_MR = HSMCI_CLK_DIV | ((0x7u << HSMCI_MR_PWSDIV_SHIFT) & HSMCI_MR_PWSDIV_MASK) | BV(HSMCI_MR_RDPROOF);
	HSMCI_SDCR = 0;
	HSMCI_CFG = BV(HSMCI_CFG_FIFOMODE) | BV(HSMCI_CFG_FERRCTRL);

	sysirq_setHandler(INT_HSMCI, hsmci_irq);
	HSMCI_CR = BV(HSMCI_CR_MCIEN);
	HSMCI_DMA &= ~BV(HSMCI_DMA_DMAEN);

	//init DMAC
	DMAC_EBCIDR = 0x3FFFFF;
	DMAC_CHDR = BV(DMAC_CHDR_DIS0);

	DMAC_CFG0 = 0;
	DMAC_CFG0 = BV(DMAC_CFG_SRC_H2SEL) | DMAC_CFG_FIFOCFG_ALAP_CFG | BV(DMAC_CFG_SOD);

	pmc_periphEnable(DMAC_ID);
	DMAC_EN = BV(DMAC_EN_ENABLE);

	//HSMCI_IER = BV(HSMCI_IER_RTOE);
}

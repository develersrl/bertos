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
#include "hw/hw_sd.h"

#include <drv/timer.h>
#include <drv/irq_cm3.h>
#include <drv/dmac_sam3.h>

#include <mware/event.h>

#include <cpu/irq.h>

#include <io/cm3.h>


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


#define HSMCI_DMAC_CH    3


void hsmci_readResp(uint32_t *resp, size_t len)
{
	ASSERT(resp);

	for (size_t i = 0; i < len ; i++)
		resp[i] = HSMCI_RSPR;
}

bool hsmci_sendCmd(uint8_t index, uint32_t argument, uint32_t reply_type)
{
	HSMCI_WAIT();

	HSMCI_ARGR = argument;
	HSMCI_CMDR = index | reply_type | BV(HSMCI_CMDR_MAXLAT);

	uint32_t status;
	do {
		status = HSMCI_SR;

		if (status & HSMCI_RESP_ERROR_MASK)
			return status;

		cpu_relax();

	} while (!(status & BV(HSMCI_SR_CMDRDY)));

	return 0;
}

#define HSMCI_WRITE_DMAC_CFG  (BV(DMAC_CFG_DST_H2SEL) | \
							   BV(DMAC_CFG_SOD) | \
						     ((0 << DMAC_CFG_DST_PER_SHIFT) & DMAC_CFG_DST_PER_MASK) | \
						      (0 & DMAC_CFG_SRC_PER_MASK))

#define HSMCI_WRITE_DMAC_CTRLB  (BV(DMAC_CTRLB_SRC_DSCR) | BV(DMAC_CTRLB_DST_DSCR) | \
								DMAC_CTRLB_FC_MEM2PER_DMA_FC | \
								DMAC_CTRLB_DST_INCR_FIXED | DMAC_CTRLB_SRC_INCR_INCREMENTING)

#define HSMCI_WRITE_DMAC_CTRLA  (DMAC_CTRLA_SRC_WIDTH_WORD | \
								DMAC_CTRLA_DST_WIDTH_WORD)

#define HSMCI_READ_DMAC_CFG  (BV(DMAC_CFG_SRC_H2SEL) | \
							  BV(DMAC_CFG_SOD) | \
							((0 << DMAC_CFG_DST_PER_SHIFT) & DMAC_CFG_DST_PER_MASK) | \
							 (0 & DMAC_CFG_SRC_PER_MASK))

#define HSMCI_READ_DMAC_CTRLB  (BV(DMAC_CTRLB_SRC_DSCR) | BV(DMAC_CTRLB_DST_DSCR) | \
								DMAC_CTRLB_FC_PER2MEM_DMA_FC | \
								DMAC_CTRLB_DST_INCR_INCREMENTING | DMAC_CTRLB_SRC_INCR_FIXED)

#define HSMCI_READ_DMAC_CTRLA  (DMAC_CTRLA_SRC_WIDTH_WORD | \
								DMAC_CTRLA_DST_WIDTH_WORD)


void hsmci_write(const uint32_t *buf, size_t word_num, size_t blk_size)
{
	HSMCI_DMA |= BV(HSMCI_DMA_DMAEN);
	HSMCI_BLKR = (blk_size << HSMCI_BLKR_BLKLEN_SHIFT) & ~0x30000;

	dmac_setSources(HSMCI_DMAC_CH, (uint32_t)buf, (uint32_t)&HSMCI_TDR);
	dmac_configureDmac(HSMCI_DMAC_CH, word_num, HSMCI_WRITE_DMAC_CFG, HSMCI_WRITE_DMAC_CTRLA, HSMCI_WRITE_DMAC_CTRLB);
	dmac_start(HSMCI_DMAC_CH);
}

void hsmci_read(uint32_t *buf, size_t word_num, size_t blk_size)
{
	HSMCI_DMA |= BV(HSMCI_DMA_DMAEN);
	HSMCI_BLKR = blk_size << HSMCI_BLKR_BLKLEN_SHIFT;

	dmac_setSources(HSMCI_DMAC_CH, (uint32_t)&HSMCI_RDR, (uint32_t)buf);
	dmac_configureDmac(HSMCI_DMAC_CH, word_num, HSMCI_READ_DMAC_CFG, HSMCI_READ_DMAC_CTRLA, HSMCI_READ_DMAC_CTRLB);
	dmac_start(HSMCI_DMAC_CH);
}


void hsmci_waitTransfer(void)
{
	while (!(HSMCI_SR & BV(HSMCI_SR_XFRDONE)))
		cpu_relax();
}

void hsmci_setSpeed(uint32_t data_rate, int flag)
{
	if (flag & HSMCI_HS_MODE)
		HSMCI_CFG |= BV(HSMCI_CFG_HSMODE);
	else
		HSMCI_CFG &= ~BV(HSMCI_CFG_HSMODE);

	HSMCI_MR = HSMCI_CLK_DIV(data_rate);

	timer_delay(10);
}

void hsmci_init(Hsmci *hsmci)
{
	(void)hsmci;

	SD_PIN_INIT();

	pmc_periphEnable(HSMCI_ID);
	HSMCI_CR = BV(HSMCI_CR_SWRST);
	HSMCI_CR = BV(HSMCI_CR_PWSDIS) | BV(HSMCI_CR_MCIDIS);
	HSMCI_IDR = 0xFFFFFFFF;

	HSMCI_DTOR = 0xFF | HSMCI_DTOR_DTOMUL_1048576;
	HSMCI_CSTOR = 0xFF | HSMCI_CSTOR_CSTOMUL_1048576;
	HSMCI_MR = HSMCI_CLK_DIV(HSMCI_INIT_SPEED);
	HSMCI_CFG = BV(HSMCI_CFG_FIFOMODE) | BV(HSMCI_CFG_FERRCTRL);

	HSMCI_CR = BV(HSMCI_CR_MCIEN);
	HSMCI_DMA = 0;

	dmac_enableCh(HSMCI_DMAC_CH, NULL);
}

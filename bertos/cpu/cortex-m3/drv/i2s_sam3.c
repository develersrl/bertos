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
 * \brief I2S driver implementation.
 *
 * \author Daniele Basile <asterix@develer.com>
 */


#include "cfg/cfg_i2s.h"

// Define log settings for cfg/log.h.
#define LOG_LEVEL         I2S_LOG_LEVEL
#define LOG_FORMAT        I2S_LOG_FORMAT
#include <cfg/log.h>

#include <drv/timer.h>
#include <drv/i2s.h>
#include <drv/dmac_sam3.h>

#include <mware/event.h>

#include <cpu/irq.h>

#include <io/cm3.h>


#define I2S_DMAC_CH    1

struct I2sHardware
{
	bool end;
};

struct I2sHardware i2s_hw;
static Event data_ready;

/* We divite for 2 because the min clock for i2s i MCLK/2 */
#define MCK_DIV     (CPU_FREQ / (48000 * CONFIG_WORD_BIT_SIZE * CONFIG_CHANNEL_NUM * 2))
#define DATALEN     ((CONFIG_WORD_BIT_SIZE - 1) & SSC_DATLEN_MASK)
#define DELAY       ((CONFIG_DELAY << SSC_STTDLY_SHIFT) & SSC_STTDLY_MASK)
#define PERIOD      ((CONFIG_PERIOD << (SSC_PERIOD_SHIFT)) & SSC_PERIOD_MASK)
#define DATNB       ((CONFIG_WORD_PER_FRAME << SSC_DATNB_SHIFT) & SSC_DATNB_MASK)
#define FSLEN       ((CONFIG_FRAME_SYNC_SIZE << SSC_FSLEN_SHIFT) & SSC_FSLEN_MASK)
#define EXTRA_FSLEN (CONFIG_EXTRA_FRAME_SYNC_SIZE << SSC_FSLEN_EXT)


DmacDesc lli0;
DmacDesc lli1;
DmacDesc *curr;
DmacDesc *next;
DmacDesc *prev;

static uint8_t *sample_buff;
static size_t next_idx = 0;
static size_t chunk_size = 0;
static size_t remaing_size = 0;

static uint32_t cfg;
static uint32_t ctrla;
static uint32_t ctrlb;


static void sam3_i2s_txStop(I2s *i2s)
{
	(void)i2s;
	SSC_CR = BV(SSC_TXDIS);
	dmac_stop(I2S_DMAC_CH);

	i2s->hw->end = true;
	remaing_size = 0;
	kprintf("stop\n");

	event_do(&data_ready);
}

static void sam3_i2s_txWait(I2s *i2s)
{
	(void)i2s;
}

static void i2s_dmac_irq(void)
{
	prev = curr;
	curr = next;
	next = prev;

	dmac_setSourcesLLI(I2S_DMAC_CH, curr, (uint32_t)&sample_buff[next_idx], (uint32_t)&SSC_THR, (uint32_t)next);
	dmac_configureDmacLLI(I2S_DMAC_CH, curr, chunk_size / 2, cfg, ctrla, ctrlb);

	event_do(&data_ready);
}

static void sam3_i2s_txStart(I2s *i2s, void *buf, size_t len, size_t slice_len)
{
	ASSERT(buf);
	ASSERT(len >= slice_len);
	ASSERT(!(len % slice_len));

	i2s->hw->end = false;

	sample_buff = (uint8_t *)buf;
	next_idx = 0;
	chunk_size = slice_len;
	remaing_size = len;

	//Confing DMAC
	uint32_t cfg = BV(DMAC_CFG_DST_H2SEL) |
				((3 << DMAC_CFG_DST_PER_SHIFT) & DMAC_CFG_DST_PER_MASK) | (3 & DMAC_CFG_SRC_PER_MASK);
	uint32_t ctrla = DMAC_CTRLA_SRC_WIDTH_HALF_WORD | DMAC_CTRLA_DST_WIDTH_HALF_WORD;
	uint32_t ctrlb = BV(DMAC_CTRLB_SRC_DSCR) |
				DMAC_CTRLB_FC_MEM2PER_DMA_FC |
				DMAC_CTRLB_DST_INCR_FIXED | DMAC_CTRLB_SRC_INCR_INCREMENTING;


	/* Program the dma with the first and second chunk of samples and update counter */
	i2s->ctx.tx_callback(i2s, &sample_buff[0], chunk_size);

	curr = &lli0;
	prev = &lli0;
	next = &lli1;

	dmac_setSourcesLLI(I2S_DMAC_CH, curr, (uint32_t)&sample_buff[0], (uint32_t)&SSC_THR,(uint32_t)next);
	dmac_configureDmacLLI(I2S_DMAC_CH, curr, chunk_size / 2, cfg, ctrla, ctrlb);

	remaing_size -= chunk_size;
	next_idx += chunk_size;

	if (chunk_size <= remaing_size)
	{
		i2s->ctx.tx_callback(i2s, &sample_buff[next_idx], chunk_size);

		prev = curr;
		curr = next;
		next = prev;

		dmac_setSourcesLLI(I2S_DMAC_CH, curr, (uint32_t)&sample_buff[next_idx], (uint32_t)&SSC_THR,(uint32_t)next);
		dmac_configureDmacLLI(I2S_DMAC_CH, curr, chunk_size / 2, cfg, ctrla, ctrlb);

		remaing_size -= chunk_size;
		next_idx += chunk_size;
	}

	if (dmac_start(I2S_DMAC_CH) < 0)
		kprintf("start erros[%x]\n", dmac_error(I2S_DMAC_CH));

	SSC_CR = BV(SSC_TXEN);

	while (1)
	{
		event_wait(&data_ready);
		if (i2s->hw->end)
			break;

		remaing_size -= chunk_size;
		next_idx += chunk_size;

		if (remaing_size <= 0)
		{
			remaing_size = len;
			next_idx = 0;
		}

		if (dmac_start(I2S_DMAC_CH) < 0)
			kprintf("start erros[%x]\n", dmac_error(I2S_DMAC_CH));

		i2s->ctx.tx_callback(i2s, &sample_buff[next_idx], chunk_size);
	}
}

static void sam3_i2s_rxStop(I2s *i2s)
{
	(void)i2s;
	SSC_CR = BV(SSC_TXDIS);
}

static void sam3_i2s_rxWait(I2s *i2s)
{
	(void)i2s;
}

static void sam3_i2s_rxStart(I2s *i2s, void *buf, size_t len, size_t slice_len)
{
	(void)i2s;
	(void)buf;
	(void)len;
	(void)slice_len;
}


static bool sam3_i2s_isTxFinish(struct I2s *i2s)
{
	(void)i2s;
	return i2s->hw->end;//dmac_isDone(&dmac);
}

static bool sam3_i2s_isRxFinish(struct I2s *i2s)
{
	(void)i2s;
	return dmac_isDone(I2S_DMAC_CH);
}

static void sam3_i2s_txBuf(struct I2s *i2s, void *buf, size_t len)
{
	(void)i2s;

	uint32_t cfg = BV(DMAC_CFG_DST_H2SEL) |
				((3 << DMAC_CFG_DST_PER_SHIFT) & DMAC_CFG_DST_PER_MASK) | (3 & DMAC_CFG_SRC_PER_MASK);
	uint32_t ctrla = DMAC_CTRLA_SRC_WIDTH_HALF_WORD | DMAC_CTRLA_DST_WIDTH_HALF_WORD;
	uint32_t ctrlb = BV(DMAC_CTRLB_SRC_DSCR) | BV(DMAC_CTRLB_DST_DSCR) |
				DMAC_CTRLB_FC_MEM2PER_DMA_FC |
				DMAC_CTRLB_DST_INCR_FIXED | DMAC_CTRLB_SRC_INCR_INCREMENTING;

	dmac_setSources(I2S_DMAC_CH, (uint32_t)buf, (uint32_t)&SSC_THR);
	dmac_configureDmac(I2S_DMAC_CH, len / 2, cfg, ctrla, ctrlb);
	dmac_start(I2S_DMAC_CH);

	SSC_CR = BV(SSC_TXEN);
}

static void sam3_i2s_rxBuf(struct I2s *i2s, void *buf, size_t len)
{
	(void)i2s;

	uint32_t cfg = BV(DMAC_CFG_SRC_H2SEL) |
				((4 << DMAC_CFG_DST_PER_SHIFT) & DMAC_CFG_DST_PER_MASK) | (4 & DMAC_CFG_SRC_PER_MASK);
	uint32_t ctrla = DMAC_CTRLA_SRC_WIDTH_HALF_WORD | DMAC_CTRLA_DST_WIDTH_HALF_WORD;
	uint32_t ctrlb = BV(DMAC_CTRLB_SRC_DSCR) | BV(DMAC_CTRLB_DST_DSCR) |
						DMAC_CTRLB_FC_PER2MEM_DMA_FC |
						DMAC_CTRLB_DST_INCR_INCREMENTING | DMAC_CTRLB_SRC_INCR_FIXED;

	dmac_setSources(I2S_DMAC_CH, (uint32_t)&SSC_RHR, (uint32_t)buf);
	dmac_configureDmac(I2S_DMAC_CH, len / 2, cfg, ctrla, ctrlb);
	dmac_start(I2S_DMAC_CH);

	SSC_CR = BV(SSC_RXEN);
}

static int sam3_i2s_write(struct I2s *i2s, uint32_t sample)
{
	(void)i2s;
	SSC_CR = BV(SSC_TXEN);
	while(!(SSC_SR & BV(SSC_TXRDY)));
	SSC_THR = sample;
	return 0;
}

static uint32_t sam3_i2s_read(struct I2s *i2s)
{
	(void)i2s;
	SSC_CR = BV(SSC_RXEN);
	while(!(SSC_SR & BV(SSC_RXRDY)));
	return SSC_RHR;
}


static DECLARE_ISR(irq_ssc)
{
}

void i2s_init(I2s *i2s, int channel)
{
	(void)channel;
	i2s->ctx.write = sam3_i2s_write;
	i2s->ctx.tx_buf = sam3_i2s_txBuf;
	i2s->ctx.tx_isFinish = sam3_i2s_isTxFinish;
	i2s->ctx.tx_start = sam3_i2s_txStart;
	i2s->ctx.tx_wait = sam3_i2s_txWait;
	i2s->ctx.tx_stop = sam3_i2s_txStop;

	i2s->ctx.read = sam3_i2s_read;
	i2s->ctx.rx_buf = sam3_i2s_rxBuf;
	i2s->ctx.rx_isFinish = sam3_i2s_isRxFinish;
	i2s->ctx.rx_start = sam3_i2s_rxStart;
	i2s->ctx.rx_wait = sam3_i2s_rxWait;
	i2s->ctx.rx_stop = sam3_i2s_rxStop;

	DB(i2s->ctx._type = I2S_SAM3X;)
	i2s->hw = &i2s_hw;

	PIOA_PDR = BV(SSC_TK) | BV(SSC_TF) | BV(SSC_TD);
	PIO_PERIPH_SEL(PIOA_BASE, BV(SSC_TK) | BV(SSC_TF) | BV(SSC_TD), PIO_PERIPH_B);
	PIOB_PDR = BV(SSC_RD) | BV(SSC_RF);
	PIO_PERIPH_SEL(PIOB_BASE, BV(SSC_RD) | BV(SSC_RF), PIO_PERIPH_A);

	/* clock the ssc */
	pmc_periphEnable(SSC_ID);

	/* reset device */
	SSC_CR = BV(SSC_SWRST) | BV(SSC_TXDIS) | BV(SSC_RXDIS);

	/* Set transmission clock */
	SSC_CMR = MCK_DIV & SSC_DIV_MASK;
	/* Set the transmission mode:
	 * - the clk is generate from master clock
	 * - clock only during transfer
	 * - transmit Clock Gating Selection none
	 * - DELAY cycle insert before starting transmission
	 * - generate frame sync each 2*(PERIOD + 1) tramit clock
	 * - Receive start on falling edge RF
	 */
	SSC_TCMR = SSC_CKS_DIV | SSC_CKO_CONT | SSC_CKG_NONE | DELAY | PERIOD | SSC_START_FALL_F;
	/* Set the transmission frame mode:
	 * - data len DATALEN + 1
	 * - word per frame DATNB + 1
	 * - frame sync len FSLEN + (FSLEN_EXT * 16) + 1
	 * - DELAY cycle insert before starting transmission
	 * - MSB
	 * - Frame sync output selection negative
	 */
	SSC_TFMR = DATALEN | DATNB | FSLEN | EXTRA_FSLEN | BV(SSC_MSBF) | SSC_FSOS_POSITIVE;


	// Receiver should start on TX and take the clock from TK
    SSC_RCMR = SSC_CKS_CLK | BV(SSC_CKI) | SSC_CKO_CONT | SSC_CKG_NONE | DELAY | PERIOD | SSC_START_TX;
    SSC_RFMR = DATALEN | DATNB | FSLEN  | EXTRA_FSLEN | BV(SSC_MSBF) | SSC_FSOS_POSITIVE;


	SSC_IDR = 0xFFFFFFFF;
	SSC_CR = BV(SSC_TXDIS) | BV(SSC_RXDIS);

	dmac_enableCh(I2S_DMAC_CH, i2s_dmac_irq);
	event_initGeneric(&data_ready);
}

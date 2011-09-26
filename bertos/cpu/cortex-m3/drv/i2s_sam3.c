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



#include "hw/hw_i2s.h"

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

#include <string.h>


#define I2S_DMAC_CH    0
#define I2S_CACHED_CHUNK_SIZE 2


#define I2S_TX_DMAC_CFG  (BV(DMAC_CFG_DST_H2SEL) | \
						  BV(DMAC_CFG_SOD) | \
						((3 << DMAC_CFG_DST_PER_SHIFT) & DMAC_CFG_DST_PER_MASK) | \
						 (4 & DMAC_CFG_SRC_PER_MASK))

#define I2S_TX_DMAC_CTRLB  (DMAC_CTRLB_FC_MEM2PER_DMA_FC | \
							DMAC_CTRLB_DST_INCR_FIXED | \
							DMAC_CTRLB_SRC_INCR_INCREMENTING)


#define I2S_RX_DMAC_CFG  (BV(DMAC_CFG_SRC_H2SEL) | \
						  BV(DMAC_CFG_SOD) | \
						((3 << DMAC_CFG_DST_PER_SHIFT) & DMAC_CFG_DST_PER_MASK) | \
						 (4 & DMAC_CFG_SRC_PER_MASK))

#define I2S_RX_DMAC_CTRLB  (DMAC_CTRLB_FC_PER2MEM_DMA_FC | \
						    DMAC_CTRLB_DST_INCR_INCREMENTING | \
							DMAC_CTRLB_SRC_INCR_FIXED)


#if CONFIG_WORD_BIT_SIZE == 32
	#define I2S_TX_DMAC_CTRLA  (DMAC_CTRLA_SRC_WIDTH_WORD | \
								DMAC_CTRLA_DST_WIDTH_WORD)
	#define I2S_RX_DMAC_CTRLA  (DMAC_CTRLA_SRC_WIDTH_WORD | \
								DMAC_CTRLA_DST_WIDTH_WORD)
	#define I2S_WORD_BYTE_SIZE      4
#elif CONFIG_WORD_BIT_SIZE == 16

	#define I2S_TX_DMAC_CTRLA  (DMAC_CTRLA_SRC_WIDTH_HALF_WORD | \
								DMAC_CTRLA_DST_WIDTH_HALF_WORD)
	#define I2S_RX_DMAC_CTRLA  (DMAC_CTRLA_SRC_WIDTH_HALF_WORD | \
								DMAC_CTRLA_DST_WIDTH_HALF_WORD)
	#define I2S_WORD_BYTE_SIZE      2

#elif  CONFIG_WORD_BIT_SIZE == 8

	#define I2S_TX_DMAC_CTRLA  (DMAC_CTRLA_SRC_WIDTH_BYTE | \
								DMAC_CTRLA_DST_WIDTH_BYTE)
	#define I2S_RX_DMAC_CTRLA  (DMAC_CTRLA_SRC_WIDTH_BYTE | \
								DMAC_CTRLA_DST_WIDTH_BYTE)
	#define I2S_WORD_BYTE_SIZE      1

#else
	#error Wrong i2s word size.
#endif


#define I2S_STATUS_ERR              BV(0)
#define I2S_STATUS_SINGLE_TRASF     BV(1)
#define I2S_STATUS_TX               BV(2)
#define I2S_STATUS_END_TX           BV(3)
#define I2S_STATUS_RX               BV(4)
#define I2S_STATUS_END_RX           BV(5)


static Event data_ready;

DmacDesc lli0;
DmacDesc lli1;
DmacDesc *curr;
DmacDesc *next;
DmacDesc *prev;

static uint8_t i2s_status;
static uint8_t *sample_buff;
static size_t next_idx = 0;
static size_t chunk_size = 0;
static size_t transfer_size = 0;

static void sam3_i2s_txStop(I2s *i2s)
{
	(void)i2s;
	SSC_CR = BV(SSC_TXDIS);
	dmac_stop(I2S_DMAC_CH);

	next_idx = 0;
	transfer_size = 0;

	i2s_status |= I2S_STATUS_END_TX;
	i2s_status &= ~I2S_STATUS_TX;
	event_do(&data_ready);
}

static void sam3_i2s_txWait(I2s *i2s)
{
	(void)i2s;
	event_wait(&data_ready);
}

static void i2s_dmac_irq(uint32_t status)
{
	if (i2s_status & I2S_STATUS_SINGLE_TRASF)
	{
		i2s_status &= ~I2S_STATUS_SINGLE_TRASF;
	}
	else
	{
		if (status & (BV(I2S_DMAC_CH) << DMAC_EBCIDR_ERR0))
		{
			i2s_status |= I2S_STATUS_ERR;
			// Disable to reset channel and clear fifo
			dmac_stop(I2S_DMAC_CH);
		}
		else
		{
			prev = curr;
			curr = next;
			next = prev;

			if (i2s_status & I2S_STATUS_TX)
			{
				curr->src_addr = (uint32_t)&sample_buff[next_idx];
				curr->dst_addr = (uint32_t)&SSC_THR;
				curr->dsc_addr = (uint32_t)next;
				curr->ctrla    = I2S_TX_DMAC_CTRLA | ((chunk_size / I2S_WORD_BYTE_SIZE) & 0xffff);
				curr->ctrlb    = I2S_TX_DMAC_CTRLB & ~BV(DMAC_CTRLB_IEN);
			}
			else
			{
				curr->src_addr = (uint32_t)&SSC_RHR;
				curr->dst_addr = (uint32_t)&sample_buff[next_idx];
				curr->dsc_addr = (uint32_t)next;
				curr->ctrla    = I2S_RX_DMAC_CTRLA | ((chunk_size / I2S_WORD_BYTE_SIZE) & 0xffff);
				curr->ctrlb    = I2S_RX_DMAC_CTRLB & ~BV(DMAC_CTRLB_IEN);
			}

		}
	}
	event_do(&data_ready);
}


static void sam3_i2s_txStart(I2s *i2s, void *buf, size_t len, size_t slice_len)
{
	ASSERT(buf);
	ASSERT(len >= slice_len);
	ASSERT(!(len % slice_len));

	i2s_status &= ~(I2S_STATUS_END_TX | I2S_STATUS_SINGLE_TRASF);

	sample_buff = (uint8_t *)buf;
	next_idx = 0;
	chunk_size = slice_len;
	size_t remaing_size = len;
	transfer_size = len;


	memset(&lli0, 0, sizeof(DmacDesc));
	memset(&lli1, 0, sizeof(DmacDesc));

	prev = 0;
	curr = &lli1;
	next = &lli0;

	for (int i = 0; i < I2S_CACHED_CHUNK_SIZE; i++)
	{
		prev = curr;
		curr = next;
		next = prev;

		i2s->ctx.tx_callback(i2s, &sample_buff[next_idx], chunk_size);

		curr->src_addr = (uint32_t)&sample_buff[next_idx];
		curr->dst_addr = (uint32_t)&SSC_THR;
		curr->dsc_addr = (uint32_t)next;
		curr->ctrla    = I2S_TX_DMAC_CTRLA | ((chunk_size / I2S_WORD_BYTE_SIZE) & 0xffff);
		curr->ctrlb    = I2S_TX_DMAC_CTRLB & ~BV(DMAC_CTRLB_IEN);

		remaing_size -= chunk_size;
		next_idx += chunk_size;

		if (remaing_size <= 0)
		{
			remaing_size = transfer_size;
			next_idx = 0;
		}
	}

	dmac_setLLITransfer(I2S_DMAC_CH, prev, I2S_TX_DMAC_CFG);

	if (dmac_start(I2S_DMAC_CH) < 0)
	{
		LOG_ERR("DMAC start[%x]\n", dmac_error(I2S_DMAC_CH));
		return;
	}

	i2s_status &= ~I2S_STATUS_ERR;
	i2s_status |= I2S_STATUS_TX;

	SSC_CR = BV(SSC_TXEN);

	while (1)
	{
		event_wait(&data_ready);
		I2S_STROBE_ON();
		remaing_size -= chunk_size;
		next_idx += chunk_size;

		if (remaing_size <= 0)
		{
			remaing_size = transfer_size;
			next_idx = 0;
		}

		if (i2s_status & I2S_STATUS_ERR)
		{
			LOG_ERR("Error while streaming.\n");
			break;
		}

		if (i2s_status & I2S_STATUS_END_TX)
		{
			LOG_INFO("Stop streaming.\n");
			break;
		}

		i2s->ctx.tx_callback(i2s, &sample_buff[next_idx], chunk_size);
		I2S_STROBE_OFF();
	}
}

static void sam3_i2s_rxStop(I2s *i2s)
{
	(void)i2s;
	SSC_CR = BV(SSC_RXDIS) | BV(SSC_TXDIS);
	dmac_stop(I2S_DMAC_CH);

	i2s_status |= I2S_STATUS_END_RX;
	next_idx = 0;
	transfer_size = 0;

	i2s_status &= ~I2S_STATUS_RX;

	event_do(&data_ready);
}

static void sam3_i2s_rxWait(I2s *i2s)
{
	(void)i2s;
	event_wait(&data_ready);
}

static void sam3_i2s_rxStart(I2s *i2s, void *buf, size_t len, size_t slice_len)
{
	ASSERT(buf);
	ASSERT(len >= slice_len);
	ASSERT(!(len % slice_len));

	i2s_status &= ~(I2S_STATUS_END_RX | I2S_STATUS_SINGLE_TRASF);

	sample_buff = (uint8_t *)buf;
	next_idx = 0;
	chunk_size = slice_len;
	size_t remaing_size = len;
	transfer_size = len;

	memset(&lli0, 0, sizeof(DmacDesc));
	memset(&lli1, 0, sizeof(DmacDesc));

	prev = 0;
	curr = &lli1;
	next = &lli0;

	for (int i = 0; i < I2S_CACHED_CHUNK_SIZE; i++)
	{
		prev = curr;
		curr = next;
		next = prev;

		curr->src_addr = (uint32_t)&SSC_RHR;
		curr->dst_addr = (uint32_t)&sample_buff[next_idx];
		curr->dsc_addr = (uint32_t)next;
		curr->ctrla    = I2S_RX_DMAC_CTRLA | ((chunk_size / I2S_WORD_BYTE_SIZE) & 0xffff);
		curr->ctrlb    = I2S_RX_DMAC_CTRLB & ~BV(DMAC_CTRLB_IEN);

		remaing_size -= chunk_size;
		next_idx += chunk_size;

		if (remaing_size <= 0)
		{
			remaing_size = transfer_size;
			next_idx = 0;
		}
	}

	dmac_setLLITransfer(I2S_DMAC_CH, prev, I2S_RX_DMAC_CFG);

	if (dmac_start(I2S_DMAC_CH) < 0)
	{
		LOG_ERR("DMAC start[%x]\n", dmac_error(I2S_DMAC_CH));
		return;
	}

	i2s_status &= ~I2S_STATUS_ERR;
	i2s_status |= I2S_STATUS_RX;

	SSC_CR = BV(SSC_TXEN) | BV(SSC_RXEN);

	while (1)
	{
		event_wait(&data_ready);
		I2S_STROBE_ON();
		i2s->ctx.rx_callback(i2s, &sample_buff[next_idx], chunk_size);

		remaing_size -= chunk_size;
		next_idx += chunk_size;

		if (remaing_size <= 0)
		{
			remaing_size = transfer_size;
			next_idx = 0;
		}

		if (i2s_status & I2S_STATUS_ERR)
		{
			LOG_ERR("Error while streaming.\n");
			break;
		}

		if (i2s_status & I2S_STATUS_END_RX)
		{
			LOG_INFO("Stop streaming.\n");
			break;
		}
		I2S_STROBE_OFF();
	}
}


static bool sam3_i2s_isTxFinish(struct I2s *i2s)
{
	(void)i2s;
	return (i2s_status & I2S_STATUS_END_TX);
}

static bool sam3_i2s_isRxFinish(struct I2s *i2s)
{
	(void)i2s;
	return (i2s_status & I2S_STATUS_END_RX);
}

static void sam3_i2s_txBuf(struct I2s *i2s, void *buf, size_t len)
{
	(void)i2s;
	i2s_status |= I2S_STATUS_SINGLE_TRASF;

	dmac_setSources(I2S_DMAC_CH, (uint32_t)buf, (uint32_t)&SSC_THR);
	dmac_configureDmac(I2S_DMAC_CH, len / I2S_WORD_BYTE_SIZE, I2S_TX_DMAC_CFG, I2S_TX_DMAC_CTRLA, I2S_TX_DMAC_CTRLB);
	dmac_start(I2S_DMAC_CH);

	SSC_CR = BV(SSC_TXEN);
}

static void sam3_i2s_rxBuf(struct I2s *i2s, void *buf, size_t len)
{
	(void)i2s;

	i2s_status |= I2S_STATUS_SINGLE_TRASF;

	dmac_setSources(I2S_DMAC_CH, (uint32_t)&SSC_RHR, (uint32_t)buf);
	dmac_configureDmac(I2S_DMAC_CH, len / I2S_WORD_BYTE_SIZE, I2S_RX_DMAC_CFG, I2S_RX_DMAC_CTRLA, I2S_RX_DMAC_CTRLB);
	dmac_start(I2S_DMAC_CH);

	SSC_CR = BV(SSC_TXEN) | BV(SSC_RXEN);
}

static int sam3_i2s_write(struct I2s *i2s, uint32_t sample)
{
	(void)i2s;

	SSC_CR = BV(SSC_TXEN);
	while(!(SSC_SR & BV(SSC_TXRDY)))
		cpu_relax();

	SSC_THR = sample;
	return 0;
}

static uint32_t sam3_i2s_read(struct I2s *i2s)
{
	(void)i2s;

	SSC_CR = BV(SSC_RXEN);
	while(!(SSC_SR & BV(SSC_RXRDY)))
		cpu_relax();

	return SSC_RHR;
}


/* We divite for 2 because the min clock for i2s i MCLK/2 */
#define MCK_DIV     (CPU_FREQ / (CONFIG_SAMPLE_FREQ * CONFIG_WORD_BIT_SIZE * CONFIG_CHANNEL_NUM * 2))
#define DATALEN     ((CONFIG_WORD_BIT_SIZE - 1) & SSC_DATLEN_MASK)
#define DELAY       ((CONFIG_DELAY << SSC_STTDLY_SHIFT) & SSC_STTDLY_MASK)
#define PERIOD      ((CONFIG_PERIOD << (SSC_PERIOD_SHIFT)) & SSC_PERIOD_MASK)
#define DATNB       ((CONFIG_WORD_PER_FRAME << SSC_DATNB_SHIFT) & SSC_DATNB_MASK)
#define FSLEN       ((CONFIG_FRAME_SYNC_SIZE << SSC_FSLEN_SHIFT) & SSC_FSLEN_MASK)
#define EXTRA_FSLEN (CONFIG_EXTRA_FRAME_SYNC_SIZE << SSC_FSLEN_EXT)

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

	I2S_STROBE_INIT();

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
	SSC_TFMR = DATALEN | DATNB | FSLEN | EXTRA_FSLEN | BV(SSC_MSBF) | SSC_FSOS_NEGATIVE;


	// Receiver should start on TX and take the clock from TK
    SSC_RCMR = SSC_CKS_CLK | BV(SSC_CKI) | SSC_CKO_CONT | SSC_CKG_NONE | DELAY | PERIOD | SSC_START_TX;
    SSC_RFMR = DATALEN | DATNB | FSLEN  | EXTRA_FSLEN | BV(SSC_MSBF) | SSC_FSOS_NEGATIVE;


	SSC_IDR = 0xFFFFFFFF;

	dmac_enableCh(I2S_DMAC_CH, i2s_dmac_irq);
	event_initGeneric(&data_ready);
}

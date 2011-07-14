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


/*
 * TODO: Revise the public api of this module to be more generic. Evalutate to
 * implement the more generic layer to be common to all I2S BeRTOS drivers.
 */
#include "i2s_sam3.h"
#include "cfg/cfg_i2s.h"

// Define log settings for cfg/log.h.
#define LOG_LEVEL         I2S_LOG_LEVEL
#define LOG_FORMAT        I2S_LOG_FORMAT
#include <cfg/log.h>

#include <drv/timer.h>
#include <drv/irq_cm3.h>

#include <cpu/irq.h>

#include <io/cm3.h>

#define DATALEN (15 & SSC_DATLEN_MASK)
// FIXME: this is not correct for 16 <= DATALEN < 24
#define PDC_DIV ((DATALEN / 8) + 1)
/*
 * PDC_DIV must be 1, 2 or 4, which are the bytes that are transferred
 * each time the PDC reads from memory.
 */
STATIC_ASSERT(PDC_DIV % 2 == 0);
#define PDC_COUNT (CONFIG_PLAY_BUF_LEN / PDC_DIV)

static uint8_t play_buf1[CONFIG_PLAY_BUF_LEN];
static uint8_t play_buf2[CONFIG_PLAY_BUF_LEN];

// the buffer in PDC next is play_buf2
volatile bool is_second_buf_next;

uint8_t *i2s_getBuffer(unsigned buf_num)
{
	LOG_INFO("getBuffer start\n");

	if (i2s_isPlaying())
	{
		ASSERT(0);
		return 0;
	}

	if (buf_num == I2S_SECOND_BUF)
		return play_buf2;
	else if (buf_num == I2S_FIRST_BUF)
		return play_buf1;
	else
		return 0;
}

uint8_t *i2s_getFreeBuffer(void)
{
	// wait PDC transmission end
	if (!(SSC_SR & BV(SSC_ENDTX)))
		return 0;

	uint8_t *ret_buf = 0;
	// the last time we got called, the second buffer was in PDC next
	if (is_second_buf_next)
	{
		is_second_buf_next = false;
		ret_buf = play_buf1;
	}
	// the last time the first buffer was in PDC next
	else
	{
		is_second_buf_next = true;
		ret_buf = play_buf2;
	}

	if (ret_buf)
	{
		SSC_TNPR = (reg32_t) ret_buf;
		SSC_TNCR = PDC_COUNT;
	}
	return ret_buf;
}

void i2s_stop(void)
{
	SSC_CR = BV(SSC_TXDIS);
}


bool i2s_start(void)
{
	/* Some time must pass between disabling and enabling again the transmission
	 * on SSC. A good empirical value seems >15 us. We try to avoid putting an
	 * explicit delay, instead we disable the transmitter when a sound finishes
	 * and hope that the delay has passed before we enter here again.
	 */
	SSC_CR = BV(SSC_TXDIS);
	timer_delay(10);

	SSC_PTCR = BV(PDC_PTCR_TXTDIS);
	SSC_TPR = (reg32_t)play_buf1;
	SSC_TCR = PDC_COUNT;
	SSC_TNPR = (reg32_t)play_buf2;
	SSC_TNCR = PDC_COUNT;
	is_second_buf_next = true;

	SSC_PTCR = BV(PDC_PTSR_TXTEN);

	/* enable output */
	SSC_CR = BV(SSC_TXEN);

	return true;
}

#define BITS_PER_CHANNEL 16
#define N_OF_CHANNEL 2
// TODO: check the computed value?
/* The last parameter (2) is due to the hadware on at91sam7s. */
#define MCK_DIV (CPU_FREQ / CONFIG_SAMPLE_FREQ / BITS_PER_CHANNEL / N_OF_CHANNEL / 2)

#define CONFIG_DELAY 1
#define CONFIG_PERIOD 15
#define CONFIG_DATNB  1
#define CONFIG_FSLEN 15

#define DELAY ((CONFIG_DELAY << SSC_STTDLY_SHIFT) & SSC_STTDLY_MASK)
#define PERIOD ((CONFIG_PERIOD << (SSC_PERIOD_SHIFT)) & SSC_PERIOD_MASK)
#define DATNB ((CONFIG_DATNB << SSC_DATNB_SHIFT) & SSC_DATNB_MASK)
#define FSLEN ((CONFIG_FSLEN << SSC_FSLEN_SHIFT) & SSC_FSLEN_MASK)

#define SSC_DMA_IRQ_PRIORITY 5


static DECLARE_ISR(irq_ssc)
{
}

void i2s_init(void)
{
	SSC_PIO_PDR = BV(SSC_TK) | BV(SSC_TF) | BV(SSC_TD);
	PIO_PERIPH_SEL(SSC_PORT, BV(SSC_TK) | BV(SSC_TF) | BV(SSC_TD), SSC_TRAN_PERIPH);

	/* reset device */
	SSC_CR = BV(SSC_SWRST);

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
	SSC_TCMR = SSC_CKS_DIV | SSC_CKO_TRAN | SSC_CKG_NONE | DELAY | PERIOD | SSC_START_FALL_F;
	/* Set the transmission frame mode:
	 * - data len DATALEN + 1
	 * - word per frame DATNB + 1
	 * - frame sync len FSLEN + (FSLEN_EXT * 16) + 1
	 * - DELAY cycle insert before starting transmission
	 * - MSB
	 * - Frame sync output selection negative
	 */
	SSC_TFMR = DATALEN | DATNB | FSLEN | BV(SSC_MSBF) | SSC_FSOS_NEGATIVE;

	SSC_IDR = 0xFFFFFFFF;
	sysirq_setHandler(INT_SSC, irq_ssc);

	/* Clock DAC peripheral */
	pmc_periphEnable(SSC_ID);

	/* Enable SSC */
	SSC_CR = BV(SSC_TXEN);
}

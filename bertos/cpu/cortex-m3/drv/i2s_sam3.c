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
#define BITS_PER_CHANNEL 16
#define N_OF_CHANNEL 2
// TODO: check the computed value?
/* The last parameter (2) is due to the hadware on at91sam7s. */
#define MCK_DIV (CPU_FREQ / (44100 * BITS_PER_CHANNEL * N_OF_CHANNEL* 2))

#define CONFIG_DELAY 0
#define CONFIG_PERIOD 15
#define CONFIG_DATNB  1
#define CONFIG_FSLEN 15


#define DELAY ((CONFIG_DELAY << SSC_STTDLY_SHIFT) & SSC_STTDLY_MASK)
#define PERIOD ((CONFIG_PERIOD << (SSC_PERIOD_SHIFT)) & SSC_PERIOD_MASK)
#define DATNB ((CONFIG_DATNB << SSC_DATNB_SHIFT) & SSC_DATNB_MASK)
#define FSLEN ((CONFIG_FSLEN << SSC_FSLEN_SHIFT) & SSC_FSLEN_MASK)



void i2s_stop(void)
{
	SSC_CR = BV(SSC_TXDIS);
}


bool i2s_start(void)
{

	/* enable output */
	SSC_CR = BV(SSC_TXEN);

	return true;
}


static DECLARE_ISR(irq_ssc)
{
}

void i2s_init(void)
{
	PIOA_PDR = BV(SSC_TK) | BV(SSC_TF) | BV(SSC_TD);
	PIO_PERIPH_SEL(SSC_PORT, BV(SSC_TK) | BV(SSC_TF) | BV(SSC_TD), PIO_PERIPH_B);
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
	SSC_TFMR = DATALEN | DATNB | FSLEN | BV(SSC_MSBF) | SSC_FSOS_POSITIVE;
}

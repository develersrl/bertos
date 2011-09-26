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

#include "dmac_sam3.h"
#include <drv/irq_cm3.h>

#include <cpu/irq.h>
#include <cpu/power.h>

#include <io/cm3.h>

#include <mware/event.h>

#include <string.h>

struct DmacCh
{
	reg32_t *src;
	reg32_t *dst;
	reg32_t *desc;
	reg32_t *cfg;
	reg32_t *ctrla;
	reg32_t *ctrlb;
};

#define DMAC_CHANNEL_CNT   6
struct DmacCh dmac_ch[] =
{
	{
		.src = &DMAC_SADDR0,
		.dst = &DMAC_DADDR0,
		.desc = &DMAC_DSCR0,
		.cfg = &DMAC_CFG0,
		.ctrla = &DMAC_CTRLA0,
		.ctrlb = &DMAC_CTRLB0,
	},
	{
		.src = &DMAC_SADDR1,
		.dst = &DMAC_DADDR1,
		.desc = &DMAC_DSCR1,
		.cfg = &DMAC_CFG1,
		.ctrla = &DMAC_CTRLA1,
		.ctrlb = &DMAC_CTRLB1,
	},
	{
		.src = &DMAC_SADDR2,
		.dst = &DMAC_DADDR2,
		.desc = &DMAC_DSCR2,
		.cfg = &DMAC_CFG2,
		.ctrla = &DMAC_CTRLA2,
		.ctrlb = &DMAC_CTRLB2,
	},
	{
		.src = &DMAC_SADDR3,
		.dst = &DMAC_DADDR3,
		.desc = &DMAC_DSCR3,
		.cfg = &DMAC_CFG3,
		.ctrla = &DMAC_CTRLA3,
		.ctrlb = &DMAC_CTRLB3,
	},
	{
		.src = &DMAC_SADDR4,
		.dst = &DMAC_DADDR4,
		.desc = &DMAC_DSCR4,
		.cfg = &DMAC_CFG4,
		.ctrla = &DMAC_CTRLA4,
		.ctrlb = &DMAC_CTRLB4,
	},
	{
		.src = &DMAC_SADDR5,
		.dst = &DMAC_DADDR5,
		.desc = &DMAC_DSCR5,
		.cfg = &DMAC_CFG5,
		.ctrla = &DMAC_CTRLA5,
		.ctrlb = &DMAC_CTRLB5,
	},
};

/* We use event to signal the end of conversion */
static Dmac dmac[DMAC_CHANNEL_CNT];
static uint8_t dmac_ch_enabled;

void dmac_setLLITransfer(int ch, DmacDesc *lli, uint32_t cfg)
{
	DMAC_CHDR = BV(ch);
	reg32_t reg = DMAC_EBCISR;
	(void)reg;

	*dmac_ch[ch].cfg = cfg | DMAC_CFG_FIFOCFG_ALAP_CFG | (0x1 << DMAC_CFG_AHB_PROT_SHIFT);
	*dmac_ch[ch].desc = (uint32_t)lli;
}

void dmac_setSources(int ch, uint32_t src, uint32_t dst)
{
	DMAC_CHDR = BV(ch);

	*dmac_ch[ch].src = src;
	*dmac_ch[ch].dst = dst;
	*dmac_ch[ch].desc = 0;
}

void dmac_configureDmac(int ch, size_t transfer_size, uint32_t cfg, uint32_t ctrla, uint32_t ctrlb)
{
	DMAC_CHDR = BV(ch);

	*dmac_ch[ch].cfg = cfg | DMAC_CFG_FIFOCFG_ALAP_CFG | (0x1 << DMAC_CFG_AHB_PROT_SHIFT);
	*dmac_ch[ch].ctrla = ctrla | (transfer_size & DMAC_CTRLA_BTSIZE_MASK);
	*dmac_ch[ch].ctrlb = ctrlb & ~BV(DMAC_CTRLB_IEN);
}

int dmac_start(int ch)
{
	if (DMAC_CHSR & BV(ch))
	{
		dmac[ch].errors |= DMAC_ERR_CH_ALREDY_ON;
		return -1;
	}
	DMAC_CHER = BV(ch);
	dmac_ch_enabled |= BV(ch);
	return 0;
}

void dmac_stop(int ch)
{
	DMAC_CHDR = BV(ch);
	dmac_ch_enabled &= ~BV(ch);
}

int dmac_error(int ch)
{
	uint32_t err = ((DMAC_EBCISR & 0x3F0000) | dmac[ch].errors);
	dmac[ch].errors = 0;
	return err;
}

static DECLARE_ISR(dmac_irq)
{
	uint32_t status = DMAC_EBCISR;
	uint32_t irq_ch = (status & (((dmac_ch_enabled |
								(dmac_ch_enabled << DMAC_EBCIDR_ERR0) >> DMAC_EBCIDR_ERR0) |
								(dmac_ch_enabled << DMAC_EBCIDR_CBTC0) >> DMAC_EBCIDR_CBTC0) & 0xFF));
	if (irq_ch)
		for(int i = 0; i < 6; i++)
		{
			if (BV(i) & irq_ch)
				if(dmac[i].handler)
					dmac[i].handler(status);
		}
}

bool dmac_enableCh(int ch, dmac_handler_t handler)
{
	ASSERT(ch <= DMAC_CHANNEL_CNT);

	dmac_ch_enabled |= BV(ch);
	if (handler)
	{
		dmac[ch].handler = handler;
		DMAC_EBCIER |= (BV(ch) << DMAC_EBCIER_BTC0) | (BV(ch) << DMAC_EBCIDR_CBTC0) | (BV(ch) << DMAC_EBCIDR_ERR0);
		kprintf("Init dmac ch[%08lx]\n", DMAC_EBCIMR);
	}

	return true;
}

void dmac_init(void)
{
	dmac_ch_enabled = 0;
	memset(&dmac, 0, sizeof(dmac));

	//init DMAC
	DMAC_EBCIDR = 0x3FFFFF;
	DMAC_CHDR = 0x1F;

	pmc_periphEnable(DMAC_ID);
	DMAC_EN = BV(DMAC_EN_ENABLE);

	sysirq_setHandler(INT_DMAC, dmac_irq);
}

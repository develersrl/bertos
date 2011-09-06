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

#include "hsmci_sam3.h"
#include "hw/hw_sd.h"

#include <drv/timer.h>
#include <drv/irq_cm3.h>

#include <cpu/irq.h>
#include <cpu/power.h>

#include <io/cm3.h>

struct DmacCh
{
	reg32_t *src;
	reg32_t *dst;
	reg32_t *desc;
	reg32_t *cfg;
	reg32_t *ctrla;
	reg32_t *ctrlb;
};

#define DMAC_CHANNEL_CNT   5
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

void dmac_setSources(Dmac *dmac, uint8_t ch, uint32_t src, uint32_t dst, size_t transfer_size)
{
	ASSERT(ch <= DMAC_CHANNEL_CNT);

	DMAC_CHDR = BV(ch);

	*dmac_ch[ch].src = src;
	*dmac_ch[ch].dst = dst;
	*dmac_ch[ch].desc = 0;
	dmac->transfer_size = transfer_size;
}

void dmac_configureDmac(Dmac *dmac, uint8_t ch, uint32_t cfg, uint32_t ctrla, uint32_t ctrlb)
{
	ASSERT(ch <= DMAC_CHANNEL_CNT);

	DMAC_CHDR = BV(ch);

	*dmac_ch[ch].cfg = cfg | DMAC_CFG_FIFOCFG_ALAP_CFG | (0x1 << DMAC_CFG_AHB_PROT_SHIFT);
	*dmac_ch[ch].ctrla = ctrla | (dmac->transfer_size & DMAC_CTRLA_BTSIZE_MASK);
	*dmac_ch[ch].ctrlb = ctrlb | BV(DMAC_CTRLB_IEN);
}

int dmac_start(Dmac *dmac, uint8_t ch)
{
	ASSERT(ch <= DMAC_CHANNEL_CNT);

	if (DMAC_CHSR & BV(ch))
	{
		dmac->errors |= DMAC_ERR_CH_ALREDY_ON;
		return -1;
	}

	DMAC_CHER = BV(ch);
	return 0;
}

bool dmac_isDone(Dmac *dmac, uint8_t ch)
{
	(void)dmac;
	return (DMAC_EBCISR |= BV(ch));
}

bool dmac_waitDone(Dmac *dmac, uint8_t ch)
{
	(void)dmac;
	while(!(DMAC_EBCISR |= BV(ch)))
		cpu_relax();

	return true;
}

static DECLARE_ISR(dmac_irq)
{
}

void dmac_init(Dmac *dmac)
{
	(void)dmac;

	//init DMAC
	DMAC_EBCIDR = 0x3FFFFF;
	DMAC_CHDR = 0x1F;

	pmc_periphEnable(DMAC_ID);
	DMAC_EN = BV(DMAC_EN_ENABLE);
	sysirq_setHandler(INT_DMAC, dmac_irq);
}

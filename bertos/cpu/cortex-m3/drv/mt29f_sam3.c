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
 *
 * -->
 *
 * \brief Micron MT29F serial NAND driver for SAM3's static memory controller.
 *
 * \author Stefano Fedrigo <aleph@develer.com>
 */

#include "mt29f_sam3.h"
#include "cfg/cfg_mt29f.h"

// Define log settings for cfg/log.h
#define LOG_LEVEL    CONFIG_MT29F_LOG_LEVEL
#define LOG_FORMAT   CONFIG_MT29F_LOG_FORMAT

#include <cfg/log.h>
#include <cfg/macros.h>

#include <io/sam3.h>
#include <io/kblock.h>

#include <drv/timer.h>
#include <drv/mt29f.h>

#include <cpu/power.h> /* cpu_relax() */
#include <cpu/types.h>

#include <string.h> /* memcpy() */


struct Mt29fHardware
{
	int boh;
};


static size_t mt29f_readDirect(struct KBlock *blk, block_idx_t idx, void *buf, size_t offset, size_t size)
{
}


static size_t mt29f_writeDirect(struct KBlock *blk, block_idx_t idx, const void *_buf, size_t offset, size_t size)
{
}


static int mt29f_error(struct KBlock *blk)
{
	Mt29f *fls = FLASH_CAST(blk);
}


static void mt29f_clearerror(struct KBlock *blk)
{
	Mt29f *fls = FLASH_CAST(blk);
}


static const KBlockVTable mt29f_buffered_vt =
{
	.readDirect = mt29f_readDirect,
	.writeDirect = mt29f_writeDirect,

	.readBuf = kblock_swReadBuf,
	.writeBuf = kblock_swWriteBuf,
	.load = kblock_swLoad,
	.store = kblock_swStore,

	.close = kblock_swClose,

	.error = mt29f_error,
	.clearerr = mt29f_clearerror,
};


static const KBlockVTable mt29f_unbuffered_vt =
{
	.readDirect = mt29f_readDirect,
	.writeDirect = mt29f_writeDirect,

	.close = kblock_swClose,

	.error = mt29f_error,
	.clearerr = mt29f_clearerror,
};


static struct Mt29fHardware mt29f_hw;
static uint8_t kblock_buf[MT29F_PAGE_SIZE];


static void common_init(Mt29f *fls)
{
	memset(fls, 0, sizeof(*fls));
	DB(fls->blk.priv.type = KBT_MT29F);

	fls->hw = &mt29f_hw;

	fls->blk.blk_size = MT29F_PAGE_SIZE;
	fls->blk.blk_cnt =  MT29F_SIZE / MT29F_PAGE_SIZE;

	// TODO: put following stuff in hw_ file dependent (and configurable cs?)
	pmc_periphEnable(PIOA_ID);
	pmc_periphEnable(PIOC_ID);
	pmc_periphEnable(PIOD_ID);

	PIO_PERIPH_SEL(PIOA_BASE, MT29F_PINS_PORTA, MT29F_PERIPH_PORTA);
	PIOA_PDR = MT29F_PINS_PORTA;
	PIOA_PUER = MT29F_PINS_PORTA;

	PIO_PERIPH_SEL(PIOC_BASE, MT29F_PINS_PORTC, MT29F_PERIPH_PORTC);
	PIOC_PDR = MT29F_PINS_PORTC;
	PIOC_PUER = MT29F_PINS_PORTC;

	PIO_PERIPH_SEL(PIOD_BASE, MT29F_PINS_PORTD, MT29F_PERIPH_PORTD);
	PIOD_PDR = MT29F_PINS_PORTD;
	PIOD_PUER = MT29F_PINS_PORTD;

    pmc_periphEnable(SMC_SDRAMC_ID);

    SMC_SETUP0 = SMC_SETUP_NWE_SETUP(0)
		| SMC_SETUP_NCS_WR_SETUP(0)
		| SMC_SETUP_NRD_SETUP(0)
		| SMC_SETUP_NCS_RD_SETUP(0);

    SMC_PULSE0 = SMC_PULSE_NWE_PULSE(2)
		| SMC_PULSE_NCS_WR_PULSE(3)
		| SMC_PULSE_NRD_PULSE(2)
		| SMC_PULSE_NCS_RD_PULSE(3);

    SMC_CYCLE0 = SMC_CYCLE_NWE_CYCLE(3)
		| SMC_CYCLE_NRD_CYCLE(3);

    SMC_TIMINGS0 = SMC_TIMINGS_TCLR(1)
		| SMC_TIMINGS_TADL(6)
		| SMC_TIMINGS_TAR(4)
		| SMC_TIMINGS_TRR(2)
		| SMC_TIMINGS_TWB(9)
		| SMC_TIMINGS_RBNSEL(7)
		| SMC_TIMINGS_NFSEL;

    SMC_MODE0 = SMC_MODE_READ_MODE
		| SMC_MODE_WRITE_MODE;


}


void mt29f_hw_init(Mt29f *fls, int flags)
{
	common_init(fls);
	fls->blk.priv.vt = &mt29f_buffered_vt;
	fls->blk.priv.flags |= KB_BUFFERED | KB_PARTIAL_WRITE;
	fls->blk.priv.buf = kblock_buf;

	// Load the first block in the cache
	void *start = 0x0;
	memcpy(fls->blk.priv.buf, start, fls->blk.blk_size);
}


void mt29f_hw_initUnbuffered(Mt29f *fls, int flags)
{
	common_init(fls);
	fls->blk.priv.vt = &mt29f_unbuffered_vt;
}


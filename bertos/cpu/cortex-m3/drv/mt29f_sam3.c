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

// Timeout for read/write transfers in ms
#define MT29F_XFER_TMOUT  1000

// NAND flash status codes
#define MT29F_STATUS_READY             BV(6)
#define MT29F_STATUS_ERROR             BV(0)

// NAND flash commands
#define MT29F_CMD_READ_1               0x00
#define MT29F_CMD_READ_2               0x30
#define MT29F_CMD_COPYBACK_READ_1      0x00
#define MT29F_CMD_COPYBACK_READ_2      0x35
#define MT29F_CMD_COPYBACK_PROGRAM_1   0x85
#define MT29F_CMD_COPYBACK_PROGRAM_2   0x10
#define MT29F_CMD_RANDOM_OUT           0x05
#define MT29F_CMD_RANDOM_OUT_2         0xE0
#define MT29F_CMD_RANDOM_IN            0x85
#define MT29F_CMD_READID               0x90
#define MT29F_CMD_WRITE_1              0x80
#define MT29F_CMD_WRITE_2              0x10
#define MT29F_CMD_ERASE_1              0x60
#define MT29F_CMD_ERASE_2              0xD0
#define MT29F_CMD_STATUS               0x70
#define MT29F_CMD_RESET                0xFF


struct Mt29fHardware
{
	uint8_t status;
};


/*
 * Translate flash page index plus a byte offset
 * in the five address cycles format needed by NAND.
 *
 * Cycles in x8 mode as the MT29F2G08AAD
 * CA = column addr, PA = page addr, BA = block addr
 *
 * Cycle    I/O7  I/O6  I/O5  I/O4  I/O3  I/O2  I/O1  I/O0
 * -------------------------------------------------------
 * First    CA7   CA6   CA5   CA4   CA3   CA2   CA1   CA0
 * Second   LOW   LOW   LOW   LOW   CA11  CA10  CA9   CA8
 * Third    BA7   BA6   PA5   PA4   PA3   PA2   PA1   PA0
 * Fourth   BA15  BA14  BA13  BA12  BA11  BA10  BA9   BA8
 * Fifth    LOW   LOW   LOW   LOW   LOW   LOW   LOW   BA16
 */
static void mt29f_getAddrCycles(block_idx_t page, size_t offset, uint32_t *cycle0, uint32_t *cycle1234)
{
	uint32_t addr = (page * MT29F_PAGE_SIZE) + offset;

	/*
	 * offset nibbles  77776666 55554444 33332222 11110000
	 * cycle1234       -------7 66665555 ----4444 33332222
	 * cycle0          11110000
	 */
	*cycle0 = addr & 0xff;
	*cycle1234 = ((addr >> 8) & 0x00000fff) | ((addr >> 4) & 0x01ff0000);
}


INLINE bool mt29f_isBusy(void)
{
	return HWREG(NFC_CMD_BASE_ADDR + NFC_CMD_NFCCMD) & 0x8000000;
}

INLINE bool mt29f_isCmdDone(void)
{
    return SMC_SR & SMC_SR_CMDDONE;
}

INLINE bool mt29f_isReadyBusy(void)
{
    return SMC_SR & SMC_SR_RB_EDGE0;
}

/*
 * Wait for transfer to complete until timeout.
 * If transfer completes return true, false in case of timeout.
 */
INLINE bool mt29f_waitTransferComplete(void)
{
	time_t start = timer_clock();

	while (!(SMC_SR & SMC_SR_XFRDONE))
	{
		cpu_relax();
		if (timer_clock() - start > MT29F_XFER_TMOUT)
			return false;
	}

	kprintf("xfer ok!!\n");
	return true;
}


/*
 * Send command to NAND and wait for completion.
 */
static void mt29f_sendCommand(uint32_t cmd, uint32_t cycle0, uint32_t cycle1234)
{
	reg32_t *cmd_addr;

	while (mt29f_isBusy());

	SMC_ADDR = cycle0;

	cmd_addr = (reg32_t *)(NFC_CMD_BASE_ADDR + cmd);
	*cmd_addr = cycle1234;

	while (!mt29f_isCmdDone());
}


static bool mt29f_isOperationComplete(void)
{
	uint8_t status;

	mt29f_sendCommand(
		NFC_CMD_NFCCMD | MT29F_CSID | NFC_CMD_ACYCLE_NONE |
		MT29F_CMD_STATUS << 2, 0, 0);

	status = (uint8_t)HWREG(MT29F_DATA_ADDR);
	return (status & MT29F_STATUS_READY) && !(status & MT29F_STATUS_ERROR);
}


static void mt29f_reset(void)
{
	mt29f_sendCommand(
		NFC_CMD_NFCCMD | MT29F_CSID | NFC_CMD_ACYCLE_NONE |
		MT29F_CMD_RESET << 2,
		0, 0);

	while (!mt29f_isReadyBusy());
}


/**
 * Erase the whole block containing given page.
 */
int mt29f_blockErase(Mt29f *fls, block_idx_t page)
{
	uint32_t cycle0;
	uint32_t cycle1234;

	mt29f_getAddrCycles(page, 0, &cycle0, &cycle1234);

	mt29f_sendCommand(
		NFC_CMD_NFCCMD | MT29F_CSID | NFC_CMD_ACYCLE_THREE | NFC_CMD_VCMD2 |
		(MT29F_CMD_ERASE_2 << 10) | (MT29F_CMD_ERASE_1 << 2),
		0, cycle1234);

	while (!mt29f_isReadyBusy());

	if (!mt29f_isOperationComplete())
	{
		LOG_ERR("mt29f: error erasing block\n");
		fls->hw->status |= MT29F_ERR_ERASE;
		return -1;
	}

	return 0;
}


static size_t mt29f_readDirect(struct KBlock *blk, block_idx_t idx, void *buf, size_t offset, size_t size)
{
	Mt29f *fls = FLASH_CAST(blk);
	uint32_t cycle0;
	uint32_t cycle1234;

	ASSERT(offset == 0);
	ASSERT(size == blk->blk_size);

	kprintf("read direct\n");

	mt29f_getAddrCycles(idx, 0, &cycle0, &cycle1234);

	mt29f_sendCommand(
		NFC_CMD_NFCCMD | NFC_CMD_NFCEN | MT29F_CSID | NFC_CMD_ACYCLE_FIVE | NFC_CMD_VCMD2 |
		(MT29F_CMD_READ_2 << 10) | (MT29F_CMD_READ_1 << 2),
		cycle0, cycle1234);

	while (!mt29f_isReadyBusy());
	if (!mt29f_waitTransferComplete())
	{
		LOG_ERR("mt29f: read timeout\n");
		fls->hw->status |= MT29F_ERR_RD_TMOUT;
		return 0;
	}

	if (!kblock_buffered(blk) && (buf != (void *)NFC_SRAM_BASE_ADDR))
		memcpy(buf, (void *)NFC_SRAM_BASE_ADDR, size);

	return size;
}


static size_t mt29f_writeDirect(struct KBlock *blk, block_idx_t idx, const void *_buf, size_t offset, size_t size)
{
	Mt29f *fls = FLASH_CAST(blk);
	uint32_t cycle0;
	uint32_t cycle1234;

	ASSERT(offset == 0);
	ASSERT(size == blk->blk_size);

	kprintf("write direct\n");

	if (!kblock_buffered(blk) && (_buf != (void *)NFC_SRAM_BASE_ADDR))
		memcpy((void *)NFC_SRAM_BASE_ADDR, _buf, size);

	mt29f_getAddrCycles(idx, 0, &cycle0, &cycle1234);

	mt29f_sendCommand(
			NFC_CMD_NFCCMD | NFC_CMD_NFCWR | NFC_CMD_NFCEN | MT29F_CSID | NFC_CMD_ACYCLE_FIVE |
			MT29F_CMD_WRITE_1 << 2,
			cycle0, cycle1234);

	if (!mt29f_waitTransferComplete())
	{
		LOG_ERR("mt29f: write timeout\n");
		fls->hw->status |= MT29F_ERR_WR_TMOUT;
		return 0;
	}

	mt29f_sendCommand(
			NFC_CMD_NFCCMD | MT29F_CSID | NFC_CMD_ACYCLE_NONE |
			MT29F_CMD_WRITE_2 << 2,
			0, 0);

	while (!mt29f_isReadyBusy());

	if (!mt29f_isOperationComplete())
	{
		LOG_ERR("mt29f: error writing page\n");
		fls->hw->status |= MT29F_ERR_WRITE;
		return 0;
	}

	return size;
}


static int mt29f_error(struct KBlock *blk)
{
	Mt29f *fls = FLASH_CAST(blk);
	return fls->hw->status;
}


static void mt29f_clearerror(struct KBlock *blk)
{
	Mt29f *fls = FLASH_CAST(blk);
	fls->hw->status = 0;
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


static void common_init(Mt29f *fls)
{
	memset(fls, 0, sizeof(*fls));
	DB(fls->blk.priv.type = KBT_MT29F);

	fls->hw = &mt29f_hw;

	fls->blk.blk_size = MT29F_PAGE_SIZE;
	fls->blk.blk_cnt =  MT29F_SIZE / MT29F_PAGE_SIZE;

	/*
	 * TODO: put following stuff in hw_ file dependent (and configurable cs?)
	 * Parameters for MT29F8G08AAD
	 */
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

	SMC_CFG = SMC_CFG_PAGESIZE_PS2048_64
		| SMC_CFG_EDGECTRL
		| SMC_CFG_DTOMUL_X1048576
		| SMC_CFG_DTOCYC(0xF);

	// Disable SMC interrupts, reset and enable NFC controller
	SMC_IDR = ~0;
	SMC_CTRL = 0;
	SMC_CTRL = SMC_CTRL_NFCEN;

	mt29f_reset();
}


void mt29f_init(Mt29f *fls)
{
	common_init(fls);
	fls->blk.priv.vt = &mt29f_buffered_vt;
	fls->blk.priv.flags |= KB_BUFFERED;
	fls->blk.priv.buf = (void *)NFC_SRAM_BASE_ADDR;

	// Load the first block in the cache
	//mt29f_readDirect(&fls->blk, 0, (void *)NFC_SRAM_BASE_ADDR, 0, MT29F_PAGE_SIZE);
	//debug
	memset((void *)NFC_SRAM_BASE_ADDR, 0xbb, MT29F_PAGE_SIZE);
}


void mt29f_initUnbuffered(Mt29f *fls)
{
	common_init(fls);
	fls->blk.priv.vt = &mt29f_unbuffered_vt;
}


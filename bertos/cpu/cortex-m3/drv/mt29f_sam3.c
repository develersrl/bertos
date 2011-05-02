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
#include <drv/timer.h>
#include <drv/mt29f.h>
#include <struct/heap.h>
#include <cpu/power.h> /* cpu_relax() */
#include <cpu/types.h>

#include <string.h> /* memcpy() */

// Timeout for NAND operations in ms
#define MT29F_TMOUT  100

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

// Addresses for sending command, addresses and data bytes to flash
#define MT29F_CMD_ADDR    0x60400000
#define MT29F_ADDR_ADDR   0x60200000
#define MT29F_DATA_ADDR   0x60000000

// Get chip select mask for command register
#define MT29F_CSID(chip)  (((chip)->chip_select << NFC_CMD_CSID_SHIFT) & NFC_CMD_CSID_MASK)


/*
 * Remap info written in the first page of each block
 * used to remap bad blocks.
 */
struct RemapInfo
{
	uint32_t tag;         // Magic number to detect valid info
	uint16_t mapped_blk;  // Bad block the block containing this info is remapping
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
static void getAddrCycles(uint32_t page, uint16_t offset, uint32_t *cycle0, uint32_t *cycle1234)
{
	ASSERT(offset < MT29F_PAGE_SIZE);

	*cycle0 = offset & 0xff;
	*cycle1234 = (page << 8) | ((offset >> 8) & 0xf);

	//LOG_INFO("mt29f addr: %lx %lx\n", *cycle1234, *cycle0);
}


INLINE bool nfcIsBusy(void)
{
	return HWREG(NFC_CMD_BASE_ADDR + NFC_CMD_NFCCMD) & 0x8000000;
}


/*
 * Return true if SMC/NFC controller completed the last operations.
 */
INLINE bool isCmdDone(void)
{
    return SMC_SR & SMC_SR_CMDDONE;
}


/*
 * Wait for edge transition of READY/BUSY NAND
 * signal.
 * Return true for edge detection, false in case of timeout.
 */
static bool waitReadyBusy(void)
{
	time_t start = timer_clock();

	while (!(SMC_SR & SMC_SR_RB_EDGE0))
	{
		cpu_relax();
		if (timer_clock() - start > MT29F_TMOUT)
		{
			LOG_INFO("mt29f: R/B timeout\n");
			return false;
		}
	}

	return true;
}

/*
 * Wait for transfer to complete until timeout.
 * If transfer completes return true, false in case of timeout.
 */
static bool waitTransferComplete(void)
{
	time_t start = timer_clock();

	while (!(SMC_SR & SMC_SR_XFRDONE))
	{
		cpu_relax();
		if (timer_clock() - start > MT29F_TMOUT)
		{
			LOG_INFO("mt29f: xfer complete timeout\n");
			return false;
		}
	}

	return true;
}


/*
 * Send command to NAND and wait for completion.
 */
static void sendCommand(uint32_t cmd,
		int num_cycles, uint32_t cycle0, uint32_t cycle1234)
{
	reg32_t *cmd_addr;

	while (nfcIsBusy());

	if (num_cycles == 5)
		SMC_ADDR = cycle0;

	cmd_addr = (reg32_t *)(NFC_CMD_BASE_ADDR + cmd);
	*cmd_addr = cycle1234;

	while (!isCmdDone());
}


static bool isOperationComplete(Mt29f *chip)
{
	uint8_t status;

	sendCommand(MT29F_CSID(chip) |
		NFC_CMD_NFCCMD | NFC_CMD_ACYCLE_NONE |
		MT29F_CMD_STATUS << 2,
		0, 0, 0);

	status = (uint8_t)HWREG(MT29F_DATA_ADDR);
	return (status & MT29F_STATUS_READY) && !(status & MT29F_STATUS_ERROR);
}


static void chipReset(Mt29f *chip)
{
	sendCommand(MT29F_CSID(chip) |
		NFC_CMD_NFCCMD | NFC_CMD_ACYCLE_NONE |
		MT29F_CMD_RESET << 2,
		0, 0, 0);

	waitReadyBusy();
}


/**
 * Erase the whole block.
 */
int mt29f_blockErase(Mt29f *chip, uint16_t block)
{
	uint32_t cycle0;
	uint32_t cycle1234;

	getAddrCycles(block * MT29F_PAGES_PER_BLOCK, 0, &cycle0, &cycle1234);

	sendCommand(MT29F_CSID(chip) |
		NFC_CMD_NFCCMD | NFC_CMD_ACYCLE_THREE | NFC_CMD_VCMD2 |
		(MT29F_CMD_ERASE_2 << 10) | (MT29F_CMD_ERASE_1 << 2),
		3, 0, cycle1234 >> 8);

	waitReadyBusy();

	if (!isOperationComplete(chip))
	{
		LOG_ERR("mt29f: error erasing block\n");
		chip->status |= MT29F_ERR_ERASE;
		return -1;
	}

	return 0;
}


/**
 * Read Device ID and configuration codes.
 */
bool mt29f_getDevId(Mt29f *chip, uint8_t dev_id[5])
{
	sendCommand(MT29F_CSID(chip) |
		NFC_CMD_NFCCMD | NFC_CMD_NFCEN | NFC_CMD_ACYCLE_ONE |
		MT29F_CMD_READID << 2,
		1, 0, 0);

	waitReadyBusy();
	if (!waitTransferComplete())
	{
		LOG_ERR("mt29f: getDevId timeout\n");
		chip->status |= MT29F_ERR_RD_TMOUT;
		return false;
	}

	memcpy(dev_id, (void *)NFC_SRAM_BASE_ADDR, 5);
	return true;
}


static bool checkEcc(void)
{
	uint32_t sr1 = SMC_ECC_SR1;

	if (sr1)
	{
		LOG_INFO("ECC error, ECC_SR1=0x%lx\n", sr1);
		return false;
	}
	else
		return true;
}


static bool mt29f_readPage(Mt29f *chip, uint32_t page, uint16_t offset)
{
	uint32_t cycle0;
	uint32_t cycle1234;

	//LOG_INFO("mt29f_readPage: page 0x%lx off 0x%x\n", page, offset);

	getAddrCycles(page, offset, &cycle0, &cycle1234);

	sendCommand(MT29F_CSID(chip) |
		NFC_CMD_NFCCMD | NFC_CMD_NFCEN | NFC_CMD_ACYCLE_FIVE | NFC_CMD_VCMD2 |
		(MT29F_CMD_READ_2 << 10) | (MT29F_CMD_READ_1 << 2),
		5, cycle0, cycle1234);

	waitReadyBusy();
	if (!waitTransferComplete())
	{
		LOG_ERR("mt29f: read timeout\n");
		chip->status |= MT29F_ERR_RD_TMOUT;
		return false;
	}

	return true;
}


/*
 * Read page data and ECC, checking for errors.
 * TODO: fix errors with ECC when possible.
 */
bool mt29f_read(Mt29f *chip, uint32_t page, void *buf, uint16_t size)
{
	ASSERT(size <= MT29F_DATA_SIZE);

	if (!mt29f_readPage(chip, page, 0))
		return false;

	memcpy(buf, (void *)NFC_SRAM_BASE_ADDR, size);

	return checkEcc();
}


/*
 * Write data in NFC SRAM buffer to a NAND page, starting at a given offset.
 * Usually offset will be 0 to write data or MT29F_DATA_SIZE to write the spare
 * area.
 *
 * According to datasheet to get ECC computed by hardware is sufficient
 * to write the main area.  But it seems that in that way the last ECC_PR
 * register is not generated.  The workaround is to write data and dummy (ff)
 * spare data in one write, at this point the last ECC_PR is correct and
 * ECC data can be written in the spare area with a second program operation.
 */
static bool mt29f_writePage(Mt29f *chip, uint32_t page, uint16_t offset)
{
	uint32_t cycle0;
	uint32_t cycle1234;

	LOG_INFO("mt29f_writePage: page 0x%lx off 0x%x\n", page, offset);

	getAddrCycles(page, offset, &cycle0, &cycle1234);

	sendCommand(MT29F_CSID(chip) |
			NFC_CMD_NFCCMD | NFC_CMD_NFCWR | NFC_CMD_NFCEN | NFC_CMD_ACYCLE_FIVE |
			MT29F_CMD_WRITE_1 << 2,
			5, cycle0, cycle1234);

	if (!waitTransferComplete())
	{
		LOG_ERR("mt29f: write timeout\n");
		chip->status |= MT29F_ERR_WR_TMOUT;
		return false;
	}

	sendCommand(MT29F_CSID(chip) |
			NFC_CMD_NFCCMD | NFC_CMD_ACYCLE_NONE |
			MT29F_CMD_WRITE_2 << 2,
			0, 0, 0);

	waitReadyBusy();

	if (!isOperationComplete(chip))
	{
		LOG_ERR("mt29f: error writing page\n");
		chip->status |= MT29F_ERR_WRITE;
		return false;
	}

	return true;
}


/*
 * Write data in a page.
 */
static bool mt29f_writePageData(Mt29f *chip, uint32_t page, const void *buf, uint16_t size)
{
	ASSERT(size <= MT29F_DATA_SIZE);

	memset((void *)NFC_SRAM_BASE_ADDR, 0xff, MT29F_PAGE_SIZE);
	memcpy((void *)NFC_SRAM_BASE_ADDR, buf, size);

	return mt29f_writePage(chip, page, 0);
}


/*
 * Write the spare area in a page: ECC and remap block index.
 *
 * ECC data are extracted from ECC_PRx registers and written
 * in the page's spare area.
 * For 2048 bytes pages and 1 ECC word each 256 bytes,
 * 24 bytes of ECC data are stored.
 */
static bool mt29f_writePageSpare(Mt29f *chip, uint32_t page)
{
	int i;
	uint32_t *buf = (uint32_t *)NFC_SRAM_BASE_ADDR;
	uint16_t  blk = page / MT29F_PAGES_PER_BLOCK;
	uint16_t  page_in_blk = page % MT29F_PAGES_PER_BLOCK;
	struct RemapInfo *remap_info = (struct RemapInfo *)(NFC_SRAM_BASE_ADDR + MT29F_REMAP_TAG_OFFSET);

	memset((void *)NFC_SRAM_BASE_ADDR, 0xff, MT29F_SPARE_SIZE);

	for (i = 0; i < MT29F_ECC_NWORDS; i++)
		buf[i] = *((reg32_t *)(SMC_BASE + SMC_ECC_PR0_OFF) + i);

	// Check for remapped block
	if (chip->block_map[blk] != blk)
		page = chip->block_map[blk] * MT29F_PAGES_PER_BLOCK + page_in_blk;

	// Write remap tag in first page in block
	if (page_in_blk == 0)
	{
		remap_info->tag = MT29F_REMAP_TAG;
		remap_info->mapped_blk = blk;
	}

	return mt29f_writePage(chip, page, MT29F_DATA_SIZE);
}


bool mt29f_write(Mt29f *chip, uint32_t page, const void *buf, uint16_t size)
{
	return
		mt29f_writePageData(chip, page, buf, size) &&
		mt29f_writePageSpare(chip, page);
}


int mt29f_error(Mt29f *chip)
{
	return chip->status;
}


void mt29f_clearError(Mt29f *chip)
{
	chip->status = 0;
}


/*
 * Check if the given block is marked bad: ONFI standard mandates
 * that bad block are marked with "00" bytes on the spare area of the
 * first page in block.
 */
static bool blockIsGood(Mt29f *chip, uint16_t blk)
{
	uint8_t *first_byte = (uint8_t *)NFC_SRAM_BASE_ADDR;
	bool good;

	// Check first byte in spare area of first page in block
	mt29f_readPage(chip, blk * MT29F_PAGES_PER_BLOCK, MT29F_DATA_SIZE);
	good = *first_byte == 0xFF;

	if (!good)
		LOG_INFO("mt29f: bad block %d\n", blk);

	return good;
}


/*
 * Return the main partition block remapped on given block in the remap
 * partition (dest_blk).
 */
static int getBadBlockFromRemapBlock(Mt29f *chip, uint16_t dest_blk)
{
	struct RemapInfo *remap_info = (struct RemapInfo *)NFC_SRAM_BASE_ADDR;

	if (!mt29f_readPage(chip, dest_blk * MT29F_PAGES_PER_BLOCK, MT29F_DATA_SIZE + MT29F_REMAP_TAG_OFFSET))
		return -1;

	if (remap_info->tag == MT29F_REMAP_TAG)
		return remap_info->mapped_blk;
	else
		return -1;
}


/*
 * Set a block remapping: src_blk (a block in main data partition) is remappend
 * on dest_blk (block in reserved remapped blocks partition).
 */
static bool setMapping(Mt29f *chip, uint32_t src_blk, uint32_t dest_blk)
{
	struct RemapInfo *remap_info = (struct RemapInfo *)NFC_SRAM_BASE_ADDR;

	LOG_INFO("mt29f, setMapping(): src=%ld dst=%ld\n", src_blk, dest_blk);

	if (!mt29f_readPage(chip, dest_blk * MT29F_PAGES_PER_BLOCK, MT29F_DATA_SIZE + MT29F_REMAP_TAG_OFFSET))
		return false;

	remap_info->tag = MT29F_REMAP_TAG;
	remap_info->mapped_blk = src_blk;

	return mt29f_writePage(chip, dest_blk * MT29F_PAGES_PER_BLOCK, MT29F_DATA_SIZE + MT29F_REMAP_TAG_OFFSET);
}


/*
 * Get a new block from the remap partition to use as a substitute
 * for a bad block.
 */
static uint16_t getFreeRemapBlock(Mt29f *chip)
{
	int blk;

	for (blk = chip->remap_start; blk < MT29F_NUM_BLOCKS; blk++)
	{
		if (blockIsGood(chip, blk))
		{
			chip->remap_start = blk + 1;
			return blk;
		}
	}

	LOG_ERR("mt29f: reserved blocks for bad block remapping exhausted!\n");
	return 0;
}


/*
 * Check if NAND is initialized.
 */
static bool chipIsMarked(Mt29f *chip)
{
	return getBadBlockFromRemapBlock(chip, MT29F_NUM_USER_BLOCKS) != -1;
}


/*
 * Initialize NAND (format). Scan NAND for factory marked bad blocks.
 * All bad blocks found are remapped to the remap partition: each
 * block in the remap partition used to remap bad blocks is marked.
 */
static void initBlockMap(Mt29f *chip)
{
	unsigned b, last;

	// Default is for each block to not be remapped
	for (b = 0; b < MT29F_NUM_USER_BLOCKS; b++)
		chip->block_map[b] = b;
	chip->remap_start = MT29F_NUM_USER_BLOCKS;

	if (chipIsMarked(chip))
	{
		LOG_INFO("mt29f: found initialized NAND, searching for remapped blocks\n");

		// Scan for assigned blocks in remap area
		for (b = last = MT29F_NUM_USER_BLOCKS; b < MT29F_NUM_BLOCKS; b++)
		{
			if (blockIsGood(chip, b))
			{
				int remapped_blk = getBadBlockFromRemapBlock(chip, b);
				if (remapped_blk != -1 && remapped_blk != MT29F_NULL_REMAP)
				{
					LOG_INFO("mt29f: found remapped block %d->%d\n", remapped_blk, b);
					chip->block_map[remapped_blk] = b;
					last = b + 1;
				}
			}
		}
		chip->remap_start = last;
	}
	else
	{
		bool remapped_anything = false;

		LOG_INFO("mt29f: found new NAND, searching for bad blocks\n");

		for (b = 0; b < MT29F_NUM_USER_BLOCKS; b++)
		{
			if (!blockIsGood(chip, b))
			{
				chip->block_map[b] = getFreeRemapBlock(chip);
				setMapping(chip, b, chip->block_map[b]);
				remapped_anything = true;
				LOG_INFO("mt29f: found new bad block %d, remapped to %d\n", b, chip->block_map[b]);
			}
		}

		/*
	     * If no bad blocks are found (we're lucky!) write a dummy
		 * remap to mark NAND and detect we already scanned it next time.
		 */
		if (!remapped_anything)
		{
			setMapping(chip, MT29F_NULL_REMAP, MT29F_NUM_USER_BLOCKS);
			LOG_INFO("mt29f: no bad block founds, marked NAND\n");
		}
	}
}


#ifdef _DEBUG

/*
 * Erase all blocks.
 * DON'T USE on production chips: this function will try to erase
 * factory marked bad blocks too.
 */
static void mt29f_wipe(Mt29f *chip)
{
	int b;
	for (b = 0; b < MT29F_NUM_BLOCKS; b++)
	{
		LOG_INFO("mt29f: erasing block %d\n", b);
		mt29f_blockErase(chip, b);
	}
}

/*
 * Create some bad blocks, erasing them and writing the bad block mark.
 */
static void mt29f_ruinSomeBlocks(Mt29f *chip)
{
	int bads[] = { 7, 99, 555, 1003, 1004, 1432 };
	unsigned i;

	LOG_INFO("mt29f: erasing mark\n");
	mt29f_blockErase(chip, MT29F_NUM_USER_BLOCKS);

	for (i = 0; i < countof(bads); i++)
	{
		LOG_INFO("mt29f: erasing block %d\n", bads[i]);
		mt29f_blockErase(chip, bads[i]);

		LOG_INFO("mt29f: marking page %d as bad\n", bads[i] * MT29F_PAGES_PER_BLOCK);
		memset((void *)NFC_SRAM_BASE_ADDR, 0, MT29F_SPARE_SIZE);
		mt29f_writePage(chip, bads[i] * MT29F_PAGES_PER_BLOCK, MT29F_DATA_SIZE);
	}
}

#endif


static void initPio(void)
{
	/*
	 * TODO: put following stuff in hw_ file dependent
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
}


static void initSmc(void)
{
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
		| SMC_CFG_DTOCYC(0xF)
		| SMC_CFG_WSPARE
		| SMC_CFG_RSPARE;

	// Disable SMC interrupts, reset and enable NFC controller
	SMC_IDR = ~0;
	SMC_CTRL = 0;
	SMC_CTRL = SMC_CTRL_NFCEN;

	// Enable ECC, 1 ECC per 256 bytes
	SMC_ECC_CTRL = SMC_ECC_CTRL_SWRST;
	SMC_ECC_MD = SMC_ECC_MD_ECC_PAGESIZE_PS2048_64 | SMC_ECC_MD_TYPCORREC_C256B;
}


bool mt29f_init(Mt29f *chip, struct Heap *heap, uint8_t chip_select)
{
	memset(chip, 0, sizeof(Mt29f));

	chip->chip_select = chip_select;
	chip->block_map = heap_allocmem(heap, MT29F_NUM_USER_BLOCKS * sizeof(*chip->block_map));
	if (!chip->block_map)
	{
		LOG_ERR("mt29f: error allocating block map\n");
		return false;
	}

	initPio();
	initSmc();
	chipReset(chip);
	initBlockMap(chip);

	return true;
}


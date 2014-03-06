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
 * Copyright 2010 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief STM32F103xx internal flash memory driver.
 *
 * \author Daniele Basile <asterix@develer.com>
 */

#include "flash_stm32.h"

#include "cfg/cfg_emb_flash.h"

// Define log settings for cfg/log.h
#define LOG_LEVEL    CONFIG_FLASH_EMB_LOG_LEVEL
#define LOG_FORMAT   CONFIG_FLASH_EMB_LOG_FORMAT
#include <cfg/log.h>

#include <drv/timer.h>
#include <drv/flash.h>

#include <cpu/power.h>
#include <cpu/detect.h>

#include <io/stm32.h>

#include <string.h>

#define EMB_FLASH                ((struct stm32_flash*)FLASH_R_BASE)

struct FlashHardware
{
	uint8_t status;
};

static bool flash_wait(struct KBlock *blk)
{
	Flash *fls = FLASH_CAST(blk);
	ticks_t start = timer_clock();
	while (EMB_FLASH->SR & FLASH_FLAG_BSY)
	{
		cpu_relax();

		if (timer_clock() - start > ms_to_ticks(CONFIG_FLASH_WR_TIMEOUT))
		{
			fls->hw->status |= FLASH_WR_TIMEOUT;
			LOG_ERR("Timeout..\n");
			return false;
		}
	}

#if CPU_CM3_STM32F1
	if (EMB_FLASH->SR & FLASH_FLAG_PGERR)
	{
		fls->hw->status |= FLASH_NOT_ERASED;
		LOG_ERR("flash not erased..\n");
		return false;
	}
#else
	if (EMB_FLASH->SR & FLASH_FLAGS_PGERR)
	{
		fls->hw->status |= FLASH_WR_ERR;
		LOG_ERR("flash write error.. 0x%lx\n", EMB_FLASH->SR & FLASH_FLAGS_PGERR);
		/* clear error flags */
		EMB_FLASH->SR |= FLASH_FLAGS_PGERR;
		return false;
	}
#endif

	if (EMB_FLASH->SR & FLASH_FLAG_WRPRTERR)
	{
		fls->hw->status |= FLASH_WR_PROTECT;
		LOG_ERR("wr protect..\n");
#if CPU_CM3_STM32F2
		/* clear error flag */
		EMB_FLASH->SR |= FLASH_FLAG_WRPRTERR;
#endif
		return false;
	}

	return true;
}

#if CPU_CM3_STM32F1

static bool stm32_erasePage(struct KBlock *blk, uint32_t page_add)
{

	EMB_FLASH->CR |= CR_PER_SET;
	EMB_FLASH->AR = page_add;
	EMB_FLASH->CR |= CR_STRT_SET;

	if (!flash_wait(blk))
		return false;

	EMB_FLASH->CR &= CR_PER_RESET;

	return true;
}

#else

/* F2xx processors have 4 x 16k sectors, 1 x 64k sector, 7 x 128k sectors */
static uint16_t sector_boundaries[] = {
	0,  16,
	16, 32,
	32, 48,
	48, 64,
	64, 128,
	128 * 1, 128 * 2,
	128 * 2, 128 * 3,
	128 * 3, 128 * 4,
	128 * 4, 128 * 5,
	128 * 5, 128 * 6,
	128 * 6, 128 * 7,
	128 * 7, 128 * 8,
};

static int8_t stm32_sectorIndex(uint32_t page_add)
{
	/* check the adress is at Kb boundary */
	if (page_add & 0x3ff)
		return -1;

	uint32_t page_kb = page_add >> 10;

	for (size_t i = 0; i < countof(sector_boundaries); i += 2)
		if (page_kb == sector_boundaries[i])
			return i / 2;

	return -1;
}

static bool stm32_eraseSector(struct KBlock *blk, uint8_t sector)
{
	/* clear error flags */
	EMB_FLASH->SR |= FLASH_FLAGS_PGERR | FLASH_FLAG_WRPRTERR;

	EMB_FLASH->CR |= CR_SER_SET;
	EMB_FLASH->CR &= ~(0xf << 3);
	EMB_FLASH->CR |= sector << 3;
	EMB_FLASH->CR |= CR_STRT_SET;

	if (!flash_wait(blk))
		return false;

	return true;
}

#endif

#if 0
// not used for now
static bool stm32_eraseAll(struct KBlock *blk)
{
	EMB_FLASH->CR |= CR_MER_SET;
	EMB_FLASH->CR |= CR_STRT_SET;

	if (!flash_wait(blk))
		return false;

	EMB_FLASH->CR &= CR_MER_RESET;

	return true;
}
#endif

static int stm32_flash_error(struct KBlock *blk)
{
	Flash *fls = FLASH_CAST(blk);
	return fls->hw->status;
}

static void stm32_flash_clearerror(struct KBlock *blk)
{
	Flash *fls = FLASH_CAST(blk);
	fls->hw->status = 0;
}

static size_t stm32_flash_readDirect(struct KBlock *blk, block_idx_t idx, void *buf, size_t offset, size_t size)
{
	memcpy(buf, (void *)(idx * blk->blk_size + offset), size);
	return size;
}


INLINE bool stm32_writeWord(struct KBlock *blk, uint32_t addr, uint16_t data)
{
	ASSERT(!(addr % 2));

#if CPU_CM3_STM32F2
	/* clear error flags */
	EMB_FLASH->SR |= FLASH_FLAGS_PGERR | FLASH_FLAG_WRPRTERR;
#endif

	EMB_FLASH->CR |= CR_PG_SET;

	*(reg16_t *)addr = data;

	if (!flash_wait(blk))
		return false;

	EMB_FLASH->CR &= CR_PG_RESET;

	return true;
}

#if CPU_CM3_STM32F2
DB(block_idx_t last_block);
#endif

static size_t stm32_flash_writeDirect(struct KBlock *blk, block_idx_t idx, const void *_buf, size_t offset, size_t size)
{
	ASSERT(offset == 0);
	ASSERT(size == blk->blk_size);

#if CPU_CM3_STM32F1
	if (!stm32_erasePage(blk, (idx * blk->blk_size)))
		return 0;
#else
	ASSERT(last_block < idx);

	int sector = stm32_sectorIndex(idx * blk->blk_size);

	if (sector != -1 && !stm32_eraseSector(blk, sector))
		return 0;
#endif

	uint32_t addr = idx * blk->blk_size;
	const uint8_t *buf = (const uint8_t *)_buf;

	while (size)
	{
		uint16_t data = (*(buf + 1) << 8) | *buf;
		if (!stm32_writeWord(blk, addr, data))
			return 0;

		buf += 2;
		size -= 2;
		addr += 2;
	}

	return blk->blk_size;
}

static const KBlockVTable flash_stm32_buffered_vt =
{
	.readDirect = stm32_flash_readDirect,
	.writeDirect = stm32_flash_writeDirect,

	.readBuf = kblock_swReadBuf,
	.writeBuf = kblock_swWriteBuf,
	.load = kblock_swLoad,
	.store = kblock_swStore,

	.close = kblock_swClose,

	.error = stm32_flash_error,
	.clearerr = stm32_flash_clearerror,
};

static const KBlockVTable flash_stm32_unbuffered_vt =
{
	.readDirect = stm32_flash_readDirect,
	.writeDirect = stm32_flash_writeDirect,

	.close = kblock_swClose,

	.error = stm32_flash_error,
	.clearerr = stm32_flash_clearerror,
};

static struct FlashHardware flash_stm32_hw;

static void common_init(Flash *fls)
{
	memset(fls, 0, sizeof(*fls));
	DB(fls->blk.priv.type = KBT_FLASH);

	fls->hw = &flash_stm32_hw;

	fls->blk.blk_size = FLASH_PAGE_SIZE_BYTES;
	fls->blk.blk_cnt = (F_SIZE * 1024) / FLASH_PAGE_SIZE_BYTES;

	/* Unlock flash memory for the FPEC Access */
	EMB_FLASH->KEYR = FLASH_KEY1;
	EMB_FLASH->KEYR = FLASH_KEY2;

#if CPU_CM3_STM32F2
	/* set program parallelism size to 16 */
	EMB_FLASH->CR &= ~(0x3 << 8);
	EMB_FLASH->CR |=  (0x1 << 8);
#endif
}

static uint8_t flash_buf[FLASH_PAGE_SIZE_BYTES];

void flash_hw_init(Flash *fls, UNUSED_ARG(int, flags))
{
	common_init(fls);
	fls->blk.priv.vt = &flash_stm32_buffered_vt;
	fls->blk.priv.flags |= KB_BUFFERED | KB_PARTIAL_WRITE;
	fls->blk.priv.buf = flash_buf;

	/* Load the first block in the cache */
	void *flash_start = 0x0;
	memcpy(fls->blk.priv.buf, flash_start, fls->blk.blk_size);
}

void flash_hw_initUnbuffered(Flash *fls, UNUSED_ARG(int, flags))
{
	common_init(fls);
	fls->blk.priv.vt = &flash_stm32_unbuffered_vt;
}

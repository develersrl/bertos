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
 * \author Francesco Sacchi <batt@develer.com>
 * \author Daniele Basile <asterix@develer.com>
 *
 * \brief NPX lpc23xx embedded flash read/write driver.
 */

#include "flash_lpc2.h"
#include "cfg/cfg_emb_flash.h"

// Define log settings for cfg/log.h
#define LOG_LEVEL    CONFIG_FLASH_EMB_LOG_LEVEL
#define LOG_FORMAT   CONFIG_FLASH_EMB_LOG_FORMAT
#include <cfg/log.h>
#include <cfg/macros.h>

#include <cpu/irq.h>
#include <cpu/attr.h>
#include <cpu/power.h>

#include <kern/kfile.h>

#include <io/arm.h>

#include <drv/timer.h>
#include <drv/flash.h>

#include <string.h>


#define IAP_ADDRESS 0x7ffffff1

typedef enum IapCommands
{
	PREPARE_SECTOR_FOR_WRITE = 50,
	COPY_RAM_TO_FLASH = 51,
	ERASE_SECTOR = 52,
	BLANK_CHECK_SECTOR = 53,
	READ_PART_ID = 54,
	READ_BOOT_VER = 55,
	COMPARE = 56,
	REINVOKE_ISP = 57,
} IapCommands;


#define CMD_SUCCESS 0

typedef struct IapCmd
{
	uint32_t cmd;
	uint32_t param[4];
} IapCmd;

typedef struct IapRes
{
	uint32_t status;
	uint32_t res[2];
} IapRes;

typedef void (*iap_callback_t)(IapCmd *, IapRes *);

iap_callback_t iap=(iap_callback_t)IAP_ADDRESS;

#define FLASH_MEM_SIZE (504 * 1024L)

static size_t flash_start_addr;

static size_t sector_size(page_t page)
{
	if (page < 8)
		return 4096;
	else if (page < 22)
		return 32768;
	else if (page < 28)
		return 4096;

	ASSERT(0);
	return 0;
}

static size_t sector_addr(page_t page)
{
	if (page < 8)
		return page * 4096;
	else if (page < 22)
		return (page - 8) * 32768 + 4096 * 8;
	else if (page < 28)
		return (page - 22) * 4096 + 32768 * 14 + 4096 * 8;

	ASSERT(0);
	return 0;
}


static page_t addr_to_sector(size_t addr)
{
	if (addr < 4096 * 8)
		return addr / 4096;
	else if (addr < 4096 * 8 + 32768L * 14)
		return ((addr - 4096 * 8) / 32768) + 8;
	else if (addr < 4096 * 8 + 32768L * 14 + 4096 * 6)
		return ((addr - 4096 * 8 - 32768L * 14) / 4096) + 22;

	ASSERT(0);
	return 0;
}

static page_addr_t addr_to_pageaddr(size_t addr)
{
	if (addr < 4096 * 8)
		return addr % 4096;
	else if (addr < 4096 * 8 + 32768L * 14)
		return (addr - 4096 * 8) % 32768;
	else if (addr < 4096 * 8 + 32768L * 14 + 4096 * 6)
		return (addr - 4096 * 8 - 32768L * 14) % 4096;

	ASSERT(0);
	return 0;
}


static uint32_t page_buf[1024];//MAX_FLASH_PAGE_SIZE / sizeof(uint32_t)];
static bool page_dirty;
static page_t curr_page;

/**
 * Send write command.
 *
 * After WR command cpu write bufferd page into flash memory.
 *
 */
static void flash_lpc2_writePage(page_t page, uint32_t *buf)
{
	cpu_flags_t flags;

	//Compute page address of current page.
	page_addr_t addr = sector_addr(page);

	LOG_INFO("Writing page %ld...\n", page);

	IRQ_SAVE_DISABLE(flags);

	IapCmd cmd;
	IapRes res;
	cmd.cmd = PREPARE_SECTOR_FOR_WRITE;
	cmd.param[0] = cmd.param[1] = page;
	iap(&cmd, &res);
	if (res.status != CMD_SUCCESS)
		LOG_ERR("%ld\n", res.status);

	cmd.cmd = ERASE_SECTOR;
	cmd.param[0] = cmd.param[1] = page;
	cmd.param[2] = CPU_FREQ / 1000;
	iap(&cmd, &res);
	if (res.status != CMD_SUCCESS)
		LOG_ERR("%ld\n", res.status);

	size_t size = sector_size(page);

	while (size)
	{
		LOG_INFO("Writing page %ld, addr %ld, size %d\n", page, addr, size);
		cmd.cmd = PREPARE_SECTOR_FOR_WRITE;
		cmd.param[0] = cmd.param[1] = page;
		iap(&cmd, &res);
		if (res.status != CMD_SUCCESS)
			LOG_ERR("%ld\n", res.status);

		cmd.cmd = COPY_RAM_TO_FLASH;
		cmd.param[0] = addr;
		cmd.param[1] = (uint32_t)buf;
		cmd.param[2] = 4096;
		cmd.param[3] = CPU_FREQ / 1000;
		iap(&cmd, &res);
		if (res.status != CMD_SUCCESS)
			LOG_ERR("%ld\n", res.status);

		size -= 4096;
		addr += 4096;
		buf += 4096 / sizeof(uint32_t);
	}

	IRQ_RESTORE(flags);
	LOG_INFO("Done\n");
}

/**
 * Write modified page on internal latch, and then send write command to
 * flush page to internal flash.
 */
static int flash_lpc2_flush(UNUSED_ARG(KFile *, _fd))
{
	if (page_dirty)
	{
		flash_lpc2_writePage(curr_page, page_buf);
		page_dirty = false;
	}
	return 0;
}


/**
 * Check current page and if \a page is different, load it in
 * temporary buffer.
 */
static void flash_lpc2_loadPage(KFile *fd, page_t page)
{
	if (page != curr_page)
	{
		flash_lpc2_flush(fd);

		size_t addr = sector_addr(page);
		size_t size = sector_size(page);
		LOG_INFO("page %ld, addr %d, size %d\n", page, addr, size);
		// Load page
		memcpy(page_buf, (char *)addr, size);
		curr_page = page;
		LOG_INFO("Loaded page %lu\n", curr_page);
	}
}

/**
 * Write program memory.
 * Write \a size bytes from buffer \a _buf to file \a fd
 * \note Write operations are buffered.
 */
static size_t flash_lpc2_write(struct KFile *fd, const void *_buf, size_t size)
{
	const uint8_t *buf =(const uint8_t *)_buf;

	page_t page;
	page_addr_t page_addr;
	size_t total_write = 0;

	size = MIN((kfile_off_t)size, fd->size - fd->seek_pos);

	LOG_INFO("Writing at pos[%lu]\n", fd->seek_pos);
	while (size)
	{
		page = addr_to_sector(fd->seek_pos + flash_start_addr);
		page_addr = addr_to_pageaddr(fd->seek_pos + flash_start_addr);
		LOG_INFO("addr %ld, page %ld, page_addr %ld\n", fd->seek_pos + flash_start_addr, page, page_addr);

		flash_lpc2_loadPage(fd, page);

		size_t wr_len = MIN(size, (size_t)(sector_size(page) - page_addr));

		memcpy(((uint8_t *)page_buf) + page_addr, buf, wr_len);
		page_dirty = true;

		buf += wr_len;
		fd->seek_pos += wr_len;
		size -= wr_len;
		total_write += wr_len;
	}
	LOG_INFO("written %u bytes\n", total_write);
	return total_write;
}

/**
 * Open flash file \a fd
 * \a name and \a mode are unused, cause flash memory is
 * threated like one file.
 */
static void flash_lpc2_open(struct FlashLpc2 *fd)
{
	curr_page = addr_to_sector(FLASH_BOOT_SIZE);
	if (addr_to_pageaddr(FLASH_BOOT_SIZE))
		curr_page++;

	flash_start_addr = sector_addr(curr_page);
	LOG_INFO("Boot size %d, curr_page %ld, flash_start_addr %d\n", FLASH_BOOT_SIZE, curr_page, flash_start_addr);

	fd->fd.size = FLASH_MEM_SIZE - sector_addr(curr_page);
	fd->fd.seek_pos = 0;

	memcpy(page_buf, (char *)sector_addr(curr_page), sector_size(curr_page));

	page_dirty = false;
	LOG_INFO("Flash file opened, pos %ld, size %ld\n", fd->fd.seek_pos, fd->fd.size);
}

/**
 * Read from file \a fd \a size bytes and put it in buffer \a buf
 * \return the number of bytes read.
 */
static size_t flash_lpc2_read(struct KFile *fd, void *_buf, size_t size)
{
	uint8_t *buf =(uint8_t *)_buf;

	size = MIN((kfile_off_t)size, fd->size - fd->seek_pos);

	LOG_INFO("Reading at pos[%lu]\n", fd->seek_pos);

	// Flush current buffered page (if modified).
	flash_lpc2_flush(fd);

	kfile_off_t *addr = (kfile_off_t *)(fd->seek_pos + flash_start_addr);
	LOG_INFO("actual addr %ld\n", (uint32_t)addr);
	memcpy(buf, addr, size);

	for (unsigned i = 0; i< size; i++)
	{
		if (i % 16 == 0)
			kputchar('\n');

		kprintf("%02x ", buf[i]);
	}
	kputchar('\n');

	fd->seek_pos += size;

	LOG_INFO("Read %u bytes\n", size);
	return size;
}


/**
 * Init module to perform write and read operation on internal
 * flash memory.
 */
void flash_hw_init(struct FlashLpc2 *fd)
{
	memset(fd, 0, sizeof(*fd));
	// Init base class.
	kfile_init(&fd->fd);
	DB(fd->fd._type = KFT_FLASH);

	// Set up flash programming functions.
	fd->fd.write = flash_lpc2_write;
	fd->fd.read = flash_lpc2_read;
	fd->fd.flush = flash_lpc2_flush;

	flash_lpc2_open(fd);
	TRACE;
}

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
 * Copyright 2007 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief Self programming routines.
 *
 * \author Francesco Sacchi <batt@develer.com>
 * \author Daniele Basile <asterix@develer.com>
 *
 * This module implements a kfile-like access for Atmel avr
 * internal flash.
 * Internal flash writing access is controlled by BOOTSZ fuses, check
 * datasheet for details.
 */

#include "flash_avr.h"

#include "cfg/cfg_flash_avr.h"
#include <cfg/macros.h> // MIN()
#include <cfg/compiler.h>
#include <cfg/debug.h>
#include <cpu/irq.h>

// Define log settings for cfg/log.h
#define LOG_LEVEL    CONFIG_FLASH_AVR_LOG_LEVEL
#define LOG_FORMAT   CONFIG_FLASH_AVR_LOG_FORMAT
#include <cfg/log.h>

#include <drv/wdt.h>
#include <drv/flash.h>

#include <kern/kfile.h>

#include <avr/io.h>
#include <avr/boot.h>
#include <avr/pgmspace.h>

#include <string.h>


/**
 * Definition of type for avr flash module.
 */
typedef uint16_t page_addr_t;


/**
 * Private avr flush funtion.
 *
 * Write current buffered page in flash memory (if modified).
 * This function erase flash memory page before writing.
 *
 * This function is only use internally in this module.
 */
static void flash_avr_flush(Flash *fd)
{
	if (fd->page_dirty)
	{

		LOG_INFO("Flushing page %d\n", fd->curr_page);

		// Wait while the SPM instruction is busy.
		boot_spm_busy_wait();

		LOG_INFO("Filling temparary page buffer...");

		// Fill the temporary buffer of the AVR
		for (page_addr_t page_addr = 0; page_addr < SPM_PAGESIZE; page_addr += 2)
		{
			uint16_t word = ((uint16_t)fd->page_buf[page_addr + 1] << 8) | fd->page_buf[page_addr];

			ATOMIC(boot_page_fill(page_addr, word));
		}
		LOG_INFO("Done.\n");

		wdt_reset();

		LOG_INFO("Erasing page, addr %u...", fd->curr_page * SPM_PAGESIZE);

		/* Page erase */
		ATOMIC(boot_page_erase(fd->curr_page * SPM_PAGESIZE));

		/* Wait until the memory is erased. */
		boot_spm_busy_wait();

		LOG_INFO("Done.\n");
		LOG_INFO("Writing page, addr %u...", fd->curr_page * SPM_PAGESIZE);

		/* Store buffer in flash page. */
		ATOMIC(boot_page_write(fd->curr_page * SPM_PAGESIZE));
		boot_spm_busy_wait();  // Wait while the SPM instruction is busy.

		/*
		* Reenable RWW-section again. We need this if we want to jump back
		* to the application after bootloading.
		*/
		ATOMIC(boot_rww_enable());

		fd->page_dirty = false;
		LOG_INFO("Done.\n");
	}
}


/**
 * Flush avr flash function.
 *
 * Write current buffered page in flash memory (if modified).
 * This function erase flash memory page before writing.
 */
static int flash_avr_kfileFlush(struct KFile *_fd)
{
	Flash *fd = FLASH_CAST(_fd);
	flash_avr_flush(fd);
	return 0;
}


/**
 * Check current page and if \a page is different, load it in
 * temporary buffer.
 */
static void flash_avr_loadPage(Flash *fd, page_t page)
{
	if (page != fd->curr_page)
	{
		flash_avr_flush(fd);
		// Load page
		memcpy_P(fd->page_buf, (const char *)(page * SPM_PAGESIZE), SPM_PAGESIZE);
		fd->curr_page = page;
		LOG_INFO("Loaded page %d\n", fd->curr_page);
	}
}

/**
 * Write program memory.
 * Write \a size bytes from buffer \a _buf to file \a fd
 * \note Write operations are buffered.
 */
static size_t flash_avr_write(struct KFile *_fd, const void *_buf, size_t size)
{
	Flash *fd = FLASH_CAST(_fd);
	const uint8_t *buf =(const uint8_t *)_buf;

	page_t page;
	page_addr_t page_addr;
	size_t total_write = 0;


	ASSERT(fd->fd.seek_pos + (kfile_off_t)size <= (kfile_off_t)fd->fd.size);
	size = MIN((kfile_off_t)size, fd->fd.size - fd->fd.seek_pos);

	LOG_INFO("Writing at pos[%u]\n", fd->fd.seek_pos);
	while (size)
	{
		page = fd->fd.seek_pos / SPM_PAGESIZE;
		page_addr = fd->fd.seek_pos % SPM_PAGESIZE;

		flash_avr_loadPage(fd, page);

		size_t wr_len = MIN(size, SPM_PAGESIZE - page_addr);
		memcpy(fd->page_buf + page_addr, buf, wr_len);
		fd->page_dirty = true;

		buf += wr_len;
		fd->fd.seek_pos += wr_len;
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
static void flash_avr_open(struct Flash *fd)
{
	fd->curr_page = 0;
	memcpy_P(fd->page_buf, (const char *)(fd->curr_page * SPM_PAGESIZE), SPM_PAGESIZE);

	fd->fd.seek_pos = 0;
	fd->fd.size = (uint16_t)(FLASHEND - CONFIG_FLASH_AVR_BOOTSIZE + 1);
	fd->page_dirty = false;

	LOG_INFO("Flash file opened\n");
}

/**
 * Close file \a fd
 */
static int flash_avr_close(struct KFile *_fd)
{
	Flash *fd = FLASH_CAST(_fd);
	flash_avr_flush(fd);
	LOG_INFO("Flash file closed\n");
	return 0;
}

/**
 * Reopen file \a fd
 */
static struct KFile *flash_avr_reopen(struct KFile *fd)
{
	Flash *_fd = FLASH_CAST(fd);
	flash_avr_close(fd);
	flash_avr_open((struct Flash *)_fd);
	return fd;
}


/**
 * Read from file \a fd \a size bytes and put it in buffer \a buf
 * \return the number of bytes read.
 */
static size_t flash_avr_read(struct KFile *_fd, void *buf, size_t size)
{
	Flash *fd = FLASH_CAST(_fd);
	ASSERT(fd->fd.seek_pos + (kfile_off_t)size <= (kfile_off_t)fd->fd.size);
	size = MIN((kfile_off_t)size, fd->fd.size - fd->fd.seek_pos);

	LOG_INFO("Reading at pos[%u]\n", fd->fd.seek_pos);
	// Flush current buffered page (if modified).
	flash_avr_flush(fd);

	/*
	 * AVR pointers are 16 bits wide, this hack is needed to avoid
	 * compiler warning, cause fd->seek_pos is a 32bit offset.
	 */
	const uint8_t *pgm_addr = (const uint8_t *)0;
	pgm_addr += fd->fd.seek_pos;

	memcpy_P(buf, pgm_addr, size);
	fd->fd.seek_pos += size;
	LOG_INFO("Read %u bytes\n", size);
	return size;
}

/**
 * Init AVR flash read/write file.
 */
void flash_hw_init(struct Flash *fd)
{
	memset(fd, 0, sizeof(*fd));
	DB(fd->fd._type = KFT_FLASH);

	// Set up flash programming functions.
	fd->fd.reopen = flash_avr_reopen;
	fd->fd.close = flash_avr_close;
	fd->fd.read = flash_avr_read;
	fd->fd.write = flash_avr_write;
	fd->fd.seek = kfile_genericSeek;
	fd->fd.flush = flash_avr_kfileFlush;
	fd->curr_page = 0;

	flash_avr_open(fd);
}



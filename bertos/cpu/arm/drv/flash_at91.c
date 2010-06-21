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
 * Copyright 2009 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \author Daniele Basile <asterix@develer.com>
 *
 * \brief At91sam7 Internal flash read/write driver.
 *
 *
 */

#include "flash_at91.h"

#include "cfg/cfg_flash_at91.h"
#include <cfg/macros.h>

#include "hw/hw_boot.h"

// Define log settings for cfg/log.h
#define LOG_LEVEL    CONFIG_FLASH_AT91_LOG_LEVEL
#define LOG_FORMAT   CONFIG_FLASH_AT91_LOG_FORMAT
#include <cfg/log.h>


#include <cpu/irq.h>
#include <cpu/attr.h>
#include <cpu/power.h>

#include <kern/kfile.h>

#include <io/arm.h>

#include <drv/timer.h>
#include <drv/flash.h>

#include <string.h>

#define FLASH_START_PAGE DIV_ROUNDUP(FLASH_BOOT_SIZE, FLASH_PAGE_SIZE_BYTES)
#define FLASH_START_ADDR (FLASH_START_PAGE * FLASH_PAGE_SIZE_BYTES)

/**
 * Really send the flash write command.
 * 
 * \note This function has to be placed in RAM because 
 *       executing code from flash while a writing process
 *       is in progress is forbidden.
 */ 
RAM_FUNC NOINLINE static void write_page(uint32_t page)
{
	// Send the 'write page' command
	MC_FCR = MC_KEY | MC_FCMD_WP | (MC_PAGEN_MASK & (page << 8));

	// Wait for the end of command
	while(!(MC_FSR & BV(MC_FRDY)))
	{
		//NOP;
	}
}


/**
 * Send write command.
 *
 * After WR command cpu write bufferd page into flash memory.
 * 
 */
INLINE void flash_at91_sendWRcmd(uint32_t page)
{
	cpu_flags_t flags;

	LOG_INFO("Writing page %ld...\n", page);

	IRQ_SAVE_DISABLE(flags);
	write_page(page);

	IRQ_RESTORE(flags);
	LOG_INFO("Done\n");
}

/**
 * Return 0 if no error are occurred after flash memory
 * read or write operation, otherwise return error code.
 */
static int flash_at91_getStatus(UNUSED_ARG(struct KFile *, _fd))
{
	/*
	 * This bit is set to one if we programming of at least one locked lock
	 * region.
	 */
	if(MC_FSR & BV(MC_LOCKE))
		return -1;

	/*
	 * This bit is set to one if an invalid command and/or a bad keywords was/were
	 * written in the Flash Command Register.
	 */
	if(MC_FSR & BV(MC_PROGE))
		return -2;

	return 0;
}


/**
 * Write modified page on internal latch, and then send write command to
 * flush page to internal flash.
 */
static int flash_at91_flush(KFile *_fd)
{
	Flash *fd = FLASH_CAST(_fd);
	if (fd->page_dirty)
	{
		//Compute page address of current page.
		page_addr_t *addr = (page_addr_t *)((fd->curr_page * FLASH_PAGE_SIZE_BYTES) + FLASH_BASE);

		//Copy modified page into internal latch.
		for (page_addr_t page_addr = 0; page_addr < FLASH_PAGE_SIZE_BYTES; page_addr += 4)
		{
			// This is needed in order to have a single 32bit write instruction in addr.
			// (8 and 16 writes cause unpredictable results).
			uint32_t data;
			memcpy(&data, &fd->page_buf[page_addr], sizeof(data));
			*addr++ = data;
		}

		// Send write command to transfer page from latch to internal flash memory.
		flash_at91_sendWRcmd(fd->curr_page);
		fd->page_dirty = false;
	}
	return 0;
}


/**
 * Check current page and if \a page is different, load it in
 * temporary buffer.
 */
static void flash_at91_loadPage(Flash *fd, page_t page)
{
	if (page != fd->curr_page)
	{
		flash_at91_flush(&fd->fd);
		// Load page
		memcpy(fd->page_buf, (char *)((page * FLASH_PAGE_SIZE_BYTES) + FLASH_BASE), FLASH_PAGE_SIZE_BYTES);
		fd->curr_page = page;
		LOG_INFO("Loaded page %lu\n", fd->curr_page);
	}
}


/**
 * Write program memory.
 * Write \a size bytes from buffer \a _buf to file \a fd
 * \note Write operations are buffered.
 */
static size_t flash_at91_write(struct KFile *_fd, const void *_buf, size_t size)
{
	Flash *fd = FLASH_CAST(_fd);
	const uint8_t *buf =(const uint8_t *)_buf;

	page_t page;
	page_addr_t page_addr;
	size_t total_write = 0;

	size = MIN((kfile_off_t)size, fd->fd.size - fd->fd.seek_pos);

	LOG_INFO("Writing at pos[%lu]\n", fd->fd.seek_pos);
	while (size)
	{
		page = (fd->fd.seek_pos + FLASH_START_ADDR) / FLASH_PAGE_SIZE_BYTES;
		page_addr = (fd->fd.seek_pos + FLASH_START_ADDR) % FLASH_PAGE_SIZE_BYTES;

		flash_at91_loadPage(fd, page);

		size_t wr_len = MIN(size, (size_t)(FLASH_PAGE_SIZE_BYTES - page_addr));

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
static void flash_at91_open(struct Flash *fd)
{
	fd->curr_page = FLASH_START_PAGE;
	fd->fd.size = FLASH_MEM_SIZE - fd->curr_page * FLASH_PAGE_SIZE_BYTES;
	fd->fd.seek_pos = 0;
	
	memcpy(fd->page_buf, (char *)((fd->curr_page * FLASH_PAGE_SIZE_BYTES) + FLASH_BASE), FLASH_PAGE_SIZE_BYTES);

	fd->page_dirty = false;
	LOG_INFO("Flash file opened, pos %ld, size %ld\n", fd->fd.seek_pos, fd->fd.size);
}

/**
 * Read from file \a fd \a size bytes and put it in buffer \a buf
 * \return the number of bytes read.
 */
static size_t flash_at91_read(struct KFile *_fd, void *_buf, size_t size)
{
	Flash *fd = FLASH_CAST(_fd);
	uint8_t *buf =(uint8_t *)_buf;

	size = MIN((kfile_off_t)size, fd->fd.size - fd->fd.seek_pos);

	LOG_INFO("Reading at pos[%lu]\n", fd->fd.seek_pos);

	// Flush current buffered page (if modified).
	flash_at91_flush(&fd->fd);

	kfile_off_t *addr = (kfile_off_t *)(fd->fd.seek_pos + FLASH_START_ADDR);
	memcpy(buf, addr, size);

	fd->fd.seek_pos += size;

	LOG_INFO("Read %u bytes\n", size);
	return size;
}


/**
 * Init module to perform write and read operation on internal
 * flash memory.
 */
void flash_hw_init(struct Flash *fd)
{
	memset(fd, 0, sizeof(*fd));
	// Init base class.
	kfile_init(&fd->fd);
	DB(fd->fd._type = KFT_FLASH);

	// Set up flash programming functions.
	fd->fd.write = flash_at91_write;
	fd->fd.read = flash_at91_read;
	fd->fd.error = flash_at91_getStatus;
	fd->fd.flush = flash_at91_flush;

	flash_at91_open(fd);
}

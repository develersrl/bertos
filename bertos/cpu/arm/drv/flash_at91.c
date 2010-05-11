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

/*
 * Check if flash memory is ready to accept other commands.
 */
RAM_FUNC static bool flash_at91_isReady(void)
{
	return (MC_FSR & BV(MC_FRDY));
}

/**
 * Send write command.
 *
 * After WR command cpu write bufferd page into flash memory.
 */
RAM_FUNC static void flash_at91_sendWRcmd(uint32_t page)
{
	cpu_flags_t flags;

	// Wait for end of command
	while(!flash_at91_isReady())
	{
		cpu_relax();
	}

	IRQ_SAVE_DISABLE(flags);

	// Send the 'write page' command
	MC_FCR = MC_KEY | MC_FCMD_WP | (MC_PAGEN_MASK & (page << 8));

	// Wait for end of command
	while(!flash_at91_isReady())
	{
		cpu_relax();
	}

	IRQ_RESTORE(flags);
}

/**
 * Return 0 if no error are occurred after flash memory
 * read or write operation, otherwise return error code.
 */
RAM_FUNC static int flash_at91_getStatus(struct KFile *_fd)
{
	(void)_fd;


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
RAM_FUNC static void flash_at91_flush(Flash *fd)
{
	if (fd->page_dirty)
	{
		//Compute page address of current page.
		page_addr_t *addr = (page_addr_t *)((fd->curr_page * FLASH_PAGE_SIZE_BYTES) + FLASH_BASE);

		//Copy modified page into internal latch.
		for (page_addr_t page_addr = 0; page_addr < FLASH_PAGE_SIZE_BYTES; page_addr += 4)
		{
			uint32_t data;
			memcpy(&data, &fd->page_buf[page_addr], sizeof(data));
			*addr = data;
			addr++;
		}

		// Send write command to transfer page from latch to internal flash memory.
		flash_at91_sendWRcmd(fd->curr_page);
	}
}

/**
 * Flush At91 flash function.
 *
 * Write current buffered page in flash memory (if modified).
 * This function erase flash memory page before writing.
 */
static int flash_at91_kfileFlush(struct KFile *_fd)
{
	Flash *fd = FLASH_CAST(_fd);
	flash_at91_flush(fd);
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
		flash_at91_flush(fd);
		// Load page
		memcpy(fd->page_buf, (const char *)((page * FLASH_PAGE_SIZE_BYTES) + FLASH_BASE), FLASH_PAGE_SIZE_BYTES);
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

	size = MIN((kfile_off_t)size, (kfile_off_t)(fd->fd.size - (fd->fd.seek_pos - FLASH_BASE)));

	LOG_INFO("Writing at pos[%lu]\n", fd->fd.seek_pos);
	while (size)
	{
		page = (fd->fd.seek_pos - FLASH_BASE) / FLASH_PAGE_SIZE_BYTES;
		page_addr = (fd->fd.seek_pos - FLASH_BASE) % FLASH_PAGE_SIZE_BYTES;

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
 * Close file \a fd
 */
static int flash_at91_close(struct KFile *_fd)
{
	Flash *fd = FLASH_CAST(_fd);
	flash_at91_flush(fd);
	LOG_INFO("Flash file closed\n");

	return 0;
}

/**
 * Open flash file \a fd
 * \a name and \a mode are unused, cause flash memory is
 * threated like one file.
 */
static void flash_at91_open(struct Flash *fd)
{
	fd->fd.size = FLASH_BASE + FLASH_MEM_SIZE;
	fd->fd.seek_pos = FLASH_BASE + FLASH_BOOT_SIZE;
	fd->curr_page = (fd->fd.seek_pos - FLASH_BASE) / FLASH_PAGE_SIZE_BYTES;

	memcpy(fd->page_buf, (const char *)((fd->curr_page * FLASH_PAGE_SIZE_BYTES) + FLASH_BASE), FLASH_PAGE_SIZE_BYTES);

	fd->page_dirty = false;
	LOG_INFO("Flash file opened\n");
}


/**
 * Move \a fd file seek position of \a offset bytes from \a whence.
 *
 */
static kfile_off_t flash_at91_seek(struct KFile *_fd, kfile_off_t offset, KSeekMode whence)
{
	Flash *fd = FLASH_CAST(_fd);
	kfile_off_t seek_pos;

	switch (whence)
	{

	case KSM_SEEK_SET:
		seek_pos = FLASH_BASE + FLASH_BOOT_SIZE;
		break;
	case KSM_SEEK_END:
		seek_pos = fd->fd.size;
		break;
	case KSM_SEEK_CUR:
		seek_pos = fd->fd.seek_pos;
		break;
	default:
		ASSERT(0);
		return EOF;
		break;
	}

	#if LOG_LEVEL >= LOG_LVL_INFO
	/* Bound check */
	if (seek_pos + offset > fd->fd.size)
		LOG_INFO("seek outside EOF\n");
	#endif

	fd->fd.seek_pos = seek_pos + offset;

	return fd->fd.seek_pos - (FLASH_BASE + FLASH_BOOT_SIZE);
}

/**
 * Reopen file \a fd
 */
static struct KFile *flash_at91_reopen(struct KFile *_fd)
{
	Flash *fd = FLASH_CAST(_fd);
	flash_at91_close(_fd);
	flash_at91_open(fd);

	return _fd;
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
	flash_at91_flush(fd);

	uint32_t *addr = (uint32_t *)fd->fd.seek_pos;
	memcpy(buf, (uint8_t *)addr, size);

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
	DB(fd->fd._type = KFT_FLASH);

	// Set up flash programming functions.
	fd->fd.reopen = flash_at91_reopen;
	fd->fd.close = flash_at91_close;
	fd->fd.write = flash_at91_write;
	fd->fd.read = flash_at91_read;
	fd->fd.seek = flash_at91_seek;
	fd->fd.error = flash_at91_getStatus;
	fd->fd.flush = flash_at91_kfileFlush;

	flash_at91_open(fd);

	uint32_t fmcn;
	uint32_t fws = 0;


	/*
	 * Compute values to insert into mode register.
	 */

	/* main clocks in 1.5uS */
	fmcn = (CPU_FREQ/1000000ul) + (CPU_FREQ/2000000ul) + 1;

	/* hard overclocking */
	if (fmcn > 0xFF)
		fmcn = 0xFF;

	/* Only allow fmcn=0 if clock period is > 30 us = 33kHz. */
	if (CPU_FREQ <= 33333ul)
		fmcn = 0;

	/* Only allow fws=0 if clock frequency is < 30 MHz. */
	if (CPU_FREQ > 30000000ul)
	{
		fws = 1;
	}

	// Set wait states and number of MCK cycles in 1.5 usecs
	MC_FMR = fmcn << 16 | fws << 8;

}

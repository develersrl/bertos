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
 * \brief LM3S1968 internal flash memory driver.
 *
 * \author Andrea Righi <arighi@develer.com>
 */

#include "flash_lm3s.h"
#include "cfg/log.h"

#include <cfg/macros.h>

#include <kern/kfile.h>

#include <drv/timer.h>
#include <drv/flash.h>
#include <cpu/power.h> /* cpu_relax() */

#include <string.h> /* memcpy() */


static int flash_lm3s_erase_page(page_t addr)
{
	FLASH_FCMISC_R = FLASH_FCMISC_AMISC;

	FLASH_FMA_R = (volatile uint32_t)addr;
	FLASH_FMC_R = FLASH_FMC_WRKEY | FLASH_FMC_ERASE;

	while (FLASH_FMC_R & FLASH_FMC_ERASE)
		cpu_relax();
	if (FLASH_FCRIS_R & FLASH_FCRIS_ARIS)
		return -1;
	return 0;
}

static int flash_lm3s_write_word(page_t addr, const uint8_t *data, size_t len)
{
	FLASH_FCMISC_R = FLASH_FCMISC_AMISC;

	uint32_t _data;
	memcpy(&_data, data, len);
	FLASH_FMA_R = (volatile uint32_t)addr;
	FLASH_FMD_R = (volatile uint32_t)_data;
	FLASH_FMC_R = FLASH_FMC_WRKEY | FLASH_FMC_WRITE;

	while (FLASH_FMC_R & FLASH_FMC_WRITE)
		cpu_relax();
	if (FLASH_FCRIS_R & FLASH_FCRIS_ARIS)
		return -1;
	return 0;
}

static void _flash_lm3s_flush(Flash *fd)
{
	if (!fd->page_dirty)
		return;

	LOG_INFO("Erase page %p\n", fd->curr_page);
	flash_lm3s_erase_page(fd->curr_page);

	LOG_INFO("Flush page %p\n", fd->curr_page);
	for (int i = 0; i < FLASH_PAGE_SIZE_BYTES; i+=4)
		flash_lm3s_write_word(fd->curr_page + i, &fd->page_buf[i], sizeof(uint32_t));
	fd->page_dirty = false;
}

static void flash_lm3s_load_page(Flash *fd, page_t page)
{
	ASSERT(!((size_t)page % FLASH_PAGE_SIZE_BYTES));

	if (page == fd->curr_page)
		return;

	/* Flush old page */
	_flash_lm3s_flush(fd);

	/* Load a new page */
	memcpy(fd->page_buf, FLASH_BASE + (uint8_t *)page, FLASH_PAGE_SIZE_BYTES);
	fd->curr_page = page;
	LOG_INFO("Loaded page %p\n", fd->curr_page);
}

/**
 * Write program memory.
 * Write \a size bytes from buffer \a _buf to file \a fd
 * \note Write operations are not buffered.
 */
static size_t flash_lm3s_write(struct KFile *_fd, const void *_buf, size_t size)
{
	Flash *fd = FLASH_CAST(_fd);
	const uint8_t *buf =(const uint8_t *)_buf;
	size_t total_write = 0;
	size_t len;

	size = MIN((kfile_off_t)size,
		(kfile_off_t)(fd->fd.size - (fd->fd.seek_pos - FLASH_BASE)));

	LOG_INFO("Writing at pos[%lx]\n", fd->fd.seek_pos);
	while (size)
	{
		page_t page = (fd->fd.seek_pos & ~(FLASH_PAGE_SIZE_BYTES - 1));
		size_t offset = fd->fd.seek_pos % FLASH_PAGE_SIZE_BYTES;

		flash_lm3s_load_page(fd, page);

		len = MIN(size, FLASH_PAGE_SIZE_BYTES - offset);

		memcpy((uint8_t *)fd->page_buf + offset, buf, len);
		fd->page_dirty = true;

		buf += len;
		fd->fd.seek_pos += len;
		size -= len;
		total_write += len;
	}
	LOG_INFO("written %u bytes\n", total_write);
	return total_write;
}

/**
 * Close file \a fd
 */
static int flash_lm3s_close(struct KFile *_fd)
{
	Flash *fd = FLASH_CAST(_fd);
	_flash_lm3s_flush(fd);
	LOG_INFO("Flash file closed\n");
	return 0;
}

/**
 * Open flash file \a fd
 */
static void flash_lm3s_open(Flash *fd)
{
	fd->fd.size = FLASH_BASE + FLASH_MEM_SIZE;
	fd->fd.seek_pos = FLASH_BASE;
	/*
	 * Set an invalid page to force the load of the next actually used page
	 * in cache.
	 */
	fd->curr_page = FLASH_BASE + FLASH_MEM_SIZE;

	fd->page_dirty = false;
	LOG_INFO("Flash file opened\n");
}

/**
 * Move \a fd file seek position of \a offset bytes from \a whence.
 */
static kfile_off_t flash_lm3s_seek(struct KFile *_fd, kfile_off_t offset, KSeekMode whence)
{
	Flash *fd = FLASH_CAST(_fd);
	kfile_off_t seek_pos;

	switch (whence)
	{
	case KSM_SEEK_SET:
		seek_pos = FLASH_BASE;
		break;
	case KSM_SEEK_END:
		seek_pos = FLASH_BASE + fd->fd.size;
		break;
	case KSM_SEEK_CUR:
		seek_pos = fd->fd.seek_pos;
		break;
	default:
		ASSERT(0);
		return EOF;
		break;
	}
	if (seek_pos + offset > fd->fd.size)
		LOG_ERR("seek outside EOF\n");
	fd->fd.seek_pos = seek_pos + offset;

	return fd->fd.seek_pos - FLASH_BASE;
}

/**
 * Reopen file \a fd
 */
static struct KFile *flash_lm3s_reopen(struct KFile *_fd)
{
	Flash *fd = FLASH_CAST(_fd);
	flash_lm3s_close(_fd);
	flash_lm3s_open(fd);

	return _fd;
}

/**
 * Read from file \a fd \a size bytes and put it in buffer \a buf
 * \return the number of bytes read.
 */
static size_t flash_lm3s_read(struct KFile *_fd, void *_buf, size_t size)
{
	Flash *fd = FLASH_CAST(_fd);
	uint8_t *buf =(uint8_t *)_buf, *addr;

	size = MIN((kfile_off_t)size, fd->fd.size - fd->fd.seek_pos);

	LOG_INFO("Reading at pos[%lx]\n", fd->fd.seek_pos);
	/* Check if we can get current cached page */
	if ((size_t)fd->fd.seek_pos / FLASH_PAGE_SIZE_BYTES ==
				(size_t)fd->curr_page)
		addr = (uint8_t *)fd->curr_page +
			fd->fd.seek_pos % FLASH_PAGE_SIZE_BYTES;
	else
		addr = (uint8_t *)fd->fd.seek_pos;
	memcpy(buf, (uint8_t *)addr, size);
	fd->fd.seek_pos += size;

	LOG_INFO("Read %u bytes\n", size);
	return size;
}

static int flash_lm3s_flush(struct KFile *_fd)
{
	Flash *fd = FLASH_CAST(_fd);

	_flash_lm3s_flush(fd);
	return 0;
}

/**
 * Init module to perform write and read operation on internal
 * flash memory.
 */
void flash_hw_init(Flash *fd)
{
	memset(fd, 0, sizeof(*fd));
	DB(fd->fd._type = KFT_FLASH);

	fd->fd.reopen = flash_lm3s_reopen;
	fd->fd.close = flash_lm3s_close;
	fd->fd.write = flash_lm3s_write;
	fd->fd.read = flash_lm3s_read;
	fd->fd.seek = flash_lm3s_seek;
	fd->fd.flush = flash_lm3s_flush;

	FLASH_USECRL_R = CPU_FREQ / 1000000 - 1;

	flash_lm3s_open(fd);
}

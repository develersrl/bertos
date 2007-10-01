/**
 * \file
 * <!--
 * Copyright 2007 Develer S.r.l. (http://www.develer.com/)
 * All Rights Reserved.
 * -->
 *
 * \brief Self programming routines
 *
 * \version $Id$
 * \author Francesco Sacchi <batt@develer.com>
 * \author Daniele Basile <asterix@develer.com>
 */

#include "prog.h"

#include <string.h>

#include <drv/wdt.h>
#include <cfg/macros.h> // MIN()
#include <cfg/compiler.h>
#include <cfg/debug.h>

#include <avr/io.h>
#include <avr/boot.h>
#include <avr/pgmspace.h>

typedef uint16_t avr_page_addr_t;
typedef uint16_t avr_page_t;

/**
 * Temporary buffer cointaing data block to
 * write on flash.
 */
static uint8_t page_buf[SPM_PAGESIZE];

bool page_modified; /// Flag for checking if current page is modified.

/**
 * Current buffered page.
 */
static avr_page_t curr_page = 0;

/**
 * Write current buffered page in flash memory (if modified).
 * This function erase flash memory page before writing.
 */
static void prog_flush(void)
{
	if (page_modified)
	{
		kprintf("Flushing page %d\n", curr_page);

		boot_spm_busy_wait();  // Wait while the SPM instruction is busy.

		kprintf("Filling temparary page buffer...");
		/* Fill the temporary buffer of the AVR */
		for (avr_page_addr_t page_addr = 0; page_addr < SPM_PAGESIZE; page_addr += 2)
		{
			uint16_t word = ((uint16_t)page_buf[page_addr + 1] << 8) | page_buf[page_addr];

			ATOMIC(boot_page_fill(page_addr, word));
		}
		kprintf("Done.\n");

		wdt_reset();

		kprintf("Erasing page, addr %d...", curr_page * SPM_PAGESIZE);

		/* Page erase */
		ATOMIC(boot_page_erase(curr_page * SPM_PAGESIZE));

		/* Wait until the memory is erased. */
		boot_spm_busy_wait();

		kprintf("Done.\n");
		kprintf("Writing page, addr %d...", curr_page * SPM_PAGESIZE);

		/* Store buffer in flash page. */
		ATOMIC(boot_page_write(curr_page * SPM_PAGESIZE));
		boot_spm_busy_wait();  // Wait while the SPM instruction is busy.

		/*
		* Reenable RWW-section again. We need this if we want to jump back
		* to the application after bootloading.
		*/
		ATOMIC(boot_rww_enable());

		page_modified = false;
		kprintf("Done.\n");
	}
}


/**
 * Check current page and if \a page is different, load it in
 * temporary buffer.
 */
static void prog_loadPage(avr_page_t page)
{
	if (page != curr_page)
	{
		prog_flush();
		// Load page
		memcpy_P(page_buf, (const char *)(page * SPM_PAGESIZE), SPM_PAGESIZE);
		curr_page = page;
		kprintf("Loaded page %d\n", curr_page);
	}
}

/**
 * Write program memory.
 * Write \a size bytes from buffer \a _buf to file \a *fd
 * \note Write operations are buffered.
 */
size_t prog_write(struct _KFile *fd, const void *_buf, size_t size)
{
	const uint8_t *buf =(const uint8_t *)_buf;

	avr_page_t page;
	avr_page_addr_t page_addr;
	size_t total_write = 0;

	ASSERT(fd->seek_pos + size <= fd->size);
	size = MIN((uint32_t)size, fd->size - fd->seek_pos);

	kprintf("Writing at pos[%d]\n", fd->seek_pos);
	while (size)
	{
		page = fd->seek_pos / SPM_PAGESIZE;
		page_addr = fd->seek_pos % SPM_PAGESIZE;

		prog_loadPage(page);

		size_t wr_len = MIN(size, SPM_PAGESIZE - page_addr);
		memcpy(page_buf + page_addr, buf, wr_len);
		page_modified = true;

		buf += wr_len;
		fd->seek_pos += wr_len;
		size -= wr_len;
		total_write += wr_len;
	}
	kprintf("written %d bytes\n", total_write);
	return total_write;
}

/**
 * Open flash file \a *fd
 * \a name and \a mode are unused, cause flash memory is
 * threated like one file.
 */
bool prog_open(struct _KFile *fd, UNUSED_ARG(const char *, name), UNUSED_ARG(int, mode))
{
	curr_page = 0;
	memcpy_P(page_buf, (const char *)(curr_page * SPM_PAGESIZE), SPM_PAGESIZE);

	fd->seek_pos = 0;
	fd->size = (uint16_t)(FLASHEND - CONFIG_BOOT_SIZE + 1);
	page_modified = false;

	kprintf("Flash file opened\n");
	return true;
}

/**
 * Close file \a *fd
 */
bool prog_close(UNUSED_ARG(struct _KFile *,fd))
{
	prog_flush();
	kprintf("Flash file closed\n");
	return true;
}

/**
 * Move \a *fd file seek position of \a offset bytes
 * from current position.
 */
bool prog_seek(struct _KFile *fd, int32_t offset)
{
	ASSERT(fd->seek_pos + offset <= fd->size);

	/* Bound check */
	if (fd->seek_pos + offset > fd->size)
		return false;

	fd->seek_pos += offset;
	kprintf("Flash seek to [%d]\n", fd->seek_pos);

	return true;
}

/**
 * Read from file \a *fd \a size bytes and put it in buffer \a *buf
 * \return the number of bytes read.
 */
size_t prog_read(struct _KFile *fd, void *buf, size_t size)
{
	ASSERT(fd->seek_pos + size <= fd->size);
	size = MIN((uint32_t)size, fd->size - fd->seek_pos);

	kprintf("Reading at pos[%d]\n", fd->seek_pos);
	// Flush current buffered page (if modified).
	prog_flush();

	/*
	 * AVR pointers are 16 bits wide, this hack is needed to avoid
	 * compiler warning, cause fd->seek_pos is a 32bit offset.
	 */
	const uint8_t *pgm_addr = (const uint8_t *)0;
	pgm_addr += fd->seek_pos;

	memcpy_P(buf, pgm_addr, size);
	fd->seek_pos += size;
	kprintf("Read %d bytes\n", size);
	return size;
}


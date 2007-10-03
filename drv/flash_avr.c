/**
 * \file
 * <!--
 * Copyright 2007 Develer S.r.l. (http://www.develer.com/)
 * This file is part of DevLib - See README.devlib for information.
 * -->
 *
 * \brief Self programming routines.
 *
 * \version $Id$
 * \author Francesco Sacchi <batt@develer.com>
 * \author Daniele Basile <asterix@develer.com>
 *
 * This module implements a kfile-like access for Atmel avr
 * internal flash.
 * Internal flash writing access is controlled by BOOTSZ fuses, check
 * datasheet for details.
 */

#include "flash_avr.h"

#include <avr/io.h>
#include <avr/boot.h>
#include <avr/pgmspace.h>

#include <cfg/macros.h> // MIN()
#include <cfg/compiler.h>
#include <cfg/debug.h>
#include <cfg/cpu.h>

#include <drv/wdt.h>

#include <string.h>
#include <stdio.h>

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
static void flash_avr_flush(void)
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

		kprintf("Erasing page, addr %u...", curr_page * SPM_PAGESIZE);

		/* Page erase */
		ATOMIC(boot_page_erase(curr_page * SPM_PAGESIZE));

		/* Wait until the memory is erased. */
		boot_spm_busy_wait();

		kprintf("Done.\n");
		kprintf("Writing page, addr %u...", curr_page * SPM_PAGESIZE);

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
static void flash_avr_loadPage(avr_page_t page)
{
	if (page != curr_page)
	{
		flash_avr_flush();
		// Load page
		memcpy_P(page_buf, (const char *)(page * SPM_PAGESIZE), SPM_PAGESIZE);
		curr_page = page;
		kprintf("Loaded page %d\n", curr_page);
	}
}

/**
 * Write program memory.
 * Write \a size bytes from buffer \a _buf to file \a fd
 * \note Write operations are buffered.
 */
static size_t flash_avr_write(struct _KFile *fd, const void *_buf, size_t size)
{
	const uint8_t *buf =(const uint8_t *)_buf;

	avr_page_t page;
	avr_page_addr_t page_addr;
	size_t total_write = 0;

	ASSERT(fd->seek_pos + size <= fd->size);
	size = MIN((uint32_t)size, fd->size - fd->seek_pos);

	kprintf("Writing at pos[%u]\n", fd->seek_pos);
	while (size)
	{
		page = fd->seek_pos / SPM_PAGESIZE;
		page_addr = fd->seek_pos % SPM_PAGESIZE;

		flash_avr_loadPage(page);

		size_t wr_len = MIN(size, SPM_PAGESIZE - page_addr);
		memcpy(page_buf + page_addr, buf, wr_len);
		page_modified = true;

		buf += wr_len;
		fd->seek_pos += wr_len;
		size -= wr_len;
		total_write += wr_len;
	}
	kprintf("written %u bytes\n", total_write);
	return total_write;
}

/**
 * Open flash file \a fd
 * \a name and \a mode are unused, cause flash memory is
 * threated like one file.
 */
static bool flash_avr_open(struct _KFile *fd, UNUSED_ARG(const char *, name), UNUSED_ARG(int, mode))
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
 * Close file \a fd
 */
static bool flash_avr_close(UNUSED_ARG(struct _KFile *,fd))
{
	flash_avr_flush();
	kprintf("Flash file closed\n");
	return true;
}

/**
 * Move \a fd file seek position of \a offset bytes
 * from current position.
 */
static int32_t flash_avr_seek(struct _KFile *fd, int32_t offset, int whence)
{
	uint32_t seek_pos;

	switch(whence)
	{
		case SEEK_SET:
			seek_pos = 0;
			break;
		case SEEK_END:
			seek_pos = fd->size - 1;
			break;
		case SEEK_CUR:
			seek_pos = fd->seek_pos;
			break;
		default:
			ASSERT(0);
			return -1;
			break;
	}
	
	/* Bound check */
	if (seek_pos + offset > fd->size)
	{
		ASSERT(0);
		return -1;
	}

	fd->seek_pos = seek_pos + offset;
	kprintf("Flash seek to [%u]\n", fd->seek_pos);

	return fd->seek_pos;
}

/**
 * Read from file \a fd \a size bytes and put it in buffer \a buf
 * \return the number of bytes read.
 */
static size_t flash_avr_read(struct _KFile *fd, void *buf, size_t size)
{
	ASSERT(fd->seek_pos + size <= fd->size);
	size = MIN((uint32_t)size, fd->size - fd->seek_pos);

	kprintf("Reading at pos[%u]\n", fd->seek_pos);
	// Flush current buffered page (if modified).
	flash_avr_flush();

	/*
	 * AVR pointers are 16 bits wide, this hack is needed to avoid
	 * compiler warning, cause fd->seek_pos is a 32bit offset.
	 */
	const uint8_t *pgm_addr = (const uint8_t *)0;
	pgm_addr += fd->seek_pos;

	memcpy_P(buf, pgm_addr, size);
	fd->seek_pos += size;
	kprintf("Read %u bytes\n", size);
	return size;
}

/**
 * Init AVR flash read/write file.
 */
void flash_avr_init(struct _KFile *fd)
{
	// Set up flash programming functions.
	fd->open = flash_avr_open;
	fd->close = flash_avr_close;
	fd->read = flash_avr_read;
	fd->write = flash_avr_write;
	fd->seek = flash_avr_seek;
}

#if CONFIG_TEST

#define TEST_SIZE 683
#define ONE_BYTE_TEST_ADDRESS 347

uint8_t test_buf[TEST_SIZE];
uint8_t save_buf[TEST_SIZE];

/**
 * Program memory read/write subtest.
 * Try to write/read in the same \a f file location \a _size bytes.
 * \return true if all is ok, false otherwise
 * \note Restore file position at exit (if no error)
 * \note Test buffer \a buf must be filled with
 * the following statement:
 * <pre>
 * buf[i] = i & 0xff
 * </pre>
 */
static bool flash_avr_rwTest(KFile *f, uint8_t *buf, size_t _size)
{
	int32_t size = _size;
	// Write test buffer
	if (f->write(f, buf, size) != size)
		return false;
	f->seek(f, -size, SEEK_CUR);

	// Reset test buffer
	memset(buf, 0, size);

	// Read flash in test buffer
	if (f->read(f, buf, size) != size)
		return false;
	f->seek(f, -size, SEEK_CUR);

	// Check test result
 	for (size_t i = 0; i < size; i++)
 		if (buf[i] != (i & 0xff))
			return false;

	return true;
}

/**
 * Test for program memory read/write.
 */
bool flash_avr_test(void)
{
	KFile fd;

	// Set up flash programming functions.
	flash_avr_init(&fd);

	// Fill in test buffer
	for (int i = 0; i < TEST_SIZE; i++)
		test_buf[i] = (i & 0xff);

	// Open flash
	fd.open(&fd, NULL, 0);
	// Save flash content (for later restore).
	fd.read(&fd, save_buf, sizeof(save_buf));
	// Seek to addr 0
	if (fd.seek(&fd, 0, SEEK_SET) != 0)
		goto flash_avr_test_end;

	// Test flash read/write to address 0..TEST_SIZE
	if (!flash_avr_rwTest(&fd, test_buf, TEST_SIZE))
		goto flash_avr_test_end;

	// One byte read/write test
	fd.seek(&fd, ONE_BYTE_TEST_ADDRESS, SEEK_CUR); // Random address
	if (!flash_avr_rwTest(&fd, test_buf, 1))
		goto flash_avr_test_end;
	fd.seek(&fd, -(int32_t)ONE_BYTE_TEST_ADDRESS, SEEK_CUR);

	// Restore old flash data
	if (fd.write(&fd, save_buf, sizeof(test_buf)) != TEST_SIZE)
		goto flash_avr_test_end;
	fd.seek(&fd, -TEST_SIZE, SEEK_CUR);

	// Go to the Flash end
	fd.seek(&fd, -TEST_SIZE, SEEK_END);
	// Save flash content (for later restore).
	fd.read(&fd, save_buf, sizeof(save_buf));
	fd.seek(&fd, -TEST_SIZE, SEEK_CUR);

	// Test flash read/write to address (FLASHEND - TEST_SIZE) ... FLASHEND
	if (!flash_avr_rwTest(&fd, test_buf, TEST_SIZE))
		goto flash_avr_test_end;

	// Go to half test size.
	fd.seek(&fd, (TEST_SIZE / 2), SEEK_CUR);

	// This test should FAIL, cause we try to write over file end.
	kprintf("This test should fail.\n");
	if (flash_avr_rwTest(&fd, test_buf, TEST_SIZE))
		goto flash_avr_test_end;

	fd.seek(&fd, -TEST_SIZE, SEEK_CUR);
	// Restore old flash data
	fd.write(&fd, save_buf, TEST_SIZE);

	fd.close(&fd);
	return true;

flash_avr_test_end:
	fd.close(&fd);
	return false;
}

#endif

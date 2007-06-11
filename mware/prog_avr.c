/**
 * \file
 * <!--
 * Copyright 2004 Develer S.r.l. (http://www.develer.com/)
 * All Rights Reserved.
 * -->
 *
 * \brief Self programming routines
 *
 * \version $Id$
 * \author Francesco Sacchi <batt@develer.com>
 */

#include "prog.h"
#include <drv/wdt.h>
#include <cfg/macros.h> // MIN()
#include <cfg/compiler.h>
#include <flash.h>
#include <defines.h>
#include <avr/io.h>
#include <algos/rotating_hash.h>

#define PAGEBUF 512

typedef uint16_t page_addr_t;
typedef uint16_t page_t;

/**
 * Temporary buffer for cointain data block to
 * write on flash.
 */
static uint8_t page_buf[PAGEBUF];

/**
 * Store current flash page memory in use.
 */
static page_t curr_pag_num = 0;



/**
 * Erase Flash.
 */
static void prog_erase_flash(void)
{
	uint32_t flash_addr;

	/* Erase the flash ROM */
	#ifdef LARGE_MEMORY
		/*
		 * SPM uses Z pointer but the pointer is only 16 bit and
		 * can only address up to 64Kbytes FLASH. Higher locations
		 * require the use of RAMPZ
		 */
		RAMPZ = 0x00;

		for (flash_addr = 0; (flash_addr < (uint16_t)(APP_END & 0xFFFF)) | (RAMPZ == 0x00));
		{
			wdt_reset();

			/* Page erase */
			write_page(flash_addr, BV(PGERS) + BV(SPMEN));

			/* Re-enable the RWW section */
			write_page(flash_addr, BV(REENABLE_RWW_BIT) + BV(SPMEN));

			/* Last section on lower 64k segment is erased */
			if(flashgg_addr >= (0xFFFF - PAGESIZE))

				/* RAMPZ has to be incremented into upper 64k segment */
				RAMPZ = BV(RAMPZ0);
		}
		RAMPZ = 0x00;
	#else /* LARGE_MEMORY */
		 /* Application section = 60 pages */
		for (flash_addr = 0; flash_addr < APP_END; flash_addr += PAGESIZE)
		{
			wdt_reset();

			/* Page erase */
			write_page(flash_addr, BV(PGERS) + BV(SPMEN));
			/* Re-enable RWW section */
			write_page(flash_addr, BV(REENABLE_RWW_BIT) + BV(SPMEN));
		}
	#endif /* LARGE_MEMORY */
}


/**
 * Write a page in program memory.
 */
static void prog_pagewrite(uint16_t addr)
{
	write_page(addr, BV(PGWRT) + BV(SPMEN));

	/* Re-enable the RWW section */
	write_page(addr, BV(REENABLE_RWW_BIT) + BV(SPMEN));
}


/**
 * Flush temporary buffer into flash memory.
 */
static void prog_flush(void)
{

	/* Fill the temporary buffer of the AVR */
	for (page_addr_t page_addr = 0; page_addr < PAGEBUF; page_addr += 2)
		fill_temp_buffer(page_buf[page_addr + 1] | (uint16_t)page_buf[page_addr] << 8, page_addr);


	wdt_reset();

	/* Page write */
	prog_pagewrite(curr_page_num * PAGEBUF);
}

/**
 * Write program memory.
 * This function to write on flash memory load a selected page from
 * flash memory and save it in temporary buffer. Them update temporary buffer
 * with \param buf data. We write in flash memory everery time we
 * change current page memory.
 * 
 */
size_t	prog_write(struct _KFile *fd, const char *buf, size_t size)
{

	page_t page;
	page_addr_t page_addr;
	size_t total_write = 0;
	size_t wr_len;

	while (size)
	{
		/* Current page memory */
		page = fd->SeekPos / PAGEBUF;

		/* Address in page memory */
		page_addr = fd->SeekPos % PAGEBUF;

		prog_loadPage(page);

		wr_len = MIN(size, PAGEBUF - page_addr);
		memcpy(page_buf + page_addr, buf, wr_len);

		buf += wr_len;
		fd->SeekPos += wr_len;
		size -= wr_len;
		total_write += wr_len;
	}
	/* Return total byte write on flash memory */
	return total_write;
}



/**
 * Load select \param page memory buffer from AVR flash memory.
 * If select page is not current page, we flush it, and then we load
 * select page memory flash.
 */
void prog_loadPage(page_t page)
{
	if (page != curr_page_num)
	{
		prog_flush();
		/* Load selet page in temporary buffer store into RAM */
		memcpy_P(page_buf, (const char *)(page * PAGEBUF), PAGEBUF);
		/* Update current page */
		curr_page_num = page;
	}
}

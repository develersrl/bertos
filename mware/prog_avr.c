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

		for (flash_addr = 0; (flash_addr < (uint16_t)(APP_END & 0xFFFF)) | (RAMPZ == 0x00); flash_addr += PAGESIZE)
		{
			wdt_reset();

			write_page(flash_addr, BV(PGERS) + BV(SPMEN));             /* Page erase */
			write_page(flash_addr, BV(REENABLE_RWW_BIT) + BV(SPMEN));  /* Re-enable the RWW section */

			if(flashgg_addr >= (0xFFFF - PAGESIZE))  /* Last section on lower 64k segment is erased */
				RAMPZ = BV(RAMPZ0);          /* RAMPZ has to be incremented into upper 64k segment */
		}
		RAMPZ = 0x00;
	#else /* LARGE_MEMORY */
		for (flash_addr = 0; flash_addr < APP_END; flash_addr += PAGESIZE)  /* Application section = 60 pages */
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
 * Write program memory.
 */
rotating_t prog_write(struct _KFile *file, progress_func_t progress)
{
	size_t size;
	rotating_t rot = 0;
	uint32_t flash_addr = 0;
	uint16_t page_addr;
	uint8_t buf[PAGESIZE];
	
	/* We erase fash memory before to write inside */
	prog_erase_flash();

	for (;;)
	{
		wdt_reset();

		/* Read data from file */
		size = file->read(file, buf, PAGESIZE);

		/* If we reached end of file exit */
		if (!size)
			break;
	
		/* Update checksum */
		rotating_update(buf, size, &rot);
	
		/* Fill the temporary buffer of the AVR */
		for (page_addr = 0; page_addr < size; page_addr += 2)
			fill_temp_buffer(buf[page_addr + 1] | (uint16_t)buf[page_addr] << 8, page_addr);
		
		/* Page write */
		prog_pagewrite(flash_addr);
	
		/* Update progess (if present) */
		if (progress)
			if (!progress(file->SeekPos, file->Size))
				break;
		
		flash_addr += size;
	}

	return rot
}

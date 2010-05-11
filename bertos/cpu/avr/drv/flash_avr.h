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
 * \author Francesco Sacchi <batt@develer.com>
 * \author Daniele Basile <asterix@develer.com>
 *
 * \brief AVR Internal flash read/write driver.
 *
 *
 */

#ifndef FLASH_AT91_H
#define FLASH_AT91_H

#include <cpu/types.h>

#include <cfg/compiler.h>

#include <kern/kfile.h>

#include <avr/io.h>


#define FLASH_PAGE_SIZE SPM_PAGESIZE

/**
 * Definition of type for avr flash module.
 */
typedef uint16_t page_t;

/* Forward declaration */
struct Flash;

/**
 * FlashAvr KFile context structure.
 * DEPREACTED STRUCTURE!
 * Use Flash instead
 *
 * \{
 */
typedef struct FlashAvr
{
	/**
	 * File descriptor.
	 */
	KFile fd;

	/**
	 * Flag for checking if current page is modified.
	 */
	bool page_dirty;

	/**
	 * Current buffered page.
	 */
	page_t curr_page;

	/**
	 * Temporary buffer cointaing data block to
	 * write on flash.
	 */
	uint8_t page_buf[SPM_PAGESIZE];
} FlashAvr;
/* \} */

void flash_hw_init(struct Flash *fd);

/**
 * WARNING!
 * This function is DEPRECADED!
 * use the flash module instead.
 */
INLINE void flash_avr_init(struct FlashAvr *fd)
{
	flash_hw_init((struct Flash *)fd);
}

#endif /* DRV_FLASH_AVR_H */

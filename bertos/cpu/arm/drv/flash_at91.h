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

#ifndef FLASH_AT91_H
#define FLASH_AT91_H

#include <cpu/types.h>

#include <kern/kfile.h>

#include <io/arm.h>


/**
 * Define data type to manage page and memory address.
 */
typedef uint32_t arm_page_t;
typedef uint32_t arm_page_addr_t;

/**
 * FlashAt91 KFile context structure.
 */
typedef struct FlashAt91
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
	arm_page_t curr_page;

	/**
	 * Temporary buffer cointaing data block to
	 * write on flash.
	 */
	uint8_t page_buf[FLASH_PAGE_SIZE_BYTES];


} FlashAt91;

/**
 * ID for FlashAt91
 */
#define KFT_FLASHAT91 MAKE_ID('F', 'A', '9', '1')

/**
 * Convert + ASSERT from generic KFile to FlashAt91.
 */
INLINE FlashAt91 * FLASHAT91_CAST(KFile *fd)
{
	ASSERT(fd->_type == KFT_FLASHAT91);
	return (FlashAt91 *)fd;
}


void flash_at91_init(FlashAt91 *fd);

#endif

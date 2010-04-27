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

#ifndef FLASH_LM3S_H
#define FLASH_LM3S_H

#include <cpu/types.h>
#include <kern/kfile.h>

/* Flash memory mapping */
#define FLASH_MEM_SIZE		0x40000 //< 256KiB
#define FLASH_PAGE_SIZE_BYTES	0x400   //< 1KiB

/**
 * FlashLM3S KFile context structure.
 */
typedef struct FlashLM3S
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
	uint32_t *curr_page;

	/**
	 * Temporary buffer cointaing data block to
	 * write on flash.
	 */
	uint32_t page_buf[FLASH_PAGE_SIZE_BYTES / sizeof(uint32_t)];
} FlashLM3S;

/**
 * ID for FlashLM3S
 */
#define KFT_FLASHLM3S MAKE_ID('F', 'L', '3', 'S')

/**
 * Convert + ASSERT from generic KFile to FlashLM3S structure.
 */
INLINE FlashLM3S * FLASHLM3S_CAST(KFile *fd)
{
	ASSERT(fd->_type == KFT_FLASHLM3S);
	return (FlashLM3S *)fd;
}

void flash_lm3sInit(FlashLM3S *fd);

#endif /* FLASH_LM3S_H */

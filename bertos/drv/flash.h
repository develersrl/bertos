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
* Copyright 2005 Develer S.r.l. (http://www.develer.com/)
* -->
*
*
* \brief Embedded flash for cpu.
*
* \author Francesco Sacchi <batt@develer.com>
* \author Daniele Basile <asterix@develer.com>
*
* $WIZ$ module_name = "flash"
* $WIZ$ module_depends = "kfile"
*/

#ifndef DRV_FLASH_H
#define DRV_FLASH_H

#include <cfg/macros.h>
#include <cfg/module.h>


#include CPU_HEADER(flash)

/**
* EmbFlash KFile context structure.
*/
typedef struct Flash
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
	uint8_t page_buf[FLASH_PAGE_SIZE];
} Flash;

/**
* ID for FLASH
*/
#define KFT_FLASH MAKE_ID('F', 'L', 'A', 'S')

/**
* Convert + ASSERT from generic KFile to Flash.
*/
INLINE Flash * FLASH_CAST(KFile *fd)
{
	ASSERT(fd->_type == KFT_FLASH);
	return (Flash *)fd;
}


MOD_DEFINE(flash);

/**
 *
 * Initialize PWM hw.
 */
INLINE void flash_init(Flash *fd)
{
	flash_hw_init(fd);

	MOD_INIT(flash);
}



#endif /* DRV_FLASH_H */


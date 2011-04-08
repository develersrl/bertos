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
* Copyright 2011 Develer S.r.l. (http://www.develer.com/)
* -->
*
* \brief Micron MT29F serial NAND driver
*
* This module allows read/write access to Micron MT29F serial
* NANDs.
* It is a block device, so it must be accessed using the KBlock
* interface functions (see kblock.h).
*
* Once you have opened the flash for writing, you may want to use
* kblock_trim() to avoid overwriting data on other flash banks.
*
* Example usage:
* \code
* Flash fls;
* flash_init(&fls, 0);
* // enable access only on desired blocks
* // start block = 50, num blocks = 20
* kblock_trim(&fls.blk, 50, 20);
* // ...
* // now write to the flash
* // block number is automatically converted
* kblock_write(&fls.blk, 0, buf, 0, 128);
* \endcode
*
* \author Stefano Fedrigo <aleph@develer.com>
*
* $WIZ$ module_name = "mt29f"
* $WIZ$ module_depends = "kfile", "kfile_block", "kblock"
* $WIZ$ module_configuration = "bertos/cfg/cfg_mt29f.h"
*/

#ifndef DRV_MT29F_H
#define DRV_MT29F_H

#include "cfg/cfg_mt29f.h"

#include <cfg/macros.h>
#include <cfg/compiler.h>

#include <io/kblock.h>
#include <io/kfile.h>
#include <io/kfile_block.h>

#include <cpu/attr.h>


struct Mt29fHardware;

/**
 * MT29F KBlock context structure.
 */
typedef struct Mt29f
{
	KBlock blk;                  ///< KBlock context
	struct Mt29fHardware *hw;
} Mt29f;

/*
 * Kblock type ID.
 */
#define KBT_MT29F MAKE_ID('M', 'T', '2', '9')

/**
 * Convert + ASSERT from generic KBlock to Flash.
 */
INLINE Mt29f *FLASH_CAST(KBlock *fls)
{
	ASSERT(fls->priv.type == KBT_MT29F);
	return (Mt29f *)fls;
}

void mt29f_hw_init(Mt29f *fls);
void mt29f_hw_initUnbuffered(Mt29f *fls);

#endif /* DRV_MT29F_H */

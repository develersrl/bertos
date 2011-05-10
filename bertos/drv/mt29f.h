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
* \author Stefano Fedrigo <aleph@develer.com>
*
* $WIZ$ module_name = "mt29f"
* $WIZ$ module_depends = "timer", "kblock", "heap"
* $WIZ$ module_configuration = "bertos/cfg/cfg_mt29f.h"
*/

#ifndef DRV_MT29F_H
#define DRV_MT29F_H

#include "cfg/cfg_mt29f.h"
#include <cfg/macros.h>
#include <io/kblock.h>


// Define log settings for cfg/log.h
#define LOG_LEVEL    CONFIG_MT29F_LOG_LEVEL
#define LOG_FORMAT   CONFIG_MT29F_LOG_FORMAT

/**
 * \name Error codes.
 * \{
 */
#define MT29F_ERR_ERASE     BV(1)   ///< Error erasing a block
#define MT29F_ERR_WRITE     BV(2)   ///< Error writing a page
#define MT29F_ERR_RD_TMOUT  BV(3)   ///< Read timeout
#define MT29F_ERR_WR_TMOUT  BV(4)   ///< Write timeout
#define MT29F_ERR_ECC       BV(5)   ///< Unrecoverable ECC error
/** \} */

#define MT29F_PAGE_SIZE         (CONFIG_MT29F_DATA_SIZE + CONFIG_MT29F_SPARE_SIZE)
#define MT29F_BLOCK_SIZE        (CONFIG_MT29F_DATA_SIZE * CONFIG_MT29F_PAGES_PER_BLOCK)

// Number of usable blocks, and index of first remapping block
#define MT29F_NUM_USER_BLOCKS   (CONFIG_MT29F_NUM_BLOCK - CONFIG_MT29F_NUM_REMAP_BLOCKS)


// NAND commands
#define MT29F_CMD_READ_1               0x00
#define MT29F_CMD_READ_2               0x30
#define MT29F_CMD_COPYBACK_READ_1      0x00
#define MT29F_CMD_COPYBACK_READ_2      0x35
#define MT29F_CMD_COPYBACK_PROGRAM_1   0x85
#define MT29F_CMD_COPYBACK_PROGRAM_2   0x10
#define MT29F_CMD_RANDOM_OUT           0x05
#define MT29F_CMD_RANDOM_OUT_2         0xE0
#define MT29F_CMD_RANDOM_IN            0x85
#define MT29F_CMD_READID               0x90
#define MT29F_CMD_WRITE_1              0x80
#define MT29F_CMD_WRITE_2              0x10
#define MT29F_CMD_ERASE_1              0x60
#define MT29F_CMD_ERASE_2              0xD0
#define MT29F_CMD_STATUS               0x70
#define MT29F_CMD_RESET                0xFF


// Get block from page
#define PAGE(blk)            ((blk) * CONFIG_MT29F_PAGES_PER_BLOCK)

// Page from block and page in block
#define BLOCK(page)          ((uint16_t)((page) / CONFIG_MT29F_PAGES_PER_BLOCK))
#define PAGE_IN_BLOCK(page)  ((uint16_t)((page) % CONFIG_MT29F_PAGES_PER_BLOCK))


/**
 * MT29F context.
 */
typedef struct Mt29f
{
	KBlock    fd;           // KBlock descriptor

	uint8_t   chip_select;  // Chip select where NAND is connected
	uint8_t   status;       // Status bitmap

	uint16_t *block_map;    // For bad blocks remapping
	uint16_t  remap_start;  // First unused remap block
} Mt29f;

/*
 * Kblock id.
 */
#define KBT_NAND  MAKE_ID('N', 'A', 'N', 'D')

/**
* Convert + ASSERT from generic KBlock to NAND context.
*/
INLINE Mt29f *MT29F_CAST(KBlock *kb)
{
	ASSERT(kb->priv.type == KBT_NAND);
	return (Mt29f *)kb;
}

struct Heap;

// Kblock interface
bool mt29f_init(Mt29f *chip, struct Heap *heap, unsigned chip_select);
bool mt29f_initUnbuffered(Mt29f *chip, struct Heap *heap, unsigned chip_select);

// NAND specific functions
bool mt29f_getDevId(Mt29f *chip, uint8_t dev_id[5]);
int mt29f_blockErase(Mt29f *chip, uint16_t block);
void mt29f_format(Mt29f *chip);

#ifdef _DEBUG
void mt29f_ruinSomeBlocks(Mt29f *chip);
#endif

// Hardware specific functions, implemented by cpu specific module
bool mt29f_waitReadyBusy(Mt29f *chip, time_t timeout);
bool mt29f_waitTransferComplete(Mt29f *chip, time_t timeout);
void mt29f_sendCommand(Mt29f *chip, uint32_t cmd1, uint32_t cmd2,
		int num_cycles, uint32_t cycle0, uint32_t cycle1234);
uint8_t mt29f_getChipStatus(Mt29f *chip);
void *mt29f_dataBuffer(Mt29f *chip);
bool mt29f_checkEcc(Mt29f *chip);
void mt29f_computeEcc(Mt29f *chip, const void *buf, size_t size, uint32_t *ecc, size_t ecc_size);
void mt29f_hwInit(Mt29f *chip);

#endif /* DRV_MT29F_H */

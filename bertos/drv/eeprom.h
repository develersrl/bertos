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
 * Copyright 2003, 2004 Develer S.r.l. (http://www.develer.com/)
 * All Rights Reserved.
 * -->
 *
 * \brief Driver for the 24xx16 and 24xx256 I2C EEPROMS.
 *
 * \author Stefano Fedrigo <aleph@develer.com>
 * \author Bernie Innocenti <bernie@codewiz.org>
 *
 * $WIZ$ module_name = "eeprom"
 * $WIZ$ module_depends = "kfile", "i2c"
 */

#ifndef DRV_EEPROM_H
#define DRV_EEPROM_H

#define EEPROM_OLD_API  1

#include <cfg/compiler.h>
#include <cfg/debug.h>

#include <drv/i2c.h>

#include <io/kblock.h>
#include <io/kfile.h>
#include <io/kfile_block.h>

#include <cpu/attr.h>

#if COMPILER_C99
	#define eeprom_init(...)        PP_CAT(eeprom_init ## _, COUNT_PARMS(__VA_ARGS__)) (__VA_ARGS__)
#else
	#define eeprom_init(args...)    PP_CAT(eeprom_init ## _, COUNT_PARMS(args)) (args)
#endif


/**
 * Values for Eeprom types.
 */
typedef enum EepromType
{
	EEPROM_24XX08,
	EEPROM_24XX16,
	EEPROM_24XX256,
	EEPROM_24XX512,
	EEPROM_24XX1024,
	EEPROM_CNT,
} EepromType;

/**
 * On the same I2C bus can live more than one EEPROM
 * device. Each device can be addressed by one or more
 * pins.
 */
typedef uint8_t e2dev_addr_t;

/**
 * Describe an EEPROM context, used by the driver to
 * access the single device.
 */
typedef struct Eeprom
{
	KBlock blk;
	I2c *i2c;
	EepromType type;   ///< EEPROM type
	e2dev_addr_t addr; ///< Device address.
	bool verify;
#if EEPROM_OLD_API
	union {
		KFile fd;
		KFileBlock fdblk;
	} DEPRECATED;
#endif
} Eeprom;

#if EEPROM_OLD_API
	STATIC_ASSERT(offsetof(Eeprom, fd) == offsetof(Eeprom, fdblk.fd));
#endif

/**
 * ID for eeproms.
 */
#define KBT_EEPROM MAKE_ID('E', 'E', 'P', 'R')

/**
 * Convert + ASSERT from generic KFile to Eeprom.
 */
INLINE Eeprom * EEPROM_CAST_KBLOCK(KBlock *blk)
{
	ASSERT(blk->priv.type == KBT_EEPROM);
	return (Eeprom *)blk;
}

/// Type for EEPROM addresses
typedef uint16_t e2addr_t;

/**
 * Macro for E2Layout offset calculation
 *
 * \note We can't just use offsetof() here because we could use
 *       non-constant expressions to access array elements.
 *
 * \note 'type' is the structure type holding eeprom layout
 *       and must be defined in user files.
 */
#define e2addr(type, field) ((e2addr_t)&(((type *)0)->field))

/**
 * Type for EEPROM block size.
 */
typedef uint16_t e2blk_size_t;

/**
 * Type for accessing EEPROM whole size.
 */
typedef uint32_t e2_size_t;

/**
 * Descrive all EEPROM informations
 * needed by the driver.
 */
typedef struct EepromInfo
{
	bool has_dev_addr;     ///< true if memory is device addressable (its A0, A1, A2 pin are used by memory).
	e2blk_size_t blk_size; ///< block size
	e2_size_t e2_size;     ///< eeprom size
} EepromInfo;

void eeprom_init_5(Eeprom *blk, I2c *i2c, EepromType type, e2dev_addr_t addr, bool verify);

#if EEPROM_OLD_API
DEPRECATED bool eeprom_erase(Eeprom *fd, e2addr_t addr, e2_size_t count);
DEPRECATED bool eeprom_verify(Eeprom *fd, const void *buf, size_t count);


DEPRECATED INLINE void eeprom_init_4(Eeprom *ee, EepromType type, e2dev_addr_t addr, bool verify)
{
	eeprom_init_5(ee, &local_i2c_old_api, type, addr, verify);
	kfileblock_init(&ee->fdblk, &ee->blk);
}

#endif

#endif /* DRV_EEPROM_H */

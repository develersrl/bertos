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
 * \brief Driver for the 24xx16 and 24xx256 I2C EEPROMS (interface)
 *
 * \version $Id$
 *
 * \author Stefano Fedrigo <aleph@develer.com>
 * \author Bernardo Innocenti <bernie@develer.com>
 *
 */

#ifndef DRV_EEPROM_H
#define DRV_EEPROM_H

#include "cfg/cfg_eeprom.h"
#include <cfg/compiler.h>

/**
 * \name Values for CONFIG_EEPROM_TYPE
 * \{
 */
#define EEPROM_24XX16 1
#define EEPROM_24XX256 2
/*\}*/

#if CONFIG_EEPROM_TYPE == EEPROM_24XX16
	#define EEPROM_BLKSIZE   0x10 ///< Eeprom block size (16byte)
	#define EEPROM_SIZE     0x800 ///< Eeprom total size (2kB)
#elif CONFIG_EEPROM_TYPE == EEPROM_24XX256
	#define EEPROM_BLKSIZE   0x40 ///< Eeprom block size (64byte)
	#define EEPROM_SIZE    0x8000 ///< Eeprom total size (32kB)
#else
	#error Unsupported EEPROM type.
#endif

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


bool eeprom_write(e2addr_t addr, const void *buf, size_t count);
bool eeprom_read(e2addr_t addr, void *buf, size_t count);
bool eeprom_write_char(e2addr_t addr, char c);
int eeprom_read_char(e2addr_t addr);
void eeprom_erase(e2addr_t addr, size_t count);
void eeprom_init(void);
void eeprom_test(void);

#endif /* DRV_EEPROM_H */

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
 * \brief PCF8574 i2c port expander driver.
 *
 * \version $Id$
 * \author Francesco Sacchi <batt@develer.com>
 *
 * $WIZ$ module_name = "pcf8574"
 * $WIZ$ module_depends = "i2c"
 */

#ifndef DRV_PCF8574_H
#define DRV_PCF8574_H

#include <cfg/compiler.h>

typedef uint8_t pcf8574_addr;

/**
 * Context for accessing a PCF8574.
 */
typedef struct Pcf8574
{
	pcf8574_addr addr;
} Pcf8574;

#define PCF8574ID 0x40 ///< I2C address

int pcf8574_get(Pcf8574 *pcf);
bool pcf8574_put(Pcf8574 *pcf, uint8_t data);
bool pcf8574_init(Pcf8574 *pcf, pcf8574_addr addr);

#endif /* DRV_PCF8574_H */

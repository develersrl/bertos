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
 * \brief LM75 sensor temperature family.
 *
 * \author Daniele Basile <asterix@develer.com>
 *
 */

#include "lm75.h"

#include "hw/hw_lm75.h"

#include "cfg/cfg_lm75.h"

#include <cfg/debug.h>
#include <cfg/module.h>

// Define logging setting (for cfg/log.h module).
#define LOG_LEVEL   LM75_LOG_LEVEL
#define LOG_FORMAT  LM75_LOG_FORMAT

#include <cfg/log.h>

#include <drv/i2c.h>
#include <drv/ntc.h> // Macro and data type to manage celsius degree

#define SELECT_ADDRESS(addr)   LM75_ADDRESS_BYTE | (addr << 1)

deg_t lm75_read(uint8_t sens_addr)
{
	uint8_t data[2];
	int16_t degree;
	int16_t deci_degree;

	i2c_start_w(SELECT_ADDRESS(sens_addr));
	i2c_put(LM75_PAD_BYTE);
	i2c_start_r(SELECT_ADDRESS(sens_addr));
	i2c_recv(data, sizeof(data));

	degree = (int16_t)data[0];
	deci_degree = (int16_t)(((data[1] >> 7) & 1 ) * 5);

	LOG_INFO("[%d.%d C]\n", degree, deci_degree);

	return degree * 10 + deci_degree;
}

void lm75_init(void)
{
	// Check dependence
	MOD_CHECK(i2c);
	LM75_HW_INIT();
}



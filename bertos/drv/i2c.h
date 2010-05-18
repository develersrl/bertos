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
 *
 * -->
 *
 * \brief I2C generic driver functions.
 *
 * \author Francesco Sacchi <batt@develer.com>
 *
 * $WIZ$ module_name = "i2c"
 * $WIZ$ module_configuration = "bertos/cfg/cfg_i2c.h"
 * $WIZ$ module_hw = "bertos/hw/hw_i2c_bitbang.h"
 * $WIZ$ module_supports = "not atmega103 and not atmega168 and not at91"
 */

#ifndef DRV_I2C_H
#define DRV_I2C_H

#include "cfg/cfg_i2c.h"
#include <cfg/compiler.h>

#define I2C_READBIT BV(0)

/**
 * I2C Backends.
 * Sometimes your cpu does not have a builtin
 * i2c driver or you don't want, for some reason, to
 * use that.
 * With this you can choose, at compile time, which backend to use.
 *
 * $WIZ$ i2c_backend = "I2C_BACKEND_BUILTIN", "I2C_BACKEND_BITBANG"
 */
#define I2C_BACKEND_BUILTIN 0 ///< Uses cpu builtin i2c driver
#define I2C_BACKEND_BITBANG 1 ///< Uses emulated bitbang driver


/**
 * I2c builtin prototypes.
 * Do NOT use these function directly, instead,
 * you can call the ones named without "_builtin_"
 * and specify in cfg_i2c.h ( \see CONFIG_I2C_BACKEND)
 * that you want the builtin backend.
 * \{
 */
void i2c_builtin_init(void);
bool i2c_builtin_start_w(uint8_t id);
bool i2c_builtin_start_r(uint8_t id);
void i2c_builtin_stop(void);
bool i2c_builtin_put(uint8_t _data);
int i2c_builtin_get(bool ack);
/*\}*/

/**
 * I2c bitbang prototypes.
 * Same thing here: do NOT use these function directly, instead,
 * you can call the ones named without "_bitbang_"
 * and specify in cfg_i2c.h ( \see CONFIG_I2C_BACKEND)
 * that you want the bitbang backend.
 * \{
 */
void i2c_bitbang_init(void);
bool i2c_bitbang_start_w(uint8_t id);
bool i2c_bitbang_start_r(uint8_t id);
void i2c_bitbang_stop(void);
bool i2c_bitbang_put(uint8_t _data);
int i2c_bitbang_get(bool ack);
/*\}*/

#if CONFIG_I2C_BACKEND == I2C_BACKEND_BUILTIN
	#define i2c_init    i2c_builtin_init
	#define i2c_start_w i2c_builtin_start_w
	#define i2c_start_r i2c_builtin_start_r
	#define i2c_stop    i2c_builtin_stop
	#define i2c_put     i2c_builtin_put
	#define i2c_get     i2c_builtin_get
#elif CONFIG_I2C_BACKEND == I2C_BACKEND_BITBANG
	#define i2c_init    i2c_bitbang_init
	#define i2c_start_w i2c_bitbang_start_w
	#define i2c_start_r i2c_bitbang_start_r
	#define i2c_stop    i2c_bitbang_stop
	#define i2c_put     i2c_bitbang_put
	#define i2c_get     i2c_bitbang_get
#else
	#error Unsupported i2c backend.
#endif

bool i2c_send(const void *_buf, size_t count);
bool i2c_recv(void *_buf, size_t count);

#endif

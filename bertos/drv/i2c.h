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

#define i2c_init(FN_ARGS) CAT(fn ## _, COUNT_PARMS(FN_ARGS)) (FN_ARGS)

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
	#define i2c_init_0    i2c_builtin_init
	#define i2c_start_w   i2c_builtin_start_w
	#define i2c_start_r   i2c_builtin_start_r
	#define i2c_stop      i2c_builtin_stop
	#define i2c_put       i2c_builtin_put
	#define i2c_get       i2c_builtin_get
#elif CONFIG_I2C_BACKEND == I2C_BACKEND_BITBANG
	#define i2c_init_0    i2c_bitbang_init
	#define i2c_start_w   i2c_bitbang_start_w
	#define i2c_start_r   i2c_bitbang_start_r
	#define i2c_stop      i2c_bitbang_stop
	#define i2c_put       i2c_bitbang_put
	#define i2c_get       i2c_bitbang_get
#else
	#error Unsupported i2c backend.
#endif

bool i2c_send(const void *_buf, size_t count);
bool i2c_recv(void *_buf, size_t count);




/*
 * I2c new api
 *
 */
#include CPU_HEADER(i2c)

struct I2cHardware;
typedef int (*i2c_writeRope_t)(I2c *i2c, uint16_t slave_addr, int flags, const void *buf, size_t len, ...);
typedef int (*i2c_readRope_t)(I2c *i2c, uint16_t slave_addr, int flags, void *buf, size_t len, ...);

typedef struct I2c
{
	int dev;
	i2c_writeRope_t write;
	i2c_readRope_t read;

	struct I2cHardware* hw;
} I2c;

void i2c_init_3(I2c *i2c, int dev, uint32_t clock)
{
	i2c_hw_init(I2c *i2c, int dev, uint32_t clock);
}

#define i2c_write(FN_ARGS) CAT(fn ## _, COUNT_PARMS(FN_ARGS)) (FN_ARGS)
#define i2c_read(FN_ARGS) CAT(fn ## _, COUNT_PARMS(FN_ARGS)) (FN_ARGS)

/*
 * Overloaded functions definition.
 */
int i2c_write_5(I2c *i2c, uint16_t slave_addr, int flags, const void *buf, size_t len)
{
	return i2c->write(i2c, slave_addr, flags, buf, len, NULL);
}

int i2c_read_5(I2c *i2c, uint16_t slave_addr, int flags, void *buf, size_t len);
{
	return i2c->read(i2c, slave_addr, flags, buf, len, NULL);
}


int i2c_write_7(I2c *i2c, uint16_t slave_addr, int flags, const void *buf, size_t len,
														  const void *buf1, size_t len1);
{
	return i2c->write(i2c, slave_addr, flags, buf, len,
											  buf1, len1, NULL);
}

int i2c_read_7(I2c *i2c, uint16_t slave_addr, int flags, void *buf, size_t len,
														 void *buf1, size_t len1);

{
	return i2c->read(i2c, slave_addr, flags, buf, len,
											 buf1, len1, NULL);
}


int i2c_write_9(I2c *i2c, uint16_t slave_addr, int flags, const void *buf, size_t len,
														  const void *buf1, size_t len1,
														  const void *buf2, size_t len2);
{
	return i2c->write(i2c, slave_addr, flags, buf, len,
											  buf1, len1,
											  buf2, len2, NULL);
}

int i2c_read_9(I2c *i2c, uint16_t slave_addr, int flags, void *buf, size_t len,
														 void *buf1, size_t len1,
														 void *buf2, size_t len2);
{
	return i2c->read(i2c, slave_addr, flags, buf, len,
											  buf1, len1,
											  buf2, len2, NULL);
}

int i2c_write_11(I2c *i2c, uint16_t slave_addr, int flags, const void *buf, size_t len,
														  const void *buf1, size_t len1,
														  const void *buf2, size_t len2
														  const void *buf3, size_t len3);
{
	return i2c->write(i2c, slave_addr, flags, buf, len,
											  buf1, len1,
											  buf2, len2,
											  buf3, len3, NULL);
}

int i2c_read_11(I2c *i2c, uint16_t slave_addr, int flags, void *buf, size_t len,
														 void *buf1, size_t len1,
														 void *buf2, size_t len2
														 void *buf3, size_t len3);

{
	return i2c->read(i2c, slave_addr, flags, buf, len,
											  buf1, len1,
											  buf2, len2,
											  buf3, len3, NULL);
}

#endif

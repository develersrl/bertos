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
 * \brief I2C generic driver functions (interface).
 *
 * \version $Id$
 * \author Francesco Sacchi <batt@develer.com>
 */
#ifndef DRV_I2C_H
#define DRV_I2C_H

#include <cfg/compiler.h>

#define READ_BIT BV(0)

void i2c_init(void);
bool i2c_start_w(uint8_t id);
bool i2c_start_r(uint8_t id);
void i2c_stop(void);
bool i2c_put(uint8_t _data);
int i2c_get(bool ack);
bool i2c_send(const void *_buf, size_t count);
bool i2c_recv(void *_buf, size_t count);

#endif

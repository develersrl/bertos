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
 * \brief I2C bitbang driver (implementation)
 *
 * \author Francesco Sacchi <batt@develer.com>
 */

#include "i2c.h"
#include "cfg/cfg_i2c.h"

#define LOG_LEVEL  I2C_LOG_LEVEL
#define LOG_FORMAT I2C_LOG_FORMAT

#include <cfg/log.h>
#include <cfg/macros.h>
#include <cfg/module.h>

#include <drv/timer.h>
#include <cpu/irq.h>

#include "hw/hw_i2c_bitbang.h"

INLINE bool i2c_bitbang_start(void)
{
	SDA_HI;
	SCL_HI;
	I2C_HALFBIT_DELAY();
	SDA_LO;
	I2C_HALFBIT_DELAY();
	ASSERT(!SDA_IN);
	return !SDA_IN;
}

void i2c_bitbang_stop(void)
{
	SDA_LO;
	SCL_HI;
	I2C_HALFBIT_DELAY();
	SDA_HI;
}

bool i2c_bitbang_put(uint8_t _data)
{
	/* Add ACK bit */
	uint16_t data = (_data << 1) | 1;

	for (uint16_t i = 0x100; i != 0; i >>= 1)
	{
		SCL_LO;
		if (data & i)
			SDA_HI;
		else
			SDA_LO;
		I2C_HALFBIT_DELAY();

		SCL_HI;
		I2C_HALFBIT_DELAY();
	}

	bool ack = !SDA_IN;
	SCL_LO;
	I2C_HALFBIT_DELAY();
	return ack;
}

bool i2c_bitbang_start_w(uint8_t id)
{
	id &= ~I2C_READBIT;
	/*
	 * Loop on the select write sequence: when the device is busy
	 * writing previously sent data it will reply to the SLA_W
	 * control byte with a NACK.  In this case, we must
	 * keep trying until the deveice responds with an ACK.
	 */
	ticks_t start = timer_clock();
	while (i2c_bitbang_start())
	{
		if (i2c_bitbang_put(id))
			return true;
		else if (timer_clock() - start > ms_to_ticks(CONFIG_I2C_START_TIMEOUT))
		{
			LOG_ERR("Timeout on I2C start write\n");
			break;
		}
		//LOG_INFO("Rep start\n");
	}

	return false;
}

bool i2c_bitbang_start_r(uint8_t id)
{
	id |= I2C_READBIT;
	if (i2c_bitbang_start())
	{
		if (i2c_bitbang_put(id))
			return true;

		LOG_ERR("NACK on I2c start read\n");
	}

	return false;
}

int i2c_bitbang_get(bool ack)
{
	uint8_t data = 0;
	for (uint8_t i = 0x80; i != 0; i >>= 1)
	{
		SCL_LO;
		I2C_HALFBIT_DELAY();
		SCL_HI;
		if (SDA_IN)
			data |= i;
		else
			data &= ~i;

		I2C_HALFBIT_DELAY();
	}
	SCL_LO;

	if (ack)
		SDA_LO;
	else
		SDA_HI;

	I2C_HALFBIT_DELAY();
	SCL_HI;
	I2C_HALFBIT_DELAY();
	SCL_LO;
	SDA_HI;
	/* avoid sign extension */
	return (int)(uint8_t)data;
}

MOD_DEFINE(i2c);

/**
 * Initialize i2c module.
 */
void i2c_bitbang_init(void)
{
	MOD_CHECK(timer);
	I2C_BITBANG_HW_INIT;
	SDA_HI;
	SCL_HI;
	MOD_INIT(i2c);
}


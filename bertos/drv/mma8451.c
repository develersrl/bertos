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
 * Copyright 2015 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief MMA8451 Freescale accelerometer sensor.
 *
 * \author Daniele Basile <asterix@develer.com>
 *
 */

#include "mma8451.h"
#include "pio_irq.h"

#include "cfg/cfg_mma8451.h"
#include "hw/hw_mma8451.h"

#include <cfg/debug.h>

#include <drv/i2c.h>
#include <drv/timer.h>

#include <cpu/types.h>
#include <cpu/power.h>

#include <mware/event.h>

// Define logging setting (for cfg/log.h module).
#define LOG_LEVEL   MMA8451_LOG_LEVEL
#define LOG_FORMAT  MMA8451_LOG_FORMAT

#include <cfg/log.h>

static uint8_t buf[6];
static Event data_ready_ev;

INLINE int write_reg(AccMMA *mma, uint8_t addr, uint8_t data)
{
	i2c_start_w(mma->i2c, mma->dev_addr,  2, I2C_STOP);
	i2c_putc(mma->i2c, addr);
	i2c_putc(mma->i2c, data);

	LOG_INFO("Reg[%0x], Value[%0x]\n", addr, data);

	if (i2c_error(mma->i2c))
	{
		LOG_ERR("Errore!\n");
		return -1;
	}

	return data;

}

INLINE int read_reg(AccMMA *mma, uint8_t addr)
{
	i2c_start_w(mma->i2c, mma->dev_addr,  1, I2C_NOSTOP);
	i2c_putc(mma->i2c, addr);
	i2c_start_r(mma->i2c, mma->dev_addr, 1, I2C_STOP);

	uint8_t data = i2c_getc(mma->i2c);
	//LOG_INFO("%.2x\n", data);

	if (i2c_error(mma->i2c))
	{
		LOG_ERR("Errore!\n");
		return -1;
	}

	return data;
}

INLINE int reset(AccMMA *mma)
{
	write_reg(mma, MMA8451_REG_CTRL_REG2, 0x40);

	ticks_t start = timer_clock();
	while (1)
	{
		uint8_t data = read_reg(mma, MMA8451_REG_CTRL_REG2);
		if (!(data & 0x40))
		{
			LOG_INFO("Reset ok!\n");
			return 0;
		}

		if (timer_clock() - start > ms_to_ticks(CFG_MMA8451_TIMEOUT))
		{
			LOG_ERR("Reset TIMEOUT!\n");
			return -1;
		}
		cpu_relax();
	}

	return 0;
}

static bool acc_event_flag = false;
static bool data_event_flag = false;

static void acc_event(uint32_t pins)
{
	(void)pins;
	acc_event_flag = true;
	event_do(&data_ready_ev);
}

static void data_ready(uint32_t pins)
{
	(void)pins;
	data_event_flag = true;
	// Data ready from accellerometer start to acquire
	event_do(&data_ready_ev);
}


int mma8451_setMode(AccMMA *mma, uint8_t mode)
{
	uint8_t reg = read_reg(mma, MMA8451_REG_CTRL_REG1);

	if (mode == MMA8451_ACTIVE_MODE)
	{
		reg = reg | MMA8451_ACTIVE;
		LOG_INFO("Active mode\n");
		pio_irq_set(EXTI_GPIOA, ACC_INT1_BIT, TRIGGER_RISING_EDGE, data_ready);
		pio_irq_set(EXTI_GPIOC, ACC_INT2_BIT, TRIGGER_RISING_EDGE, acc_event);
	}

	if (mode == MMA8451_STANDBY_MODE)
	{
		reg = reg & ~MMA8451_ACTIVE;
		LOG_INFO("Sleep mode\n");
		pio_irq_clear(ACC_INT1_BIT);
		pio_irq_clear(ACC_INT2_BIT);
	}

	return write_reg(mma, MMA8451_REG_CTRL_REG1, reg);
}

int mma8451_readData(AccMMA *mma, AccData *data)
{
	ASSERT(data);
	memset(data, 0, sizeof(AccData));

	LOG_INFO("wait data\n");
	event_wait(&data_ready_ev);

	if (data_event_flag)
	{
		data_event_flag = false;
		data->data_status = read_reg(mma, MMA8451_REG_STATUS);
		i2c_start_w(mma->i2c, mma->dev_addr,  1, I2C_NOSTOP);
		i2c_putc(mma->i2c, MMA8451_REG_OUT_X_MSB);
		i2c_start_r(mma->i2c, mma->dev_addr, 6, I2C_STOP);

		memset(buf, 0, sizeof(buf));
		i2c_read(mma->i2c, buf, sizeof(buf));

		if (i2c_error(mma->i2c))
		{
			LOG_ERR("Errore!\n");
			return -1;
		}
		else
		{
			data->x = (int16_t)((buf[0] << 8) | buf[1]) >> 2;
			data->y = (int16_t)((buf[2] << 8) | buf[3]) >> 2;
			data->z = (int16_t)((buf[4] << 8) | buf[5]) >> 2;
		}
	}

	if (acc_event_flag)
	{
		acc_event_flag = false;
		data->ff_status = read_reg(mma,  MMA8451_REG_FF_MT_SRC);
		data->pl_status = read_reg(mma,  MMA8451_REG_PL_STATUS);
		data->tap_status = read_reg(mma, MMA8451_REG_PULSE_SRC);
	}

	return 0;
}

void mma8451_updateCfg(AccMMA *mma, const AccCfg *cfg)
{
	// Apply all settings
	for (int i = 0;; i++)
	{
		if ((cfg[i].reg == MMA8451_CFG_END) &&
			(cfg[i].value == MMA8451_CFG_END))
		{
			return;
		}
		write_reg(mma, cfg[i].reg, cfg[i].value);
	}
	return;
}

bool mma8451_init(AccMMA *mma, const AccCfg *cfg, uint8_t dev_addr, I2c *i2c)
{
	ACC_HW_INIT();

	mma->i2c = i2c;
	mma->dev_addr = dev_addr;

	// init data events
	event_initGeneric(&data_ready_ev);
	LOG_INFO("Acc init ev..done\n");

	// Check correct device ID
	uint8_t id = read_reg(mma, MMA8451_REG_WHOAMI);
	if (id != MMA8451_ID)
	{
		LOG_ERR("Wrong device id[%d != %d]\n", MMA8451_ID, id);
		return false;
	}

	// Reset device settings
	reset(mma);
	LOG_INFO("Acc reset..done\n");
	mma8451_updateCfg(mma, cfg);
	LOG_INFO("Acc set cfg..done\n");

	return true;
}



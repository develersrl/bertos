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
 * Copyright 2010 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief STM32F103xx I2C driver.
 *
 * \author Daniele Basile <asterix@develer.com>
 */

#include "cfg/cfg_i2c.h"

#define LOG_LEVEL  I2C_LOG_LEVEL
#define LOG_FORMAT I2C_LOG_FORMAT
#include <cfg/log.h>

#include <cfg/debug.h>
#include <cfg/macros.h> // BV()
#include <cfg/module.h>

#include <drv/gpio_stm32.h>
#include <drv/irq_cm3.h>
#include <drv/clock_stm32.h>
#include <drv/i2c.h>
#include <drv/timer.h>

#include <io/stm32.h>


struct I2cHardware
{
	struct stm32_i2c *base;
	uint32_t clk_gpio_en;
	uint32_t clk_i2c_en;
	struct stm32_gpio *gpio_base;
	uint32_t pin_mask;
	uint8_t cache[2];
	bool cached;
};

#define WAIT_BTF(base)        while( !(base->SR1 & BV(SR1_BTF)) )
#define WAIT_RXNE(base)        while( !(base->SR1 & BV(SR1_RXNE)) )

INLINE uint32_t get_status(struct stm32_i2c *base)
{
	return ((base->SR1 | (base->SR2 << 16)) & 0x00FFFFFF);
}


INLINE bool check_i2cStatus(I2c *i2c, uint32_t event)
{
	while (true)
	{
		uint32_t stat = get_status(i2c->hw->base);

		if (stat == event)
			break;

		if (stat & SR1_ERR_MASK)
		{
			i2c->hw->base->SR1 &= ~SR1_ERR_MASK;
			i2c->hw->base->CR1 |= CR1_START_SET;
			return false;
		}

	}

	return true;
}

/**
 * Send START condition on the bus.
 *
 * \return true on success, false otherwise.
 */
INLINE bool i2c_hw_restart(I2c *i2c)
{

	i2c->hw->base->CR1 |= CR1_ACK_SET | CR1_START_SET;

	if(check_i2cStatus(i2c, I2C_EVENT_MASTER_MODE_SELECT))
		return true;

	return false;
}

/**
 * Send STOP condition.
 */
static void i2c_hw_stop(I2c *i2c)
{
	i2c->hw->base->CR1 |= CR1_STOP_SET;
}

static void i2c_stm32_start(struct I2c *i2c, uint16_t slave_addr)
{
	i2c->hw->cached = false;

	if (I2C_TEST_START(i2c->flags) == I2C_START_W)
	{
		/*
		 * Loop on the select write sequence: when the eeprom is busy
		 * writing previously sent data it will reply to the SLA_W
		 * control byte with a NACK.  In this case, we must
		 * keep trying until the eeprom responds with an ACK.
		 */
		ticks_t start = timer_clock();
		while (i2c_hw_restart(i2c))
		{
			i2c->hw->base->DR = slave_addr & OAR1_ADD0_RESET;

			if(check_i2cStatus(i2c, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
				break;

			if (timer_clock() - start > ms_to_ticks(CONFIG_I2C_START_TIMEOUT))
			{
				LOG_ERR("Timeout on I2C START\n");
				i2c->errors |= I2C_NO_ACK;
				i2c_hw_stop(i2c);
				break;
			}
		}

	}
	else if (I2C_TEST_START(i2c->flags) == I2C_START_R)
	{
		i2c->hw->base->CR1 |= CR1_START_SET;

		if(!check_i2cStatus(i2c, I2C_EVENT_MASTER_MODE_SELECT))
		{
			LOG_ERR("ARBIT lost\n");
			i2c->errors |= I2C_ARB_LOST;
			i2c_hw_stop(i2c);
			return;
		}

		i2c->hw->base->DR = (slave_addr | OAR1_ADD0_SET);

		if (i2c->xfer_size == 2)
			i2c->hw->base->CR1 |= CR1_ACK_SET | CR1_POS_SET;

		if(!check_i2cStatus(i2c, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
		{
			LOG_ERR("SLAR NACK:%08lx\n", get_status(i2c->hw->base));
			i2c->errors |= I2C_NO_ACK;
			i2c_hw_stop(i2c);
			return;
		}


		if (i2c->xfer_size == 1)
		{
			i2c->hw->base->CR1 &= CR1_ACK_RESET;

			cpu_flags_t irq;
			IRQ_SAVE_DISABLE(irq);

			(void)i2c->hw->base->SR2;

			if (I2C_TEST_STOP(i2c->flags) == I2C_STOP)
				i2c->hw->base->CR1 |= CR1_STOP_SET;

			IRQ_RESTORE(irq);

			WAIT_RXNE(i2c->hw->base);

			i2c->hw->cache[0] = i2c->hw->base->DR;
			i2c->hw->cached = true;

			if (I2C_TEST_STOP(i2c->flags) == I2C_STOP)
				while (i2c->hw->base->CR1 & CR1_STOP_SET);

			i2c->hw->base->CR1 |= CR1_ACK_SET;

		}
		else if (i2c->xfer_size == 2)
		{
			cpu_flags_t irq;
			IRQ_SAVE_DISABLE(irq);

			(void)i2c->hw->base->SR2;

			i2c->hw->base->CR1 &= CR1_ACK_RESET;

			IRQ_RESTORE(irq);

			WAIT_BTF(i2c->hw->base);

			IRQ_SAVE_DISABLE(irq);

			if (I2C_TEST_STOP(i2c->flags) == I2C_STOP)
				i2c->hw->base->CR1 |= CR1_STOP_SET;

			/*
			 * We store read bytes like a fifo..
			 */
			i2c->hw->cache[1] = i2c->hw->base->DR;
			i2c->hw->cache[0] = i2c->hw->base->DR;
			i2c->hw->cached = true;

			IRQ_RESTORE(irq);

			i2c->hw->base->CR1 &= CR1_POS_RESET;
			i2c->hw->base->CR1 |= CR1_ACK_SET;
		}
	}
	else
	{
		ASSERT(0);
	}

}

static void i2c_stm32_put(I2c *i2c, const uint8_t data)
{
	i2c->hw->base->DR = data;

	WAIT_BTF(i2c->hw->base);


	/* Generate the stop if we finish to send all programmed bytes */
	if ((i2c->xfer_size == 1) &&(I2C_TEST_STOP(i2c->flags) == I2C_STOP))
	{
			check_i2cStatus(i2c, I2C_EVENT_MASTER_BYTE_TRANSMITTED);
			i2c_hw_stop(i2c);
	}
}

static uint8_t i2c_stm32_get(I2c *i2c)
{
	if (i2c->hw->cached)
	{
		ASSERT(i2c->xfer_size <= 2);
		return i2c->hw->cache[i2c->xfer_size - 1];
	}
	else
	{
		WAIT_BTF(i2c->hw->base);

		if (i2c->xfer_size == 3)
		{
			i2c->hw->base->CR1 &= CR1_ACK_RESET;

			cpu_flags_t irq;
			IRQ_SAVE_DISABLE(irq);

			uint8_t data = i2c->hw->base->DR;

			if (I2C_TEST_STOP(i2c->flags) == I2C_STOP)
				i2c->hw->base->CR1 |= CR1_STOP_SET;

			i2c->hw->cache[1] = i2c->hw->base->DR;

			IRQ_RESTORE(irq);

			WAIT_RXNE(i2c->hw->base);

			i2c->hw->cache[0] = i2c->hw->base->DR;
			i2c->hw->cached = true;

			if (I2C_TEST_STOP(i2c->flags) == I2C_STOP)
				while (i2c->hw->base->CR1 & CR1_STOP_SET);

			return data;
		}
		else
			return i2c->hw->base->DR;
	}
}


static const I2cVT i2c_stm32_vt =
{
	.start = i2c_stm32_start,
	.get = i2c_stm32_get,
	.put = i2c_stm32_put,
	.send = i2c_swSend,
	.recv = i2c_swRecv,
};

struct I2cHardware i2c_stm32_hw[] =
{
	{ /* I2C1 */
		.base = (struct stm32_i2c *)I2C1_BASE,
		.clk_gpio_en = RCC_APB2_GPIOB,
		.clk_i2c_en  = RCC_APB1_I2C1,
		.gpio_base = (struct stm32_gpio *)GPIOB_BASE,
		.pin_mask = (GPIO_I2C1_SCL_PIN | GPIO_I2C1_SDA_PIN),
	},
	#if 0
	{ /* I2C2 */
		.base = (struct stm32_i2c *)I2C2_BASE,
		.clk_gpio_en = RCC_APB2_GPIO--,
		.clk_i2c_en  = RCC_APB1_I2C2,
		.gpio_base = (struct stm32_gpio *)GPIO---_BASE,
		.pin_mask = (GPIO_I2C2_SCL_PIN | GPIO_I2C2_SDA_PIN),
	},
	#endif
};

MOD_DEFINE(i2c);

/**
 * Initialize I2C module.
 */
void i2c_hw_init(I2c *i2c, int dev, uint32_t clock)
{

	i2c->hw = &i2c_stm32_hw[dev];
	i2c->vt = &i2c_stm32_vt;

	RCC->APB2ENR |= i2c->hw->clk_gpio_en;
	RCC->APB1ENR |= i2c->hw->clk_i2c_en;

	/* Set gpio to use I2C driver */
	stm32_gpioPinConfig(i2c->hw->gpio_base, i2c->hw->pin_mask,
				GPIO_MODE_AF_OD, GPIO_SPEED_50MHZ);

	/* Clear all needed registers */
	i2c->hw->base->CR1 = 0;
	i2c->hw->base->CR2 = 0;
	i2c->hw->base->CCR = 0;
	i2c->hw->base->TRISE = 0;
	i2c->hw->base->OAR1 = 0;

	/* Set PCLK1 frequency accornding to the master clock settings. See stm32_clock.c */
	i2c->hw->base->CR2 |= CR2_FREQ_36MHZ;

	/* Configure spi in standard mode */
	ASSERT2(clock >= 100000, "fast mode not supported");

	i2c->hw->base->CCR |= (uint16_t)((CR2_FREQ_36MHZ * 1000000) / (clock << 1));
	i2c->hw->base->TRISE |= (CR2_FREQ_36MHZ + 1);

	i2c->hw->base->CR1 |= CR1_PE_SET;

	MOD_INIT(i2c);
}

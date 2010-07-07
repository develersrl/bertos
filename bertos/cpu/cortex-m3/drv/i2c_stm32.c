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

struct stm32_i2c *i2c = (struct stm32_i2c *)I2C1_BASE;

INLINE uint32_t get_status(struct stm32_i2c *base)
{
	return ((base->SR1 | (base->SR2 << 16)) & 0x00FFFFFF);
}

/**
 * Send START condition on the bus.
 *
 * \return true on success, false otherwise.
 */
static bool i2c_builtin_start(void)
{

	i2c->CR1 |= CR1_ACK_SET;
	i2c->CR1 |= BV(CR1_POS);

	i2c->CR1 |= CR1_PE_SET;
	i2c->CR1 |= CR1_START_SET;

	while (true)
	{
		uint32_t stat = get_status(i2c);

		if (stat == I2C_EVENT_MASTER_MODE_SELECT)
			break;

		if (stat & SR1_ERR_MASK)
		{
			LOG_ERR("s_error[%08lx]\n", stat & SR1_ERR_MASK);
			i2c->SR1 &= ~SR1_ERR_MASK;
			return false;
		}

	}

	return true;
}


/**
 * Send START condition and select slave for write.
 * \c id is the device id comprehensive of address left shifted by 1.
 * The LSB of \c id is ignored and reset to 0 for write operation.
 *
 * \return true on success, false otherwise.
 */
bool i2c_builtin_start_w(uint8_t id)
{

	/*
	 * Loop on the select write sequence: when the eeprom is busy
	 * writing previously sent data it will reply to the SLA_W
	 * control byte with a NACK.  In this case, we must
	 * keep trying until the eeprom responds with an ACK.
	 */
	ticks_t start = timer_clock();
	while (i2c_builtin_start())
	{
		i2c->DR = id & OAR1_ADD0_RESET;

		while (true)
		{
			uint32_t stat = get_status(i2c);
			//LOG_ERR("[%08lx]\n", stat);

			if (stat == I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)
				return true;

			if (stat & SR1_ERR_MASK)
			{
				LOG_ERR("w_error[%08lx]\n", stat & SR1_ERR_MASK);
				i2c->SR1 &= ~SR1_ERR_MASK;

				i2c->CR1 |= CR1_START_SET;
				//i2c->CR1 &= CR1_PE_RESET;

				break;
			}
		}

		if (timer_clock() - start > ms_to_ticks(CONFIG_I2C_START_TIMEOUT))
		{
			LOG_ERR("Timeout on I2C_START\n");
			break;
		}
	}

	return false;
}


/**
 * Send START condition and select slave for read.
 * \c id is the device id comprehensive of address left shifted by 1.
 * The LSB of \c id is ignored and set to 1 for read operation.
 *
 * \return true on success, false otherwise.
 */
bool i2c_builtin_start_r(uint8_t id)
{
	id |=  OAR1_ADD0_SET;
	i2c_builtin_start();

	i2c->DR = id;

	while (true)
	{
		uint32_t stat = get_status(i2c);

		if (stat == I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)
			break;

		if (stat & SR1_ERR_MASK)
		{
			LOG_ERR("r_error[%08lx]\n", stat & SR1_ERR_MASK);
			i2c->SR1 &= ~SR1_ERR_MASK;
			return false;
		}
	}

	return true;
}


/**
 * Send STOP condition.
 */
void i2c_builtin_stop(void)
{
	i2c->CR1 |= CR1_STOP_SET;
	i2c->CR1 &= CR1_PE_RESET;
}



bool i2c_builtin_put(const uint8_t data)
{

	return true;
}

int i2c_builtin_get(bool ack)
{

	return 0;
}

bool i2c_send(const void *_buf, size_t count)
{
	const uint8_t *buf = (const uint8_t *)_buf;

	i2c->DR = *buf++;
	count--;


	while (count)
	{
		ASSERT(buf);
		while( !(i2c->SR1 & BV(SR1_BTF)) );

		i2c->DR = *buf++;
		count--;

    }

	while (true)
	{
		uint32_t stat = get_status(i2c);

		if (stat == I2C_EVENT_MASTER_BYTE_TRANSMITTED)
			break;

		if (stat & SR1_ERR_MASK)
		{
			LOG_ERR("r_error[%08lx]\n", stat & SR1_ERR_MASK);
			i2c->SR1 &= ~SR1_ERR_MASK;
			return false;
		}
	}

	return true;
}


bool i2c_recv(void *_buf, size_t count)
{
	uint8_t *buf = (uint8_t *)_buf;

	while (count)
	{
		if (count == 1)
		{
			i2c->CR1 &= ~BV(CR1_POS);

			while (true)
			{
				uint32_t stat = get_status(i2c);

				if (stat == I2C_EVENT_MASTER_BYTE_RECEIVED)
					break;

				if (stat & SR1_ERR_MASK)
				{
					LOG_ERR("r_error[%08lx]\n", stat & SR1_ERR_MASK);
					i2c->SR1 &= ~SR1_ERR_MASK;
					return false;
				}
			}

			i2c->CR1 &= CR1_ACK_RESET;

			*buf++ = i2c->DR;
			count = 0;
		}
		else if (count == 2)
		{
			i2c->CR1 &= CR1_ACK_RESET;


			while (true)
			{

				if (i2c->SR1 & BV(SR1_BTF))
					break;

				uint32_t stat = get_status(i2c);
				if (stat & SR1_ERR_MASK)
				{
					LOG_ERR("r1_error[%08lx]\n", stat & SR1_ERR_MASK);
					i2c->SR1 &= ~SR1_ERR_MASK;
					return false;
				}
			}

			i2c->CR1 |= CR1_STOP_SET;

			*buf++ = i2c->DR;
			*buf++ = i2c->DR;
			count = 0;

			i2c->CR1 &= ~BV(CR1_POS);

		}
		else if (count == 3)
		{
			i2c->CR1 &= ~BV(CR1_POS);

			while (true)
			{

				if (i2c->SR1 & BV(SR1_BTF))
					break;

				uint32_t stat = get_status(i2c);
				if (stat & SR1_ERR_MASK)
				{
					LOG_ERR("r2_error[%08lx]\n", stat & SR1_ERR_MASK);
					i2c->SR1 &= ~SR1_ERR_MASK;
					return false;
				}
			}

			i2c->CR1 &= CR1_ACK_RESET;
			*buf++ = i2c->DR;

			i2c->CR1 |= CR1_STOP_SET;

			*buf++ = i2c->DR;

			while (true)
			{

				if (i2c->SR1 & BV(SR1_RXE))
					break;

				uint32_t stat = get_status(i2c);
				if (stat & SR1_ERR_MASK)
				{
					LOG_ERR("r3_error[%08lx]\n", stat & SR1_ERR_MASK);
					i2c->SR1 &= ~SR1_ERR_MASK;
					return false;
				}
			}

			*buf++ = i2c->DR;

			count = 0;
		}
		else
		{
			i2c->CR1 &= ~BV(CR1_POS);

			while( !(i2c->SR1 & BV(SR1_BTF)) );
			*buf++ = i2c->DR;
			count--;
		}
	}

	return true;
}

MOD_DEFINE(i2c);

/**
 * Initialize I2C module.
 */
void i2c_builtin_init(void)
{
	MOD_INIT(i2c);

	RCC->APB2ENR |= RCC_APB2_GPIOB;
	RCC->APB1ENR |= RCC_APB1_I2C1;

	stm32_gpioPinConfig((struct stm32_gpio *)GPIOB_BASE, GPIO_I2C1_SCL_PIN,
				GPIO_MODE_AF_OD, GPIO_SPEED_50MHZ);

	stm32_gpioPinConfig((struct stm32_gpio *)GPIOB_BASE, GPIO_I2C1_SDA_PIN,
				GPIO_MODE_AF_OD, GPIO_SPEED_50MHZ);

	i2c->CR1 = 0;
	i2c->CR2 = 0;
	i2c->CCR = 0;
	i2c->TRISE = 0;
	i2c->OAR1 = 0;

	i2c->CR2 |= CR2_FREQ_36MHZ;

	/* Configure spi in standard mode */
	#if CONFIG_I2C_FREQ <= 100000
		i2c->CCR |= (uint16_t)((CR2_FREQ_36MHZ * 1000000) / (CONFIG_I2C_FREQ << 1));
		i2c->TRISE |= (CR2_FREQ_36MHZ + 1);
	#else
		#error fast mode not supported
	#endif

}

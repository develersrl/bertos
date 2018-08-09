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
 * All Rights Reserved.
 * -->
 *
 * \brief STM32 SPI driver
 *
 * \author Daniele Basile <asterix@develer.com>
 */

#include "hw/hw_spi.h"
#include "cfg/cfg_spi.h"

#include <cfg/debug.h>
// Define logging setting (for cfg/log.h module).
#define LOG_LEVEL   SPI_LOG_LEVEL
#define LOG_FORMAT  SPI_LOG_FORMAT
#include <cfg/log.h>


#include <cpu/irq.h>
#include <cpu/types.h>
#include <cpu/power.h>

#include <io/stm32.h>

#include <drv/clock_stm32.h>
#include <drv/spi.h>
#include <drv/timer.h>



struct SpiHardware
{
	struct stm32_spi *base;
	uint8_t gpio_af_id;

	uint8_t mosi_gpio;
	uint8_t mosi_pin;

	uint8_t clk_pin;
	uint8_t clk_gpio;

	uint8_t miso_pin;
	uint8_t miso_gpio;
};

#define SPI_TIMEOUT_ERR   100
#define CHECK_FLAG(reg, flag, err, log)  \
	do { \
		ticks_t start = timer_clock(); \
		while(!((reg) & (flag))) \
		{ \
			if (timer_clock() - start > ms_to_ticks(SPI_TIMEOUT_ERR)) \
			{ \
				LOG_ERR("spi time out[%s]\n", (log)); \
				(err) |= SPI_SENDRECV_TIMEOUT; \
				break; \
			} \
			cpu_relax(); \
		} \
	} while(0)

static uint8_t stm32_sendRecv(Spi *spi, uint8_t c)
{
	spi->hw->base->DR = (uint8_t)c;
	CHECK_FLAG(spi->hw->base->SR, SPI_SR_TXE, spi->errors, "tx");
	CHECK_FLAG(spi->hw->base->SR, SPI_SR_RXNE,spi->errors, "rx");
	return (uint8_t)spi->hw->base->DR;
}

INLINE void stm32_cs_active(Spi *spi)
{
	(void)spi;
	SPI_HW_SS_ACTIVE();
}

INLINE void stm32_cs_inactive(Spi *spi)
{
	(void)spi;
	SPI_HW_SS_INACTIVE();
}

static const SpiVT spi_stm32_vt =
{
	.assertSS = stm32_cs_active,
	.deassertSS = stm32_cs_inactive,
	.sendRecv = stm32_sendRecv,
};

static struct SpiHardware spi_stm32_hw[] =
{
#if CPU_CM3_STM32F1
	{ /* SPI1 */
		.base = (struct stm32_spi *) SPI1_BASE,
		.clk_spi_en  = RCC_APB2_SPI1,
		.gpio_af_id = GPIO_MODE_AF_PP,
		.clk_gpio = 0,
		.clk_pin = 5,
		.mosi_gpio = 0,
		.mosi_pin = 7,
		.miso_gpio = 0,
		.miso_pin = 6,
	},
#elif CPU_CM3_STM32L1
	{ /* SPI1 */
		.base = (struct stm32_spi *)SPI1_BASE,
		.gpio_af_id = 5,
		.mosi_gpio = 0,
		.mosi_pin = 7,
		.miso_gpio = 0,
		.miso_pin = 6,
		.clk_gpio = 0,
		.clk_pin = 5,
	},
	{ /* SPI2 */
		.base = (struct stm32_spi *)SPI2_BASE,
		.gpio_af_id = 5,
		.mosi_gpio = 1,
		.mosi_pin = 15,
		.miso_gpio = 1,
		.miso_pin = 14,
		.clk_gpio = 1,
		.clk_pin = 13,
	},
	{ /* SPI3 */
		.base = (struct stm32_spi *)SPI3_BASE,
		.gpio_af_id = 6,
		.clk_gpio = 1,
		.clk_pin = 3,
		.mosi_gpio = 1,
		.mosi_pin = 5,
		.miso_gpio = 1,
		.miso_pin = 4,
	},
#else
	#error "Unknown CPU"
#endif
};

void spi_hw_init(Spi *spi, int dev, uint32_t clock)
{
	(void)clock;
	spi->hw = &spi_stm32_hw[dev];
	spi->vt = &spi_stm32_vt;

	SPI_HW_INIT();

	RCC_GPIO_ENABLE((struct RCC *)RCC_BASE, spi->hw->mosi_gpio);
	RCC_GPIO_ENABLE((struct RCC *)RCC_BASE, spi->hw->miso_gpio);
	RCC_GPIO_ENABLE((struct RCC *)RCC_BASE, spi->hw->clk_gpio);

	stm32_gpioPinConfig(GPIOx(spi->hw->clk_gpio), BV(spi->hw->clk_pin),
			GPIO_MODE_AF_PP | spi->hw->gpio_af_id, GPIO_SPEED_50MHZ);
	stm32_gpioPinConfig(GPIOx(spi->hw->miso_gpio), BV(spi->hw->miso_pin),
			GPIO_MODE_AF_PP | spi->hw->gpio_af_id, GPIO_SPEED_50MHZ);
	stm32_gpioPinConfig(GPIOx(spi->hw->mosi_gpio), BV(spi->hw->mosi_pin),
			GPIO_MODE_AF_PP | spi->hw->gpio_af_id, GPIO_SPEED_50MHZ);

#if CPU_CM3_STM32F1
	((struct RCC *)RCC_BASE)->APB2ENR |= spi->hw->clk_spi_en;

#elif CPU_CM3_STM32L1
	switch(dev)
	{
		case 0:
			((struct RCC *)RCC_BASE)->APB2ENR |= RCC_APB2ENR_SPI1EN;
			break;
		case 1:
			((struct RCC *)RCC_BASE)->APB1ENR |= RCC_APB1ENR_SPI2EN;
			break;
		case 2:
			((struct RCC *)RCC_BASE)->APB1ENR |= RCC_APB1ENR_SPI3EN;
			break;
	}

#else
	#error "Unknown CPU"
#endif


	spi->hw->base->CR1 = 0;
	spi->hw->base->CR2 = 0;
	spi->hw->base->I2SCFGR = 0;

	/*
	 * MSB,
	 * Baudrate=6MHz
	 * MASTER,
	 * spi en
	 */
#if CONFIG_SPI_DATAORDER == SPI_LSB_FIRST
	spi->hw->base->CR1 |= SPI_CR1_MSTR      | \
						 SPI_CR1_SSM       | \
						 SPI_CR1_SSI       | \
						 SPI_CR1_CLK_DIV16 | \
						 SPI_CR1_LSBFIRST | \
						 SPI_CR1_SPIEN;

#elif CONFIG_SPI_DATAORDER == SPI_MSB_FIRST
	spi->hw->base->CR1 |= SPI_CR1_MSTR      | \
						 SPI_CR1_SSM       | \
						 SPI_CR1_SSI       | \
						 SPI_CR1_CLK_DIV16;
#endif

	spi->hw->base->CR2 |= SPI_CR2_SSOE;
	spi->hw->base->CR1 |= SPI_CR1_SPIEN;
}


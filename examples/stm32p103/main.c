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
 * \brief STM32-P103 Cortex-M3 testcase
 *
 * \author Andrea Righi <arighi@develer.com>
 */

#include "cfg/compiler.h"
#include <drv/timer.h>
#include <drv/clock_stm32.h>
#include <drv/gpio_stm32.h>
#include <io/stm32_memmap.h>

#define LED_PIN                    (1 << 12)

int main(void)
{
	IRQ_ENABLE;
	timer_init();

	/* Enable clocking on RCC APB2 */
	RCC->AHBENR |= 1;
	/* Enable clocking on GPIOA and GPIOC */
	RCC->APB2ENR |= RCC_APB2_GPIOA | RCC_APB2_GPIOC;

	stm32_gpioPinConfig((struct stm32_gpio *)GPIOC_BASE,
			LED_PIN, GPIO_MODE_OUT_PP, GPIO_SPEED_50MHZ);
	while(1)
	{
		ticks_t clock = timer_clock_unlocked();

		if (clock & 0x10)
			stm32_gpioPinWrite((struct stm32_gpio *)GPIOC_BASE, LED_PIN, 1);
		else
			stm32_gpioPinWrite((struct stm32_gpio *)GPIOC_BASE, LED_PIN, 0);
	}
}

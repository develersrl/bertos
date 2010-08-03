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
 * All Rights Reserved.
 * -->
 *
 * \brief Led on/off macros for STM32-P103 board.
 *
 * \author Andrea Righi <arighi@develer.com>
 */

#ifndef HW_LED_H
#define HW_LED_H

#include <cfg/macros.h>

#include <io/stm32.h>

#include <drv/gpio_stm32.h>
#include <drv/clock_stm32.h>

#define LED_PIN			    BV(12)
#define LED_GPIO_BASE		((struct stm32_gpio *)GPIOC_BASE)

#define LED_ON()							\
		do {							\
			stm32_gpioPinWrite(LED_GPIO_BASE, LED_PIN, 1);	\
		} while (0)

#define LED_OFF()							\
		do {							\
			stm32_gpioPinWrite(LED_GPIO_BASE, LED_PIN, 0);	\
		} while (0)

#define LED_INIT()							\
		do {							\
			/* Enable clocking on GPIOA and GPIOC */	\
			RCC->APB2ENR |= RCC_APB2_GPIOC;			\
			/* Configure the LED pin as GPIO */		\
			stm32_gpioPinConfig(LED_GPIO_BASE,		\
					LED_PIN, GPIO_MODE_OUT_PP,	\
					GPIO_SPEED_50MHZ);		\
		} while(0)

#endif /* HW_LED_H */

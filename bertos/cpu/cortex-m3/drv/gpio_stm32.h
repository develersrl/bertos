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
 * \brief STM32 GPIO control interface.
 */

#ifndef GPIO_STM32_H
#define GPIO_STM32_H

#include <io/stm32.h>

#if CPU_CM3_STM32F207IG
enum
{
	STM32_GPIO_MODE_IN     = 0 << 4,
	STM32_GPIO_MODE_GP     = 1 << 4,
	STM32_GPIO_MODE_AF     = 2 << 4,
	STM32_GPIO_MODE_ANAL   = 3 << 4,

	STM32_GPIO_MODE_PP     = 0 << 6,
	STM32_GPIO_MODE_OD     = 1 << 6,

	STM32_GPIO_MODE_NOPUPD = 0 << 7,
	STM32_GPIO_MODE_PU     = 1 << 7,
	STM32_GPIO_MODE_PD     = 2 << 7,
};
#endif

/**
 * GPIO mode
 * \{
 */
enum
{
#if CPU_CM3_STM32F1
	GPIO_MODE_AIN = 0x0,
	GPIO_MODE_IN_FLOATING = 0x04,
	GPIO_MODE_IPD = 0x28,
	GPIO_MODE_IPU = 0x48,
	GPIO_MODE_OUT_OD = 0x14,
	GPIO_MODE_OUT_PP = 0x10,
	GPIO_MODE_AF_OD = 0x1C,
	GPIO_MODE_AF_PP = 0x18,
#else
	GPIO_MODE_AIN = STM32_GPIO_MODE_ANAL,
	GPIO_MODE_IN_FLOATING = STM32_GPIO_MODE_IN,
	GPIO_MODE_IPU = STM32_GPIO_MODE_IN | STM32_GPIO_MODE_PU,
	GPIO_MODE_OUT_PP = STM32_GPIO_MODE_GP | STM32_GPIO_MODE_PP,
	GPIO_MODE_OUT_OD = STM32_GPIO_MODE_GP | STM32_GPIO_MODE_OD,
	GPIO_MODE_AF_PP = STM32_GPIO_MODE_AF | STM32_GPIO_MODE_PP,
	GPIO_MODE_AF_OD = STM32_GPIO_MODE_AF | STM32_GPIO_MODE_OD,
#endif
};
/*\}*/


/**
 * GPIO speed
 *\{
 */
enum
{
#if CPU_CM3_STM32F1
	GPIO_SPEED_10MHZ = 1,
	GPIO_SPEED_2MHZ,
	GPIO_SPEED_50MHZ,
#else
	GPIO_SPEED_2MHZ = 0,
	GPIO_SPEED_25MHZ,
	GPIO_SPEED_50MHZ,
	GPIO_SPEED_100MHZ,
#endif
};
/*\}*/

/**
 * Write a value to the specified pin(s)
 *
 * \param base gpio register address
 * \param pins mask of pins that we want set or clear
 * \param val true to set selected pins of false to clear they.
 */
INLINE void stm32_gpioPinWrite(struct stm32_gpio *base, uint16_t pins, bool val)
{
	if (val)
		base->BSRR |= pins;
	else
		base->BRR  |= pins;
}

/**
 * Read a value from the specified pin(s)
 *
 * \param base gpio register address
 * \param pins mask of pins that we want read
 */
INLINE uint16_t stm32_gpioPinRead(struct stm32_gpio *base, uint16_t pins)
{
	return (base->IDR & pins);
}

/**
 * Initialize a GPIO peripheral configuration
 *
 * \param base gpio register address
 * \param pins mask of pins that we want to configure
 * \param mode select the behaviour of selected pins
 * \param speed clock frequency for selected gpio ports
 */
int stm32_gpioPinConfig(struct stm32_gpio *base, uint16_t pins, uint8_t mode, uint8_t speed);

#endif /* GPIO_STM32_H */

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
 * \brief LM3S1968 GPIO control interface.
 */

#ifndef GPIO_LM3S_H
#define GPIO_LM3S_H

#include <io/lm3s.h>

/**
 * GPIO mode
 */
/*\{*/
#define GPIO_DIR_MODE_IN        0x00000000  //< Pin is a GPIO input
#define GPIO_DIR_MODE_OUT       0x00000001  //< Pin is a GPIO output
#define GPIO_DIR_MODE_HW        0x00000002  //< Pin is a peripheral function
/*\}*/

/**
 * GPIO strenght
 */
/*\{*/
#define GPIO_STRENGTH_2MA       0x00000001  //< 2mA drive strength
#define GPIO_STRENGTH_4MA       0x00000002  //< 4mA drive strength
#define GPIO_STRENGTH_8MA       0x00000004  //< 8mA drive strength
#define GPIO_STRENGTH_8MA_SC    0x0000000C  //< 8mA drive with slew rate control
/*\}*/

/**
 * GPIO type
 */
/*\{*/
#define GPIO_PIN_TYPE_STD       0x00000008  //< Push-pull
#define GPIO_PIN_TYPE_STD_WPU   0x0000000A  //< Push-pull with weak pull-up
#define GPIO_PIN_TYPE_STD_WPD   0x0000000C  //< Push-pull with weak pull-down
#define GPIO_PIN_TYPE_OD        0x00000009  //< Open-drain
#define GPIO_PIN_TYPE_OD_WPU    0x0000000B  //< Open-drain with weak pull-up
#define GPIO_PIN_TYPE_OD_WPD    0x0000000D  //< Open-drain with weak pull-down
#define GPIO_PIN_TYPE_ANALOG    0x00000000  //< Analog comparator
/*\}*/

/* Write a value to the specified pin(s) */
INLINE void lm3s_gpio_pin_write(uint32_t port, uint8_t pins, uint8_t val)
{
	HWREG(port + (GPIO_O_DATA + (pins << 2))) = val;
}

int lm3s_gpio_pin_config(uint32_t port, uint8_t pins,
		uint32_t mode, uint32_t strength, uint32_t type);
void lm3s_gpio_pin_write(uint32_t port, uint8_t pins, uint8_t val);

#endif /* GPIO_LM3S_H */

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
 * \brief LM3S1968 keypad: hardware-specific definitions
 *
 * \author Andrea Righi <arighi@develer.com>
 */

#ifndef HW_KBD_H
#define HW_KBD_H

#include <cfg/macros.h> /* BV() */
#include <io/lm3s.h> /* GPIO_PORTG_BASE */
#include <drv/gpio_lm3s.h> /* lm3s_gpioPinConfig() / lm3s_gpioPinRead() */
#include "hw/kbd_map.h"

#define K_RPT_MASK (K_UP | K_DOWN | K_LEFT | K_RIGHT | K_OK)

#define KBD_HW_INIT					\
	do {						\
		lm3s_gpioPinConfig(GPIO_PORTG_BASE,	\
			K_RPT_MASK,			\
			GPIO_DIR_MODE_IN,		\
			GPIO_STRENGTH_2MA,		\
			GPIO_PIN_TYPE_STD_WPU);		\
	} while (0)

/**
 * Read the keyboard ports and return the mask of depressed keys.
 */
INLINE keymask_t kbd_readkeys(void)
{
	return ~lm3s_gpioPinRead(GPIO_PORTG_BASE,
		K_UP | K_DOWN | K_LEFT | K_RIGHT | K_OK);
}

#endif /* HW_KBD_H */

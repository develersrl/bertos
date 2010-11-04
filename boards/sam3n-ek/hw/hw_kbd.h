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
 * \brief SAM3N-EK keypad: hardware-specific definitions
 *
 * \author Stefano Fedrigo <aleph@develer.com>
 */

#ifndef HW_KBD_H
#define HW_KBD_H

#include <cfg/macros.h> /* BV() */
#include <io/sam3_pio.h>
#include "hw/kbd_map.h"

#define K_RPT_MASK (K_LEFT | K_RIGHT)

#define KBD_HW_INIT							\
	do {								\
		PIOA_IDR = BV(15) | BV(16);                             \
		PIOA_PUER = BV(15) | BV(16);                            \
		PIOA_ODR = BV(15) | BV(16);                             \
		PIOA_PER = BV(15) | BV(16);                             \
	} while (0)

/**
 * Read the keyboard ports and return the mask of depressed keys.
 */
INLINE keymask_t kbd_readkeys(void)
{
	keymask_t mask = 0;
	uint32_t reg = PIOA_PDSR;
	if (!(reg & BV(15)))
		mask |= K_LEFT;
	if (!(reg & BV(16)))
		mask |= K_RIGHT;
	return mask;
}

#endif /* HW_KBD_H */

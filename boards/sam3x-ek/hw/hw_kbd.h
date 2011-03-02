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
 * Copyright 2011 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief SAM3X-EK keypad: hardware-specific definitions
 *
 * \author Stefano Fedrigo <aleph@develer.com>
 */

#ifndef HW_KBD_H
#define HW_KBD_H

#include <cfg/macros.h> /* BV() */
#include <io/sam3_pio.h>
#include "hw/kbd_map.h"

#define KBD_LEFT_PIN   BV(7)   // Port E
#define KBD_RIGHT_PIN  BV(23)  // Port B

#define K_RPT_MASK (K_LEFT | K_RIGHT)

#define KBD_HW_INIT	\
	do { \
		PIOE_IDR =  KBD_LEFT_PIN; \
		PIOE_PUER = KBD_LEFT_PIN; \
		PIOE_ODR =  KBD_LEFT_PIN; \
		PIOE_PER =  KBD_LEFT_PIN; \
		PIOB_IDR =  KBD_RIGHT_PIN; \
		PIOB_PUER = KBD_RIGHT_PIN; \
		PIOB_ODR =  KBD_RIGHT_PIN; \
		PIOB_PER =  KBD_RIGHT_PIN; \
	} while (0)

/**
 * Read the keyboard ports and return the mask of depressed keys.
 */
INLINE keymask_t kbd_readkeys(void)
{
	keymask_t mask = 0;
	if (!(PIOE_PDSR & KBD_LEFT_PIN))
		mask |= K_LEFT;
	if (!(PIOB_PDSR & KBD_RIGHT_PIN))
		mask |= K_RIGHT;
	return mask;
}

#endif /* HW_KBD_H */

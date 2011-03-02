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
 * \brief Atmel SAM3X-EK LCD backlight control
 *
 * \author Stefano Fedrigo <aleph@develer.com>
 */

#ifndef HW_LCD_H
#define HW_LCD_H

#include <io/cm3.h>

#include <cpu/attr.h>
#include <cpu/types.h>
#include <drv/timer.h>

#define LCD_BACKLIGHT_MAX  32
#define LCD_BACKLIGHT_PIN  BV(27)    // Port B


INLINE void lcd_setBacklight(uint8_t level)
{
	if (level > LCD_BACKLIGHT_MAX)
		level = LCD_BACKLIGHT_MAX;

	PIOB_OER = LCD_BACKLIGHT_PIN;
	PIOB_SODR = LCD_BACKLIGHT_PIN;
	PIOB_PER = LCD_BACKLIGHT_PIN;

	// Switch off backlight first
	PIOB_CODR = LCD_BACKLIGHT_PIN;
	timer_delay(1);

	for (unsigned i = 0; i < level; i++)
	{
		PIOB_CODR = LCD_BACKLIGHT_PIN;
		NOP;NOP;NOP;NOP;NOP;
		PIOB_SODR = LCD_BACKLIGHT_PIN;
		NOP;NOP;NOP;NOP;NOP;
	}
}

#endif /* HW_LCD_H */

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
 * \brief Atmel SAM3N-EK testcase
 *
 * \author Luca Ottaviano <lottaviano@develer.com>
 * \author Daniele Basile <asterix@develer.com>
 */

#ifndef HW_LCD_H
#define HW_LCD_H

#include <io/cm3.h>

#include <cpu/attr.h>
#include <cpu/types.h>

#define LCD_BACKLIGHT_MAX  15
#define LCD_BACKLIGHT_PIN  BV(13)    // Port C
#define LCD_SPICLOCK       12000000  // Minimum cycle len = 80 ns according specs


#define LCD_BACKLIGHT_LEVEL_UP() \
	do \
	{ \
		PIOC_CODR = LCD_BACKLIGHT_PIN; \
		NOP;NOP;NOP;NOP;NOP; \
		PIOC_SODR = LCD_BACKLIGHT_PIN; \
		NOP;NOP;NOP;NOP;NOP; \
	} while(0)


INLINE void lcd_setBacklight(uint8_t level)
{
	if (level > LCD_BACKLIGHT_MAX)
		level = LCD_BACKLIGHT_MAX;

	for (uint8_t i = level; i <= LCD_BACKLIGHT_MAX; i++)
		LCD_BACKLIGHT_LEVEL_UP();
}

#define LCD_BACKLIGHT_INIT() \
	do { \
		PIOC_OER = LCD_BACKLIGHT_PIN; \
		PIOC_SODR = LCD_BACKLIGHT_PIN; \
		PIOC_PER = LCD_BACKLIGHT_PIN; \
	} while(0)

#endif /* HW_LCD_H */

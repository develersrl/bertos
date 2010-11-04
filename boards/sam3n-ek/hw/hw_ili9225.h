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
 * \brief ILI9225B low-level hardware macros for Atmel SAM3N-EK board.
 *
 * \author Stefano Fedrigo <aleph@develer.com>
 */

#ifndef HW_ILI9225_H
#define HW_ILI9225_H

#include "cfg/macros.h"   /* BV() */
#include <io/sam3_pio.h>


/**
 * \name LCD I/O pins/ports
 * @{
 */
#define LCD_CS_PIN         BV(30)
#define LCD_RESET_PIN      BV(29)
#define LCD_RS_PIN         BV(28)
/*@}*/

/**
 * \name LCD bus control macros
 * @{
 */
#define LCD_CS_HIGH()        do { PIOA_SODR = LCD_CS_PIN; } while (0)
#define LCD_CS_LOW()         do { PIOA_CODR = LCD_CS_PIN; } while (0)
#define LCD_RESET_HIGH()     do { PIOA_SODR = LCD_RESET_PIN; } while (0)
#define LCD_RESET_LOW()      do { PIOA_CODR = LCD_RESET_PIN; } while (0)
#define LCD_RS_HIGH()        do { PIOA_SODR = LCD_RS_PIN; } while (0)
#define LCD_RS_LOW()         do { PIOA_CODR = LCD_RS_PIN; } while (0)
/*@}*/

INLINE void lcd_ili9225_hw_bus_init(void)
{
	// Reset pin high
	PIOA_OER = LCD_RESET_PIN;
	LCD_RESET_HIGH();
	PIOA_PER = LCD_RESET_PIN;

	// RS pin high
	PIOA_OER = LCD_RS_PIN;
	LCD_RS_HIGH();
	PIOA_PER = LCD_RS_PIN;

	// Init chip select pin
	PIOA_OER = LCD_CS_PIN;
	LCD_CS_HIGH();
	PIOA_PER = LCD_CS_PIN;
}

#endif /* HW_ILI9225_H */

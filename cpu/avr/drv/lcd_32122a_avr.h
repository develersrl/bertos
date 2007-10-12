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
 * Copyright 2003, 2004, 2006 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2001 Bernardo Innocenti <bernie@codewiz.org>
 *
 * -->
 *
 * \version $Id$
 *
 * \author Bernardo Innocenti <bernie@develer.com>
 * \author Stefano Fedrigo <aleph@develer.com>
 *
 * \brief Displaytech 32122A LCD driver
 */

/*#*
 *#* $Log$
 *#* Revision 1.6  2006/07/19 12:56:25  bernie
 *#* Convert to new Doxygen style.
 *#*
 *#* Revision 1.5  2006/04/27 05:40:11  bernie
 *#* Naming convention fixes; Partial merge from project_grl.
 *#*
 *#* Revision 1.4  2006/02/15 09:13:16  bernie
 *#* Switch to BITMAP_FMT_PLANAR_V_LSB.
 *#*
 *#* Revision 1.3  2006/02/10 12:33:51  bernie
 *#* Make emulator display a bit larger.
 *#*
 *#* Revision 1.2  2006/01/17 22:59:48  bernie
 *#* Hardcode a different display size.
 *#*
 *#* Revision 1.1  2006/01/16 03:50:57  bernie
 *#* Import into DevLib.
 *#*/

#ifndef LCD_32122A_AVR_H
#define LCD_32122A_AVR_H

/* Predefined LCD PWM contrast values */
#define LCD_DEF_PWM 145
#define LCD_MAX_PWM 505
#define LCD_MIN_PWM 130

/* Display bitmap dims */
#define LCD_WIDTH 122
#define LCD_HEIGHT 32

/* fwd decl */
struct Bitmap;

extern struct Bitmap lcd_bitmap;

void lcd_init(void);
void lcd_setPwm(int duty);
void lcd_blitBitmap(struct Bitmap *bm);

#endif /* LCD_32122A_AVR_H */

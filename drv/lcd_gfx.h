/*!
 * \file
 * <!--
 * Copyright 2003, 2004, 2006 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2001 Bernardo Innocenti <bernie@codewiz.org>
 * This file is part of DevLib - See README.devlib for information.
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

#ifndef LCD_GFX_H
#define LCD_GFX_H

/* Predefined LCD PWM contrast values */
#define LCD_DEF_PWM 145
#define LCD_MAX_PWM 505
#define LCD_MIN_PWM 130

/* Display bitmap dims */
#define LCD_WIDTH  128
#define LCD_HEIGHT 64

/* fwd decl */
struct Bitmap;

extern struct Bitmap lcd_bitmap;

void lcd_init(void);
void lcd_setPwm(int duty);
void lcd_blitBitmap(struct Bitmap *bm);

#endif /* LCD_GFX_H */

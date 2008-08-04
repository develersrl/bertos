/**
 * \file
 * <!--
 * Copyright 2003, 2004, 2006 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2001 Bernie Innocenti <bernie@codewiz.org>
 * This file is part of DevLib - See README.devlib for information.
 * -->
 *
 * \version $Id: lcd_gfx.h,v 1.6 2006/07/19 12:56:25 bernie Exp $
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 * \author Stefano Fedrigo <aleph@develer.com>
 *
 * \brief Displaytech 32122A LCD driver
 */
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

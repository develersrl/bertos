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
 *#* Revision 1.1  2006/01/16 03:50:57  bernie
 *#* Import into DevLib.
 *#*
 *#* Revision 1.13  2005/11/27 22:41:08  bernie
 *#* Remove trailing whitespace.
 *#*
 *#* Revision 1.12  2004/08/29 21:46:11  bernie
 *#* CVSSILENT: Mark CVS log blocks.
 *#*
 *#* Revision 1.11  2004/06/11 12:40:11  customer_pw
 *#* Changes for new board_kf
 *#*
 *#* Revision 1.10  2004/03/24 15:37:03  bernie
 *#* Remove Copyright messages from Doxygen output
 *#*
 *#* Revision 1.9  2004/02/26 18:48:52  aleph
 *#* Remove generic setting function which live better in levels.c
 *#*
 *#* Revision 1.8  2004/02/11 11:14:35  aleph
 *#* Adeguate lcd_setcontrast to new leveledit callback interface
 *#*
 *#* Revision 1.7  2004/02/10 21:45:13  aleph
 *#* Values for contrast setting are logical ascending (1->10) instead of raw PWM (240->50)
 *#*
 *#* Revision 1.6  2004/02/09 15:52:09  aleph
 *#* Add reasonable min and max level #define for contrast
 *#*
 *#* Revision 1.5  2004/01/13 00:04:10  aleph
 *#* change lcd_clear() to static and minor var/define name enhancements
 *#*
 *#* Revision 1.4  2004/01/07 19:06:48  aleph
 *#* Some LCD bus optmizations and add bitmap rendering funcs
 *#*
 *#* Revision 1.3  2003/12/22 15:27:26  aleph
 *#* Add E2 chip select and some code cosmetic changes
 *#*
 *#* Revision 1.2  2003/12/19 00:20:17  aleph
 *#* Add contrast PWM
 *#*
 *#* Revision 1.1  2003/12/08 03:46:42  aleph
 *#* Add lcd driver
 *#*
 *#*/

#ifndef LCD_GFX_H
#define LCD_GFX_H

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
void lcd_setpwm(int duty);
void lcd_blit_bitmap(struct Bitmap *bm);

#endif // LCD_GFX_H

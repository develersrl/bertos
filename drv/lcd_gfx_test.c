/**
 * \file
 * <!--
 * Copyright 2006 Develer S.r.l. (http://www.develer.com/)
 * This file is part of DevLib - See README.devlib for information.
 * -->
 *
 * \version $Id$
 *
 * \author Bernardo Innocenti <bernie@develer.com>
 *
 * \brief dot-matrix LCD test.
 */

/*#*
 *#* $Log$
 *#* Revision 1.2  2006/01/17 02:31:57  bernie
 *#* Test gfx with the usual pentagons.
 *#*
 *#* Revision 1.1  2006/01/16 03:51:35  bernie
 *#* Add LCD Qt emulator.
 *#*
 *#*/

#include <emul/emul.h>
#include <drv/lcd_gfx.h>
#include <gfx/gfx.h>

static void magic(struct Bitmap *bitmap, coord_t x, coord_t y)
{
	static const coord_t coords[] = { 120, 34, 90, 90, 30, 90, 0, 34, 60, 0, 90, 90, 0, 34, 120, 34, 30, 90, 60, 0 };
	unsigned int i;

	gfx_moveTo(bitmap, coords[countof(coords)-2]/2 + x, coords[countof(coords)-1]/3 + y);
	for (i = 0; i < countof(coords); i += 2)
		gfx_lineTo(bitmap, coords[i]/2 + x, coords[i+1]/3 + y);
}

int main(int argc, char *argv[])
{
	emul_init(&argc, argv);
	lcd_init();

	coord_t x = 0, y = LCD_WIDTH / 2;
	coord_t xdir = +1, ydir = -1;

	for(;;)
	{
		gfx_bitmapClear(&lcd_bitmap);
		gfx_rectDraw(&lcd_bitmap, 10, 10, LCD_WIDTH-10, LCD_HEIGHT-10);
		gfx_setClipRect(&lcd_bitmap, 10, 10, LCD_WIDTH-10, LCD_HEIGHT-10);
		magic(&lcd_bitmap, x, y);

		x += xdir;
		y += ydir;
		if (x >= LCD_WIDTH)  xdir = -1;
		if (x <= -50)        xdir = +1;
		if (y >= LCD_HEIGHT) ydir = -1;
		if (y <= -50)        ydir = +1;

		lcd_blit_bitmap(&lcd_bitmap);
		emul_idle();
		usleep(100);
	}

	emul_cleanup();
	return 0;
}

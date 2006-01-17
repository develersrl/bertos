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
 *#* Revision 1.3  2006/01/17 23:00:26  bernie
 *#* Don't use hardcoded coordinates.
 *#*
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
	Bitmap *bm = &lcd_bitmap;

	for(;;)
	{
		gfx_bitmapClear(bm);
		gfx_setClipRect(bm, 0, 0, bm->width, bm->height);
		gfx_rectDraw(bm, 10, 10, bm->width-10, bm->height-10);
		gfx_setClipRect(bm, 11, 11, bm->width-11, bm->height-11);
		magic(bm, x, y);

		x += xdir;
		y += ydir;
		if (x >= bm->width)  xdir = -1;
		if (x <= -50)        xdir = +1;
		if (y >= bm->height) ydir = -1;
		if (y <= -50)        ydir = +1;

		lcd_blit_bitmap(bm);
		emul_idle();
		usleep(10000);
	}

	emul_cleanup();
	return 0;
}

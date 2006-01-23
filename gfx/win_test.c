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
 * \brief Windowing system test.
 */

/*#*
 *#* $Log$
 *#* Revision 1.1  2006/01/23 23:14:29  bernie
 *#* Implement simple, but impressive windowing system.
 *#*
 *#*/

#include <emul/emul.h>
#include <drv/lcd_gfx.h>
#include <gfx/gfx.h>
#include <gfx/win.h>

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

	const coord_t small_left = 45, small_top = 30, small_width = 50, small_height = 30;
	const coord_t large_left = -10, large_top = 10, large_width = 85, large_height = 41;

	Window root_win, small_win, large_win;
	Bitmap small_bm, large_bm;
	uint8_t small_raster[RASTER_SIZE(small_width, small_height)];
	uint8_t large_raster[RASTER_SIZE(large_width, large_height)];

	win_create(&root_win,  &lcd_bitmap);

	gfx_bitmapInit(&large_bm, large_raster, large_width, large_height);
	win_create(&large_win, &large_bm);
	win_open(&large_win, &root_win);
	win_move(&large_win, large_left, large_top);

	gfx_bitmapInit(&small_bm, small_raster, small_width, small_height);
	win_create(&small_win, &small_bm);
	win_open(&small_win, &root_win);
	win_move(&small_win, small_left, small_top);


	coord_t x = 0, y = LCD_WIDTH / 2;
	coord_t xdir = +1, ydir = -1;
	coord_t xdir_large = +1;
	coord_t ydir_small = +1;
	int raise_counter = 0;
	int i;
	Bitmap *bm;

	for(;;)
	{
		/* Background animation */
		bm = &lcd_bitmap;
		gfx_bitmapClear(bm);
/*		gfx_setClipRect(bm, 0, 0, bm->width, bm->height);
		gfx_rectDraw(bm, 10, 10, bm->width-10, bm->height-10);
		gfx_setClipRect(bm, 11, 11, bm->width-11, bm->height-11);
*/		magic(bm, x, y);
		x += xdir;
		y += ydir;
		if (x >= bm->width)  xdir = -1;
		if (x <= -50)        xdir = +1;
		if (y >= bm->height) ydir = -1;
		if (y <= -50)        ydir = +1;

		/* Large window animation */
		bm = large_win.bitmap;
		gfx_bitmapClear(bm);
		for (i = 0; i < bm->height / 2; i += 2)
			gfx_rectDraw(bm, 0 + i, 0 + i, bm->width - i, bm->height - i);


		/* Small window animation */
		bm = small_win.bitmap;
		gfx_bitmapClear(bm);
		gfx_rectDraw(bm, 0, 0, bm->width, bm->height);
		gfx_line(bm, 0, 0, bm->width, bm->height);
		gfx_line(bm, bm->width, 0, 0, bm->height);

		/* Move windows around */
		win_move(&large_win, large_win.geom.xmin + xdir_large, large_top);
		if (large_win.geom.xmin < -20) xdir_large = +1;
		if (large_win.geom.xmin > RECT_WIDTH(&root_win.geom) - 5) xdir_large = -1;

		win_move(&small_win, small_left, small_win.geom.ymin + ydir_small);
		if (small_win.geom.ymin < -20) ydir_small = +1;
		if (small_win.geom.ymin > RECT_HEIGHT(&root_win.geom) - 5) ydir_small = -1;

		++raise_counter;
		if (raise_counter % 997 == 0)
			win_raise(&small_win);
		else if (raise_counter % 731 == 0)
			win_raise(&large_win);

		win_compose(&root_win);
		lcd_blit_bitmap(root_win.bitmap);
		emul_idle();
		usleep(10000);
	}

	emul_cleanup();
	return 0;
}

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
 *#* Revision 1.2  2006/03/27 04:49:50  bernie
 *#* Add bouncing logo demo.
 *#*
 *#* Revision 1.1  2006/03/22 09:52:13  bernie
 *#* Add demo application.
 *#*
 *#* Revision 1.1  2006/01/23 23:14:29  bernie
 *#* Implement simple, but impressive windowing system.
 *#*
 *#*/

#include <emul/emul.h>
#include <kern/proc.h>
#include <drv/timer.h>
#include <drv/buzzer.h>
#include <drv/lcd_gfx.h>
#include <drv/kbd.h>
#include <gfx/gfx.h>
#include <gfx/win.h>
#include <gfx/text.h>
#include <gfx/font.h>
#include <icons/logo.h>
#include <mware/menu.h>
#include <cfg/macros.h>


/* SETTINGS SUBMENU ***************************/

static struct MenuItem settings_items[] =
{
	{ (const_iptr_t)"settings_0", 0, (MenuHook)0,  (iptr_t)0        },
	{ (const_iptr_t)"settings_1", 0, (MenuHook)0,  (iptr_t)0        },
	{ (const_iptr_t)"settings_2", 0, (MenuHook)0, (iptr_t)0      },
	{ (const_iptr_t)"settings_3", 0, (MenuHook)0,  (iptr_t)0   },
	{ (const_iptr_t)"settings_4", 0, (MenuHook)0,  (iptr_t)0 },
	{ (const_iptr_t)"settings_5", MIF_TOGGLE, (MenuHook)0, (iptr_t)0             },
	{ (const_iptr_t)0, 0, NULL, (iptr_t)0 }
};
static struct Menu settings_menu = { settings_items, "Settings Menu", MF_STICKY, &lcd_bitmap, 0 };

/*** DISPLAY MENU ****************************/

static struct MenuItem display_items[] =
{
	{ (const_iptr_t)"display_0", 0, (MenuHook)0,           (iptr_t)0 },
	{ (const_iptr_t)"display_1", 0, (MenuHook)0, (iptr_t)0     },
	{ (const_iptr_t)"display_2", 0, (MenuHook)0,   (iptr_t)0     },
	{ (const_iptr_t)"display_3", 0, (MenuHook)0,  (iptr_t)0 },
	{ (const_iptr_t)0, 0, NULL, (iptr_t)0 }
};
static struct Menu display_menu = { display_items, "Display Menu", MF_STICKY, &lcd_bitmap, 0 };


/*** SETUP MENU ******************************/

static struct MenuItem setup_items[] =
{
	{ (const_iptr_t)"Setup 0", 0, (MenuHook)NULL, (iptr_t)0    },
	{ (const_iptr_t)"Setup 1", 0, (MenuHook)NULL, (iptr_t)0    },
	{ (const_iptr_t)"Setup 2", 0, (MenuHook)NULL, (iptr_t)0    },
	{ (const_iptr_t)"Setup 3", 0, (MenuHook)NULL, (iptr_t)0    },
	{ (const_iptr_t)0, 0, NULL, NULL }
};
static struct Menu setup_menu = { setup_items, "Setup Menu", MF_STICKY, &lcd_bitmap, 0 };


/*** MAIN MENU *******************************/

static struct MenuItem main_items[] =
{
	{ (const_iptr_t)"Settings", 0, (MenuHook)menu_handle, (iptr_t)&settings_menu },
	{ (const_iptr_t)"Display", 0, (MenuHook)menu_handle, (iptr_t)&display_menu  },
	{ (const_iptr_t)"Setup",   0, (MenuHook)menu_handle, (iptr_t)&setup_menu    },
	{ (const_iptr_t)0, 0, NULL, (iptr_t)0 }
};
static struct Menu main_menu = { main_items, "Main Menu", MF_STICKY, &lcd_bitmap, 0 };

static void magic(struct Bitmap *bitmap, coord_t x, coord_t y)
{
	static const coord_t coords[] = { 120, 34, 90, 90, 30, 90, 0, 34, 60, 0, 90, 90, 0, 34, 120, 34, 30, 90, 60, 0 };
	unsigned int i;

	gfx_moveTo(bitmap, coords[countof(coords)-2]/2 + x, coords[countof(coords)-1]/3 + y);
	for (i = 0; i < countof(coords); i += 2)
		gfx_lineTo(bitmap, coords[i]/2 + x, coords[i+1]/3 + y);
}

Window root_win;

void schedule(void)
{
//	win_compose(&root_win);
	lcd_blit_bitmap(root_win.bitmap);
	emul_idle();
	usleep(10000);
}

void hello_world(void)
{
	gfx_bitmapClear(&lcd_bitmap);
	extern const Font font_10x20;
	extern const Font font_helvB10;
	extern const Font font_courB08;
	extern const Font font_courB14;
	extern const Font font_mono17;
	extern const Font font_luBS14;
	extern const Font font_ncenB18;
	gfx_setFont(&lcd_bitmap, &font_luBS14);

	text_xprintf(&lcd_bitmap, 1, 0, STYLEF_BOLD | TEXT_FILL | TEXT_CENTER, "Hello, world!");
	for (int i = 0; i < 1000; ++i)
	{
		lcd_blit_bitmap(&lcd_bitmap);
		emul_idle();
	}
}

/**
 * Show the splash screen
 */
void bouncing_logo(void)
{
	const long SPEED_SCALE = 1000;
	const long GRAVITY_ACCEL = 10;
	const long BOUNCE_ELASTICITY = 2;
	long h = (long)(-project_grl_logo.height) * SPEED_SCALE;
	long speed = 1000;

	/* Repeat until logo stands still on the bottom edge */
	while (!((speed == 0) && (h == 0)))
	{
		/* Move */
		h += speed;

		/* Gravity acceleration */
		speed += GRAVITY_ACCEL;

		if (h > 0 && speed > 0)
		{
			/* Bounce */
			speed = - (speed / BOUNCE_ELASTICITY);

		}

		/* Update graphics */
		gfx_bitmapClear(&lcd_bitmap);
		gfx_blitImage(&lcd_bitmap,
			(lcd_bitmap.width - project_grl_logo.width) / 2,
			h / SPEED_SCALE,
			&project_grl_logo);
		lcd_blit_bitmap(&lcd_bitmap);

		timer_delay(10);
	}
}

int main(int argc, char *argv[])
{
	emul_init(&argc, argv);
	timer_init();
	buz_init();
	kbd_init();
	lcd_init();
	proc_init();

	hello_world();
	bouncing_logo();

	const coord_t small_left = 45, small_top = 30, small_width = 50, small_height = 30;
	const coord_t large_left = -10, large_top = 10, large_width = 85, large_height = 41;

	Window small_win, large_win;
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

		menu_handle(&main_menu);

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

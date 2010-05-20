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
 * Copyright 2006, 2008 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 * \brief Multifunction system test for BeRTOS modules.
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 */

#include <cfg/macros.h>

#include <emul/emul.h>
#include <emul/kfile_posix.h>

#include <kern/irq.h>
#include <kern/proc.h>
#include <kern/monitor.h>
#include <kern/msg.h>

#include <drv/timer.h>
#include <drv/buzzer.h>
#include <drv/kbd.h>
#include <drv/lcd_gfx_qt.h>

#include <gfx/gfx.h>
#include <gfx/win.h>
#include <gfx/text.h>
#include <gfx/font.h>

#include <gui/menu.h>
#include <icons/logo.h>

/** Default LCD bitmap */
static Bitmap lcd_bitmap;

/**
 * Refresh the GUI.
 */
void schedule(void)
{
	lcd_gfx_qt_blitBitmap(&lcd_bitmap);
	emul_idle();
}

/**
 * Draw a pentacle in the provided bitmap.
 */
static void magic(struct Bitmap *bitmap, coord_t x, coord_t y)
{
	static const coord_t coords[] = { 120, 34, 90, 90, 30, 90, 0, 34, 60, 0, 90, 90, 0, 34, 120, 34, 30, 90, 60, 0 };
	unsigned int i;

	gfx_moveTo(bitmap, coords[countof(coords)-2]/2 + x, coords[countof(coords)-1]/3 + y);
	for (i = 0; i < countof(coords); i += 2)
		gfx_lineTo(bitmap, coords[i]/2 + x, coords[i+1]/3 + y);
}

static void hello_world(Bitmap *bm)
{
	extern const Font font_ncenB18;
	const Font *old_font;

	old_font = bm->font;

	gfx_bitmapClear(bm);

	/* Set big font */
	gfx_setFont(bm, &font_ncenB18);

	text_xprintf(bm, 0, 0, STYLEF_BOLD | TEXT_FILL | TEXT_CENTER,
			"Hello, world!");
	schedule();
	while (1)
		if (kbd_peek())
			break;

	/* Restore old font */
	gfx_setFont(bm, old_font);
}

/**
 * Show the splash screen
 */
static void bouncing_logo(Bitmap *bm)
{
	const long SPEED_SCALE = 1000;
	const long GRAVITY_ACCEL = 10;
	const long BOUNCE_ELASTICITY = 2;
	long h = (long)(-bertos_logo.height) * SPEED_SCALE;
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
		gfx_bitmapClear(bm);
		gfx_blitImage(bm,
			(bm->width - bertos_logo.width) / 2,
			h / SPEED_SCALE,
			&bertos_logo);
		schedule();
		timer_delay(10);
	}
}

void win_demo(Bitmap *bm)
{
	const coord_t small_left = 45, small_top = 30, small_width = 50, small_height = 30;
	const coord_t large_left = -10, large_top = 10, large_width = 85, large_height = 41;

	Window root_win, small_win, large_win;
	Bitmap small_bm, large_bm;
	uint8_t small_raster[RAST_SIZE(small_width, small_height)];
	uint8_t large_raster[RAST_SIZE(large_width, large_height)];

	win_create(&root_win, bm);

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

	for(;;)
	{
		/* Background animation */
		bm = root_win.bitmap;
		gfx_bitmapClear(bm);
		// gfx_setClipRect(bm, 0, 0, bm->width, bm->height);
		// gfx_rectDraw(bm, 10, 10, bm->width-10, bm->height-10);
		// gfx_setClipRect(bm, 11, 11, bm->width-11, bm->height-11);
		magic(bm, x, y);
		x += xdir;
		y += ydir;
		if (x >= bm->width)  xdir = -1;
		if (x <= -50)        xdir = +1;
		if (y >= bm->height) ydir = -1;
		if (y <= -50)        ydir = +1;

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
		/* Also does LCD refresh, etc. */
		if (kbd_peek())
			break;
	}
}

void proc_demo(void)
{
	proc_testRun();
}

void timer_demo(void)
{
	timer_testRun();
	timer_testTearDown();
}


/* SETTINGS SUBMENU */

static struct MenuItem settings_items[] =
{
	{ (const_iptr_t)"System",     0, (MenuHook)0,  (iptr_t)0 },
	{ (const_iptr_t)"Language",   0, (MenuHook)0,  (iptr_t)0 },
	{ (const_iptr_t)"Networking", 0, (MenuHook)0,  (iptr_t)0 },
	{ (const_iptr_t)"Date & Time",0, (MenuHook)0,  (iptr_t)0 },
	{ (const_iptr_t)"Power Saving", MIF_TOGGLE, (MenuHook)0, (iptr_t)0 },
	{ (const_iptr_t)0, 0, NULL, (iptr_t)0 }
};
static struct Menu settings_menu = { settings_items, "Settings Menu", MF_STICKY | MF_SAVESEL, &lcd_bitmap, 0, lcd_gfx_qt_blitBitmap };


/* MX SUBMENU */

static struct MenuItem mx_items[] =
{
	{ (const_iptr_t)"Mouse",                  MIF_CHECKIT | MIF_EXCLUDE_1 | MIF_EXCLUDE_2, (MenuHook)0,  (iptr_t)0 },
	{ (const_iptr_t)"Keyboard", MIF_CHECKED | MIF_CHECKIT | MIF_EXCLUDE_0 | MIF_EXCLUDE_2, (MenuHook)0,  (iptr_t)0 },
	{ (const_iptr_t)"Joystick", MIF_CHECKIT | MIF_EXCLUDE_0 | MIF_EXCLUDE_1, (MenuHook)0,  (iptr_t)0 },
	{ (const_iptr_t)"Autosave", MIF_CHECKED | MIF_CHECKIT | MIF_TOGGLE, (MenuHook)0,  (iptr_t)0 },
	{ (const_iptr_t)0, 0, NULL, (iptr_t)0 }
};

static struct Menu mx_menu = { mx_items, (const_iptr_t)0, MF_STICKY | MF_SAVESEL, &lcd_bitmap, 0, lcd_gfx_qt_blitBitmap };


/* DISPLAY SUBMENU */

static struct MenuItem display_items[] =
{
	{ (const_iptr_t)"Background", 0, (MenuHook)0, (iptr_t)0 },
	{ (const_iptr_t)"Colors",     0, (MenuHook)0, (iptr_t)0 },
	{ (const_iptr_t)"Style",      0, (MenuHook)0, (iptr_t)0 },
	{ (const_iptr_t)"Icon Theme", 0, (MenuHook)0, (iptr_t)0 },
	{ (const_iptr_t)0, 0, NULL, (iptr_t)0 }
};
static struct Menu display_menu = { display_items, "Display Menu", MF_SAVESEL, &lcd_bitmap, 0, lcd_gfx_qt_blitBitmap };


/* MAIN MENU */

static struct MenuItem main_items[] =
{
	{ (const_iptr_t)"Win Fly",     0, (MenuHook)win_demo,     (iptr_t)&lcd_bitmap    },
	{ (const_iptr_t)"Bounce!",     0, (MenuHook)bouncing_logo,(iptr_t)&lcd_bitmap    },
	{ (const_iptr_t)"Hello World", 0, (MenuHook)hello_world,  (iptr_t)&lcd_bitmap    },
	{ (const_iptr_t)"Scheduling",  0, (MenuHook)proc_demo,    (iptr_t)&lcd_bitmap    },
	{ (const_iptr_t)"Timer Test",  0, (MenuHook)timer_demo,   (iptr_t)&lcd_bitmap    },
	{ (const_iptr_t)"Menu MX",     0, (MenuHook)menu_handle,  (iptr_t)&mx_menu       },
	{ (const_iptr_t)"Display",     0, (MenuHook)menu_handle,  (iptr_t)&display_menu  },
	{ (const_iptr_t)"Settings",    0, (MenuHook)menu_handle,  (iptr_t)&settings_menu },
	{ (const_iptr_t)0, 0, NULL, (iptr_t)0 }
};
static struct Menu main_menu = { main_items, "Main Menu", MF_STICKY, &lcd_bitmap, 0, lcd_gfx_qt_blitBitmap };

#if CONFIG_KERN_HEAP
#define monitor_stack NULL
#else
static PROC_DEFINE_STACK(monitor_stack, KERN_MINSTACKSIZE);
#endif

int main(int argc, char *argv[])
{
	emul_init(&argc, argv);

	timer_init();
	buz_init();
	kbd_init();
	lcd_gfx_qt_init(&lcd_bitmap);
	proc_init();
	monitor_start(KERN_MINSTACKSIZE, monitor_stack);

	menu_handle(&main_menu);

	timer_cleanup();
	emul_cleanup();
	return 0;
}

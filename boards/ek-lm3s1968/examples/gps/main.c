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
 * \author Andrea Righi <arighi@develer.com>
 *
 * \brief DevelGPS demo application with gps.
 */

#include "hw/hw_led.h"

#include <cpu/irq.h>

#include <drv/lcd_rit128x96.h>
#include <drv/timer.h>
#include <drv/ser.h>
#include <drv/flash.h>
#include <drv/kbd.h>

#include <io/kblock.h>
#include <io/kfile_block.h>

#include <kern/proc.h>
#include <kern/sem.h>

#include <net/nmea.h>

#include <gfx/font.h>
#include <gfx/text.h>
#include <gfx/gfx_p.h> /* BM_PLOT() */
#include <gui/menu.h>
#include <math.h>

#include "compass.h"

/* OLED/GUI stuff */
#include "logo.c"
#define KEY_MASK (K_UP | K_DOWN | K_LEFT | K_RIGHT | K_OK)
extern Font font_gohu;
static uint8_t raster[RAST_SIZE(LCD_WIDTH, LCD_HEIGHT)];
static Bitmap lcd_bitmap;

#define SCRSVR_TIME 60000
static ticks_t scrsvr_timestamp;
static bool is_lcd_off;

/* Serial and NMEA stuff */
static Serial ser_port;
static nmeap_context_t nmea;
static NmeaGga gga;
static NmeaRmc rmc;
static NmeaVtg vtg;
static bool nmea_update;

static long prev_b;
static long lat, lon;
static long target_lat, target_lon;

/* Storage stuff */
#define GPS_POS_MAGIC 0xdeadbeef
static Flash flash_blk;
static KFileBlock flash;

struct SettingsData
{
	uint32_t magic;
	long target_lat, target_lon;
};

static void flash_load_target(void)
{
	struct SettingsData data;

	kfile_seek(&flash.fd, -sizeof(data), KSM_SEEK_END);
	kfile_read(&flash.fd, &data, sizeof(data));

	if (data.magic == GPS_POS_MAGIC)
	{
		target_lat = data.target_lat;
		target_lon = data.target_lon;
	}
}

static void flash_save_target(void)
{
	struct SettingsData data;

	data.magic = GPS_POS_MAGIC;
	data.target_lat = target_lat;
	data.target_lon = target_lon;

	kfile_seek(&flash.fd, -sizeof(data), KSM_SEEK_END);
	kfile_write(&flash.fd, &data, sizeof(data));
	kfile_flush(&flash.fd);
}


/* Display management */
INLINE void video_off(void)
{
	unsigned long delta =
		(long)ticks_to_ms(timer_clock_unlocked()) -
		(long)scrsvr_timestamp;

	if (!is_lcd_off && delta > SCRSVR_TIME)
	{
		rit128x96_off();
		is_lcd_off = true;
	}
}

INLINE void video_on(void)
{
	if (is_lcd_off)
	{
		rit128x96_on();
		is_lcd_off = false;
	}
	scrsvr_timestamp = ticks_to_ms(timer_clock_unlocked());
}

INLINE void repaint(Bitmap *bm)
{
	rit128x96_blitBitmap(bm);
}

INLINE keymask_t keypad_peek(void)
{
	keymask_t key = kbd_peek();

	if (key & KEY_MASK)
	{
		if (is_lcd_off)
			key = 0;
		video_on();
	}
	return key;
}

/* Status LED thread */
static void NORETURN led_process(void)
{
	while (1)
	{
		video_off();
		if (!nmea_update)
		{
			timer_delay(1000);
			continue;
		}
		LED_ON();
		timer_delay(100);
		LED_OFF();
		nmea_update = false;
	}
}

/* NMEA parser */
static void nmea_callback(nmeap_context_t *context, void *data, void *user_data)
{
	(void)context;
	(void)data;
	(void)user_data;

	lat = (long)gga.latitude;
	lon = (long)gga.longitude;

	nmea_update = true;
}

static void NORETURN ser_process(void)
{
	while (1)
	{
		nmea_poll(&nmea, &ser_port.fd);
		kfile_clearerr(&ser_port.fd);
	}
}

/* Target position screen */
static void target(Bitmap *bm)
{
	const long STEP = 10000000,
		MAX_LAT = 90000000, MIN_LAT = -90000000,
		MAX_LON = 180000000, MIN_LON = -180000000;
	long step = STEP, target;
	int row = 0, pos = 1;
	keymask_t key;

	gfx_bitmapClear(bm);
	video_on();
	text_xprintf(bm, 0, 0,
		STYLEF_UNDERLINE | TEXT_CENTER | TEXT_FILL,
		"Target position");
	while (1)
	{
		if (!is_lcd_off)
		{
			text_xprintf(bm, 3, 0, TEXT_FILL,
					"Lat:  %02ld.%06ld %c %s",
					ABS(target_lat) / 1000000L,
					ABS(target_lat) % 1000000,
					target_lat >= 0 ? 'N' : 'S',
					row == 0 ? "<-" : "");
			text_xprintf(bm, row ? 4 : 6, 0,
					TEXT_FILL | TEXT_CENTER, " ");
			text_xprintf(bm, row ? 6 : 4, 0,
					TEXT_FILL, "%*c",
					(step < 1000000L) ?
						pos + 7 : pos + 6, '^');
			text_xprintf(bm, 5, 0, TEXT_FILL,
					"Lon: %03ld.%06ld %c %s",
					ABS(target_lon) / 1000000L,
					ABS(target_lon) % 1000000L,
					target_lon >= 0 ? 'E' : 'W',
					row == 1 ? "<-" : "");
			repaint(bm);
		}
		key = keypad_peek();
		if (key & K_UP)
		{
			if (row == 0)
			{
				target = target_lat + step;
				if (target <= MAX_LAT)
				{
					if (target_lat < 0 && target > 0)
						target_lat = ABS(target_lat) +
								step;
					else
						target_lat = target;
				}
			}
			else
			{
				target = target_lon + step;
				if (target <= MAX_LON)
				{
					if (target_lon < 0 && target > 0)
						target_lon = ABS(target_lon) +
								step;
					else
						target_lon = target;
				}
			}
		}
		else if (key & K_DOWN)
		{
			if (row == 0)
			{
				target = target_lat - step;
				if (target >= MIN_LAT)
				{
					if (target_lat > 0 && target < 0)
						target_lat = -ABS(target_lat) -
								step;
					else
						target_lat = target;
				}
			}
			else
			{
				target = target_lon - step;
				if (target >= MIN_LON)
				{
					if (target_lon > 0 && target < 0)
						target_lon = -ABS(target_lon) -
								step;
					else
						target_lon = target;
				}
			}
		}
		else if (key & K_LEFT)
		{
			if (step < STEP)
			{
				step *= 10;
				pos--;
			}
		}
		else if (key & K_RIGHT)
		{
			if (step >= 10)
			{
				step /= 10;
				pos++;
			}
		}
		else if (key & K_OK)
		{
			if (row++ == 1)
				break;
			 /* Move to longigude */
		}
		cpu_relax();
	}
	flash_save_target();
}

/* Compass management */
static void draw_compass(Bitmap *bm)
{
	const int R = LCD_HEIGHT / 3, R_SMALL = 10;
	long x, y, x1, y1, x2, y2, d;
	int i;

	d = distance(lat / 1E6, lon / 1E6, target_lat / 1E6, target_lon / 1E6);

	x = (long)((float)R * (1 + sin(deg2rad((float)prev_b))));
	y = (long)((float)R * (1 - cos(deg2rad((float)prev_b))));

	x1 = R - R_SMALL + (long)((float)R_SMALL *
		(1 + sin(deg2rad((float)prev_b - 120.0))));
	y1 = R - R_SMALL + (long)((float)R_SMALL *
		(1 - cos(deg2rad((float)prev_b - 120.0))));
	x2 = R - R_SMALL + (long)((float)R_SMALL *
		(1 + sin(deg2rad((float)prev_b + 120.0))));
	y2 = R - R_SMALL + (long)((float)R_SMALL *
		(1 - cos(deg2rad((float)prev_b + 120.0))));

	gfx_bitmapClear(bm);
	/* Print direction heading and degrees */
	text_xprintf(bm, 0, 5, 0, "%s", "N");
	text_xprintf(bm, 0, 0, TEXT_RIGHT, "%s", compass_heading(prev_b));
	text_xprintf(bm, 1, 0, TEXT_RIGHT, "%ld deg.", prev_b);
	/* Print distance */
	text_xprintf(bm, 2, 0, TEXT_RIGHT, "%ld %s",
			d >= 1000 ? d / 1000 : d,
			d >= 1000 ? "Km" : "m");
	/* Print current and target position */
	text_xprintf(bm, 6, 0, TEXT_FILL, "%02ld.%06ld%c%03ld.%06ld%c",
			ABS(lat) / 1000000L,
			ABS(lat) % 1000000,
			lat >= 0 ? 'N' : 'S',
			ABS(lon) / 1000000L,
			ABS(lon) % 1000000L,
			lon >= 0 ? 'E' : 'W');
	text_xprintf(bm, 7, 0, TEXT_FILL, "%02ld.%06ld%c%03ld.%06ld%c",
			ABS(target_lat) / 1000000L,
			ABS(target_lat) % 1000000,
			target_lat >= 0 ? 'N' : 'S',
			ABS(target_lon) / 1000000L,
			ABS(target_lon) % 1000000L,
			target_lon >= 0 ? 'E' : 'W');
	/* Draw the circle */
	for (i = 0; i < 360; i++)
		BM_PLOT(bm,
			(long)((float)R * (1 + sin(deg2rad((float)i)))),
			(long)((float)R * (1 - cos(deg2rad((float)i)))));
	/* Draw the needle */
	gfx_rectFill(bm, R - 2, R - 2, R + 2, R + 2);
	gfx_line(bm, R, R, x1, y1);
	gfx_line(bm, R, R, x2, y2);
	gfx_line(bm, x1, y1, x, y);
	gfx_line(bm, x2, y2, x, y);

	repaint(bm);
}

static void compass(Bitmap *bm)
{
	long b, inc;

	video_on();
	while (1)
	{
		if (!is_lcd_off)
		{
			b = bearing(lat / 1E6, lon / 1E6,
					target_lat / 1E6, target_lon / 1E6);
			inc = ABS(b - prev_b) < 360 - ABS(b - prev_b) ?  1 : -1;
			/* Compass animation */
			if (b < prev_b)
				inc = -inc;
			while (prev_b != b)
			{
				prev_b = (prev_b + inc) % 360;
				if (prev_b < 0)
					prev_b = 359;
				if (!(prev_b % 5))
					draw_compass(bm);
				if (keypad_peek() & KEY_MASK)
					return;
			}
			draw_compass(bm);
		}
		cpu_relax();
		if (keypad_peek() & KEY_MASK)
			return;
	}
}

/* GPS receiver status */
static const char *gps_fix[] =
{
	"invalid",
	"GPS",
	"DGPS",
	"PPS",
	"RTK",
	"float-RTK",
	"estimated",
	"manual",
	"simulation",
};

static void gps_data(Bitmap *bm)
{
	struct tm tm;
	char buf[32];

	video_on();
	gfx_bitmapClear(bm);
	while (1)
	{
		if (!is_lcd_off)
		{
			if (!rmc.time)
			{
				text_xprintf(bm, 3, 0,
					TEXT_CENTER | TEXT_FILL, "No GPS data");
			}
			else
			{
				gmtime_r(&rmc.time, &tm);

				text_xprintf(bm, 0, 0, TEXT_FILL,
						"Lat. %ld.%06ld%c",
						ABS(lat) / 1000000L,
						ABS(lat) % 1000000,
						lat >= 0 ? 'N' : 'S');
				text_xprintf(bm, 1, 0, TEXT_FILL,
						"Lon. %ld.%06ld%c",
						ABS(lon) / 1000000L,
						ABS(lon) % 1000000L,
						lon >= 0 ? 'E' : 'W');
				text_xprintf(bm, 2, 0, TEXT_FILL,
						"Alt. %ld", gga.altitude);
				text_xprintf(bm, 3, 0, TEXT_FILL,
						"Speed: %d", vtg.km_speed);
				if (gga.quality < countof(gps_fix))
					text_xprintf(bm, 4, 0, TEXT_FILL,
							"Fix: %s",
							gps_fix[gga.quality]);
				else
					text_xprintf(bm, 4, 0, TEXT_FILL,
							"Fix: %d", gga.quality);
				text_xprintf(bm, 5, 0, TEXT_FILL,
						"Satellites: %d",
						gga.satellites);
				strftime(buf, sizeof(buf),
						"Date: %Y-%m-%d %a", &tm);
				text_xprintf(bm, 6, 0, TEXT_FILL, "%s", buf);
				strftime(buf, sizeof(buf),
						"Time: %H:%M:%S (UTC)", &tm);
				text_xprintf(bm, 7, 0, TEXT_FILL, "%s", buf);
			}
			repaint(bm);
		}
		if (keypad_peek() & KEY_MASK)
			break;
		cpu_relax();
	}
}

/* BeRTOS screen */
static void about(Bitmap *bm)
{
	gfx_bitmapClear(bm);
	video_on();
	text_xprintf(bm, 7, 0,
		STYLEF_UNDERLINE | TEXT_CENTER | TEXT_FILL,
		"http://www.bertos.org");
	repaint(bm);
	if (!is_lcd_off)
	{
		const uint8_t *p = &logo[BITMAP_HEADER_SIZE];
		uint8_t h = logo[BITMAP_HEIGHT_OFFSET];
		uint8_t w = logo[BITMAP_WIDTH_OFFSET];
		uint8_t r;

		for (r = 0; r < h; r++)
		{
			rit128x96_blitRaw(p,
				(128 - w) / 2, 70 - r, w, 1);
			p += w / 2;
		}
	}
	while (!(keypad_peek() & KEY_MASK))
		cpu_relax();
}

static struct MenuItem main_items[] =
{
	{(const_iptr_t)"Target", 0, (MenuHook)target, (iptr_t)&lcd_bitmap},
	{(const_iptr_t)"Compass", 0, (MenuHook)compass, (iptr_t)&lcd_bitmap},
	{(const_iptr_t)"GPS data", 0, (MenuHook)gps_data, (iptr_t)&lcd_bitmap},
	{(const_iptr_t)"About...", 0, (MenuHook)about, (iptr_t)&lcd_bitmap},
	{(const_iptr_t)0, 0, NULL, (iptr_t)NULL}
};

static struct Menu main_menu =
{
	main_items, "DevelGPS v0.1", MF_STICKY | MF_SAVESEL, &lcd_bitmap, 0, rit128x96_blitBitmap
};

static void init(void)
{
	IRQ_ENABLE;

	kdbg_init();
	timer_init();
	proc_init();

	scrsvr_timestamp = ticks_to_ms(timer_clock_unlocked());
	LED_INIT();

	flash_init(&flash_blk, 0);
	kfileblock_init(&flash, &flash_blk.blk);
	flash_load_target();

	ser_init(&ser_port, SER_UART1);
	ser_setbaudrate(&ser_port, 38400);

	nmeap_init(&nmea, NULL);
	nmeap_addParser(&nmea, "GPGGA", nmea_gpgga, nmea_callback, &gga);
	nmeap_addParser(&nmea, "GPRMC", nmea_gprmc, nmea_callback, &rmc);
	nmeap_addParser(&nmea, "GPVTG", nmea_gpvtg, nmea_callback, &vtg);

	rit128x96_init();
	gfx_bitmapInit(&lcd_bitmap, raster, LCD_WIDTH, LCD_HEIGHT);
        gfx_setFont(&lcd_bitmap, &font_gohu);
	repaint(&lcd_bitmap);

	kbd_init();
}

int main(void)
{
	init();

	proc_new(led_process, NULL, KERN_MINSTACKSIZE * 2, NULL);
	proc_new(ser_process, NULL, KERN_MINSTACKSIZE * 2, NULL);

	while (1)
	{
		iptr_t res = menu_handle(&main_menu);
		if (res != MENU_TIMEOUT)
			video_on();
		cpu_relax();
	}
}

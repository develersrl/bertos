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
 * \brief LM3S8962 Cortex-M3 testcase
 *
 * \author Andrea Righi <arighi@develer.com>
 */

#include <cpu/irq.h>
#include <drv/timer.h>
#include <drv/ser.h>
#include <drv/kbd.h>
#include <drv/lcd_rit128x96.h>
#include <gfx/gfx.h>
#include <gfx/font.h>
#include <gfx/text.h>
#include <gui/menu.h>
#include <icons/logo.h>
#include <stdio.h>

#include "cfg/compiler.h"
#include "cfg/cfg_gfx.h"

#include "hw/hw_rit128x96.h"

#define PROC_STACK_SIZE	KERN_MINSTACKSIZE * 2

#if CONFIG_KERN_HEAP
#define hp_stack NULL
#define lp_stack NULL
#define ser_stack NULL
#define led_stack NULL
#else
static PROC_DEFINE_STACK(hp_stack, PROC_STACK_SIZE);
static PROC_DEFINE_STACK(lp_stack, PROC_STACK_SIZE);
static PROC_DEFINE_STACK(ser_stack, PROC_STACK_SIZE);
static PROC_DEFINE_STACK(led_stack, PROC_STACK_SIZE);
#endif

extern Font font_gohu;
static uint8_t raster[RAST_SIZE(LCD_WIDTH, LCD_HEIGHT)];
static Bitmap lcd_bitmap;

#define KEY_MASK (K_UP | K_DOWN | K_LEFT | K_RIGHT | K_OK)

static Process *hp_proc, *lp_proc, *led_proc;
static hptime_t start, end;

static Serial ser_port;

static void led_init(void)
{
	/* Enable the GPIO port that is used for the on-board LED */
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOF;
	/*
	 * Perform a dummy read to insert a few cycles delay before enabling
	 * the peripheral.
	 */
	(void)SYSCTL_RCGC2_R;
	/* Enable the GPIO pin for the LED */
	GPIO_PORTF_DIR_R = 0x01;
	GPIO_PORTF_DEN_R = 0x01;
}

INLINE void led_on(void)
{
	GPIO_PORTF_DATA_R |= 0x01;
}

INLINE void led_off(void)
{
	GPIO_PORTF_DATA_R &= ~0x01;
}

static bool led_blinking;

static void NORETURN led_process(void)
{
	int i;

	for (i = 0; ; i++)
	{
		if (!led_blinking)
		{
			led_off();
			sig_wait(SIG_USER0);
		}
		if (i & 1)
			led_on();
		else
			led_off();
		timer_delay(50);
	}
}

static void led_test(UNUSED_ARG(Bitmap *, bm))
{
	led_blinking = !led_blinking;
	sig_send(led_proc, SIG_USER0);
}

static void bouncing_logo(Bitmap *bm)
{
	const long SPEED_SCALE = 1000;
	const long GRAVITY_ACCEL = 100;
	const long BOUNCE_ELASTICITY = 1;
	long h = (long)(-bertos_logo.height) * SPEED_SCALE;
	long speed = 0, i;

	for (i = 0; ; i++)
	{
		/* Move */
		h += speed;

		/* Gravity acceleration */
		speed += GRAVITY_ACCEL;

		if (h > 0 && speed > 0)
		{
			/* Bounce */
			speed = -(speed / BOUNCE_ELASTICITY);

		}
		/* Update graphics */
		gfx_bitmapClear(bm);
		gfx_blitImage(bm,
			(LCD_WIDTH - bertos_logo.width) / 2,
			(LCD_HEIGHT - bertos_logo.height) / 2 + h / SPEED_SCALE,
			&bertos_logo);
		text_xprintf(bm, 7, 0, TEXT_FILL | TEXT_CENTER, "Press SELECT to quit");
		rit128x96_blitBitmap(bm);
		timer_delay(15);
		if (kbd_peek() & KEY_MASK)
			break;
	}
}

static void screen_saver(Bitmap *bm)
{
	int x1, y1, x2, y2;
	int i;

	for (i = 0; ; i++)
	{
		x1 = i % LCD_WIDTH;
		y1 = i % LCD_HEIGHT;

		x2 = LCD_WIDTH - i % LCD_WIDTH;
		y2 = LCD_HEIGHT - i % LCD_HEIGHT;

		gfx_bitmapClear(bm);
		gfx_rectDraw(bm, x1, y1, x2, y2);
		rit128x96_blitBitmap(bm);
		if (kbd_peek() & KEY_MASK)
			break;
	}
}

INLINE hptime_t get_hp_ticks(void)
{
	return (TIMER_HW_CNT - timer_hw_hpread()) +
			timer_clock_unlocked() * TIMER_HW_CNT;
}

static void NORETURN hp_process(void)
{
	while (1)
	{
		sig_wait(SIG_USER0);
		end = get_hp_ticks();
		timer_delay(100);
		sig_send(lp_proc, SIG_USER0);
	}
}

static void NORETURN lp_process(void)
{
	while (1)
	{
		start = get_hp_ticks();
		sig_send(hp_proc, SIG_USER0);
		sig_wait(SIG_USER0);
	}
}

static void res_process(void)
{
	const char spinner[] = {'/', '-', '\\', '|'};
	int i;
	char c;

	for (i = 0; ; i++)
	{
		/* Show context switch (in clock cycles) */
		c = spinner[i % countof(spinner)];
		text_xprintf(&lcd_bitmap, 3, 0, TEXT_CENTER | TEXT_FILL, "%c Context switch %c", c, c);
		text_xprintf(&lcd_bitmap, 5, 0, TEXT_FILL, " %lu clock cycles", end - start);
		/* Show context switch (in usec) */
		text_xprintf(&lcd_bitmap, 6, 0, TEXT_FILL,
			" %lu.%lu usec",
				((end - start) * 1000000) / CPU_FREQ,
				((end - start) * (100000000 / CPU_FREQ)) % 100);
		rit128x96_blitBitmap(&lcd_bitmap);
		timer_delay(5);
		if (kbd_peek() & KEY_MASK)
			break;
	}
}

static void context_switch_test(Bitmap *bm)
{
	gfx_bitmapClear(bm);
	text_xprintf(bm, 0, 0, TEXT_FILL,
			"CPU: Cortex-M3 %luMHz", CPU_FREQ / 1000000);
	rit128x96_blitBitmap(bm);
	text_xprintf(bm, 1, 0, TEXT_FILL, "Board: LM3S8962 EVB");
	rit128x96_blitBitmap(bm);

	res_process();
}

static void uptime(Bitmap *bm)
{
	extern const Font font_luBS14;
	const Font *old_font;

	old_font = bm->font;

	/* Set big font */
	gfx_bitmapClear(bm);
	gfx_setFont(bm, &font_luBS14);
	text_xprintf(bm, 0, 0, TEXT_FILL | TEXT_CENTER, "Uptime");
	while (1)
	{
		ticks_t clock = ticks_to_ms(timer_clock_unlocked());

		/* Display uptime (in ticks) */
		text_xprintf(&lcd_bitmap, 2, 0, TEXT_FILL | TEXT_CENTER,
				"%lu", clock / 1000);
		rit128x96_blitBitmap(bm);
		timer_delay(5);
		if (kbd_peek() & KEY_MASK)
			break;
	}
	gfx_setFont(bm, old_font);
}

static void NORETURN soft_reset(Bitmap * bm)
{
	extern const Font font_luBS14;
	int i;

	/* Set big font */
	gfx_bitmapClear(bm);
	gfx_setFont(bm, &font_luBS14);
	for (i = 5; i; --i)
	{
		text_xprintf(bm, 2, 0, TEXT_FILL | TEXT_CENTER, "%d", i);
		rit128x96_blitBitmap(bm);
		timer_delay(1000);
	}
	text_xprintf(bm, 2, 0, TEXT_FILL | TEXT_CENTER, "REBOOT");
	rit128x96_blitBitmap(bm);
	timer_delay(1000);

	/* Perform a software reset request */
	HWREG(NVIC_APINT) = NVIC_APINT_VECTKEY | NVIC_APINT_SYSRESETREQ;
	UNREACHABLE();
}

static void NORETURN ser_process(void)
{
	char buf[32];
	int i;

	ser_init(&ser_port, SER_UART0);
	ser_setbaudrate(&ser_port, 115200);

	/* BeRTOS terminal */
	for (i = 0; ; i++)
	{
		kfile_printf(&ser_port.fd, "\n\r[%03d] BeRTOS:~$ ", i);
		kfile_gets_echo(&ser_port.fd, buf, sizeof(buf), true);
		kfile_printf(&ser_port.fd, "%s", buf);
	}
}

static struct MenuItem main_items[] =
{
	{ (const_iptr_t)"LED blinking", 0, (MenuHook)led_test, (iptr_t)&lcd_bitmap },
	{ (const_iptr_t)"Bouncing logo", 0, (MenuHook)bouncing_logo, (iptr_t)&lcd_bitmap },
	{ (const_iptr_t)"Screen saver demo", 0, (MenuHook)screen_saver, (iptr_t)&lcd_bitmap },
	{ (const_iptr_t)"Scheduling test", 0, (MenuHook)context_switch_test, (iptr_t)&lcd_bitmap },
	{ (const_iptr_t)"Show uptime", 0, (MenuHook)uptime, (iptr_t)&lcd_bitmap },
	{ (const_iptr_t)"Reboot", 0, (MenuHook)soft_reset, (iptr_t)&lcd_bitmap },
	{ (const_iptr_t)0, 0, NULL, (iptr_t)0 }
};
static struct Menu main_menu = { main_items, "BeRTOS", MF_STICKY | MF_SAVESEL, &lcd_bitmap, 0, rit128x96_blitBitmap };

int main(void)
{
	IRQ_ENABLE;
	kdbg_init();

	kputs("Init LED..");
	led_init();
	kputs("Done.\n");
	kputs("Init Timer..");
	timer_init();
	kputs("Done.\n");
	kputs("Init Process..");
	proc_init();
	kputs("Done.\n");

	kputs("Init OLED display..");
	rit128x96_init();
	gfx_bitmapInit(&lcd_bitmap, raster, LCD_WIDTH, LCD_HEIGHT);
	gfx_setFont(&lcd_bitmap, &font_gohu);
	rit128x96_blitBitmap(&lcd_bitmap);
	kputs("Done.\n");
	kputs("Init Keypad..");
	kbd_init();
	kputs("Done.\n");
	hp_proc = proc_new(hp_process, NULL, PROC_STACK_SIZE, hp_stack);
	lp_proc = proc_new(lp_process, NULL, PROC_STACK_SIZE, lp_stack);
	led_proc = proc_new(led_process, NULL, PROC_STACK_SIZE, led_stack);
	/* Open a dummy echo terminal on UART0 */
	proc_new(ser_process, NULL, PROC_STACK_SIZE, ser_stack);

	proc_setPri(hp_proc, 2);
	proc_setPri(lp_proc, 1);
	while (1)
	{
		menu_handle(&main_menu);
		cpu_relax();
	}
}

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
 * \brief LM3S1968 Cortex-M3 testcase
 *
 * \author Andrea Righi <arighi@develer.com>
 */

#include <cpu/irq.h>
#include <drv/timer.h>
#include <drv/ser.h>
#include <drv/lcd_rit128x96.h>
#include <gfx/gfx.h>
#include <gfx/font.h>
#include <gfx/text.h>
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

extern Font font_helvB10;
static uint8_t raster[RAST_SIZE(LCD_WIDTH, LCD_HEIGHT)];
static Bitmap bm;

static Process *hp_proc, *lp_proc, *res_proc;
static hptime_t start, end;

static Serial ser_port;

static void led_init(void)
{
	/* Enable the GPIO port that is used for the on-board LED */
	SYSCTL_RCGC2_R = SYSCTL_RCGC2_GPIOG;
	/*
	 * Perform a dummy read to insert a few cycles delay before enabling
	 * the peripheral.
	 */
	(void)SYSCTL_RCGC2_R;
	/* Enable the GPIO pin for the LED */
	GPIO_PORTG_DIR_R = 0x04;
	GPIO_PORTG_DEN_R = 0x04;
}

INLINE void led_on(void)
{
	GPIO_PORTG_DATA_R |= 0x04;
}

INLINE void led_off(void)
{
	GPIO_PORTG_DATA_R &= ~0x04;
}

static void NORETURN led_process(void)
{
	int i;

	for (i = 0; ; i++)
	{
		if (i & 1)
			led_on();
		else
			led_off();
		timer_delay(50);
	}
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

INLINE hptime_t get_hp_ticks(void)
{
	return (TIMER_HW_CNT - timer_hw_hpread()) +
			timer_clock_unlocked() * TIMER_HW_CNT;
}

static void NORETURN res_process(void)
{
	const char spinner[] = {'/', '-', '\\', '|'};
	int i;
	char c;

	for (i = 0; ; i++)
	{
		ticks_t clock;

		clock = timer_clock_unlocked();

		/* Display uptime (in ticks) */
		text_xprintf(&bm, 2, 0, TEXT_FILL, "uptime: %lu sec", clock / 1000);

		/* Show context switch (in clock cycles) */
		c = spinner[i % countof(spinner)];
		text_xprintf(&bm, 4, 0, TEXT_CENTER | TEXT_FILL, "%c Context switch %c", c, c);
		text_xprintf(&bm, 6, 0, TEXT_FILL, " %lu clock cycles", end - start);
		/* Show context switch (in usec) */
		text_xprintf(&bm, 7, 0, TEXT_FILL,
			" %lu.%lu usec",
				((end - start) * 1000000) / CPU_FREQ,
				((end - start) * (100000000 / CPU_FREQ)) % 100);
		rit128x96_lcd_blitBitmap(&bm);
	}
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

/**
 * Show the splash screen
 */
static void bouncing_logo(Bitmap *bm)
{
	const long SPEED_SCALE = 1000;
	const long GRAVITY_ACCEL = 100;
	const long BOUNCE_ELASTICITY = 2;
	const long TOT_FRAMES = 100;
	long h = (long)(-bertos_logo.height) * SPEED_SCALE;
	long speed = 0, i;

	for (i = 0; i < TOT_FRAMES; i++)
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
		rit128x96_lcd_blitBitmap(bm);
		timer_delay(5);
	}
}

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
	rit128x96_lcd_init();
	gfx_bitmapInit(&bm, raster, LCD_WIDTH, LCD_HEIGHT);
	gfx_setFont(&bm, &font_helvB10);
	rit128x96_lcd_blitBitmap(&bm);
	kputs("Done.\n");

	bouncing_logo(&bm);
	gfx_bitmapClear(&bm);
#ifdef _DEBUG
	text_xprintf(&bm, 4, 0, TEXT_CENTER | TEXT_FILL, "BeRTOS up & running!");
	rit128x96_lcd_blitBitmap(&bm);
	proc_testRun();
#endif
	text_xprintf(&bm, 0, 0, TEXT_FILL,
			"CPU: Cortex-M3 %luMHz", CPU_FREQ / 1000000);
	rit128x96_lcd_blitBitmap(&bm);
	text_xprintf(&bm, 1, 0, TEXT_FILL, "Board: LM3S1968 EVB");
	rit128x96_lcd_blitBitmap(&bm);

	hp_proc = proc_new(hp_process, NULL, PROC_STACK_SIZE, hp_stack);
	lp_proc = proc_new(lp_process, NULL, PROC_STACK_SIZE, lp_stack);
	proc_new(led_process, NULL, PROC_STACK_SIZE, led_stack);
	proc_new(ser_process, NULL, PROC_STACK_SIZE, ser_stack);

	res_proc = proc_current();

	proc_setPri(hp_proc, 2);
	proc_setPri(lp_proc, 1);

	res_process();
}

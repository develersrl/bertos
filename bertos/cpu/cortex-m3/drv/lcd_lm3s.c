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
 * \brief LM3S1968 OLED display driver.
 *
 * \author Andrea Righi <arighi@develer.com>
 */

#include <cfg/debug.h>
#include <cfg/macros.h>
#include "io/lm3s.h"
#include "clock_lm3s.h"
#include "ssi_lm3s.h"
#include "gpio_lm3s.h"
#include "lcd_lm3s.h"

#define GPIO_OLEDDC_BASE            GPIO_PORTH_BASE
#define GPIO_OLEDDC_PIN             BV(2)
#define GPIO_OLEDEN_PIN             BV(3)

/*
 * Hard-coded command initialization sequence.
 *
 * NOTE: the first byte is the size of the command.
 */
static const uint8_t init_cmd[] =
{
	/* Unlock commands */
	3, 0xfd, 0x12, 0xe3,
	/* Display off */
	2, 0xae, 0xe3,
	/* Icon off */
	3, 0x94, 0, 0xe3,
	/* Multiplex ratio */
	3, 0xa8, 95, 0xe3,
	/* Contrast */
	3, 0x81, 0xb7, 0xe3,
	/* Pre-charge current */
	3, 0x82, 0x3f, 0xe3,
	/* Display Re-map */
	3, 0xa0, 0x52, 0xe3,
	/* Display Start Line */
	3, 0xa1, 0, 0xe3,
	/* Display Offset */
	3, 0xa2, 0x00, 0xe3,
	/* Display Mode Normal */
	2, 0xa4, 0xe3,
	/* Phase Length */
	3, 0xb1, 0x11, 0xe3,
	/* Frame frequency */
	3, 0xb2, 0x23, 0xe3,
	/* Front Clock Divider */
	3, 0xb3, 0xe2, 0xe3,
	/* Set gray scale table */
	17, 0xb8, 1, 2, 3, 4, 5, 6, 8, 10, 12, 14, 16, 19, 22, 26, 30, 0xe3,
	/* Second pre-charge period */
	3, 0xbb, 0x01, 0xe3,
	/* Pre-charge voltage */
	3, 0xbc, 0x3f, 0xe3,
	/* Display ON */
	2, 0xaf, 0xe3,
};

/*
 * Hard-coded command shutdown sequence.
 */
static const uint8_t exit_cmd[] =
{
	/* Display OFF */
	0xae, 0xe3
};

/*
 * Hard-coded horizontal increment command.
 */
static const uint8_t horizontal_inc[] =
{
	0xa0, 0x52
};

/**
 * Write a sequence of data bytes to the LCD SSD1329 controller
 */
static void lcd_writeData(const uint8_t *buf, size_t count)
{
	while (count--)
		lm3s_ssi_write_frame(SSI0_BASE, *buf++);
}

/* Turn on the OLED display */
void lm3s_lcd_on(void)
{
	unsigned int i;

	/* Loop through the SSD1329 controller initialization sequence */
	for (i = 0; i < sizeof(init_cmd); i += init_cmd[i] + 1)
	{
		lm3s_gpio_pin_write(GPIO_OLEDDC_BASE, GPIO_OLEDDC_PIN, 0);
		lcd_writeData(init_cmd + i + 1, init_cmd[i] - 1);
	}
}

/* Turn off the OLED display */
void lm3s_lcd_off(void)
{
	lm3s_gpio_pin_write(GPIO_OLEDDC_BASE, GPIO_OLEDDC_PIN, 0);
	lcd_writeData(exit_cmd, sizeof(exit_cmd));
}

/* Refresh a bitmap on screen */
void lm3s_lcd_blitBitmap(const Bitmap *bm)
{
	uint8_t lcd_row[bm->width / 2];
	uint8_t buffer[8];
	uint8_t mask;
	int i, l;

	ASSERT(bm->width == LCD_WIDTH && bm->height == LCD_HEIGHT);

	/* Enter command mode */
	lm3s_gpio_pin_write(GPIO_OLEDDC_BASE, GPIO_OLEDDC_PIN, 0);

	buffer[0] = 0x15;
	buffer[1] = 0;
	buffer[2] = (bm->width - 2) / 2;
	lcd_writeData(buffer, 3);

	buffer[0] = 0x75;
	buffer[1] = 0;
	buffer[2] = bm->height - 1;
	lcd_writeData(buffer, 3);
	lcd_writeData((const uint8_t *)&horizontal_inc, sizeof(horizontal_inc));

	/*
	 * Enter data mode and send the encoded image data to the OLED display,
	 * over the SSI bus.
	 */
	lm3s_gpio_pin_write(GPIO_OLEDDC_BASE, GPIO_OLEDDC_PIN, GPIO_OLEDDC_PIN);
	for (l = 0; l < bm->height / 8; l++)
	{
		for (mask = 1; mask; mask <<= 1)
		{
			for (i = 0; i < bm->width; i++)
			{
				if (bm->raster[l * bm->width + i] & mask)
					lcd_row[i / 2] |= i & 1 ? 0x0f : 0xf0;
				else
					lcd_row[i / 2] &= i & 1 ? 0xf0 : 0x0f;
			}
			/* Write an entire row at once */
			lcd_writeData(lcd_row, sizeof(lcd_row));
		}
	}
}

/**
 * Initialize the OLED display
 *
 * \param freq The SSI Clock Frequency
 */
void lm3s_lcd_init(unsigned long freq)
{
	uint32_t dummy;

	/* Enable the peripheral clock */
	SYSCTL_RCGC1_R |= SYSCTL_RCGC1_SSI0;
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOA;
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOH;
	__delay(512);

	/* Configure the SSI0CLK and SSIOTX pins for SSI operation. */
	lm3s_gpio_pin_config(GPIO_PORTA_BASE, BV(2) | BV(3) | BV(5),
		GPIO_DIR_MODE_HW, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPU);
	/*
	 * Configure the GPIO port pin used as a D/Cn signal for OLED device,
	 * and the port pin used to enable power to the OLED panel.
	 */
	lm3s_gpio_pin_config(GPIO_OLEDDC_BASE, GPIO_OLEDDC_PIN | GPIO_OLEDEN_PIN,
		GPIO_DIR_MODE_OUT, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD);
	lm3s_gpio_pin_write(GPIO_OLEDDC_BASE, GPIO_OLEDDC_PIN | GPIO_OLEDEN_PIN,
			GPIO_OLEDDC_PIN | GPIO_OLEDEN_PIN);

	/* Configure the SSI0 port for master mode */
	lm3s_ssi_init(SSI0_BASE, SSI_FRF_MOTO_MODE_2, SSI_MODE_MASTER, freq, 8);
	/*
	 * Configure the GPIO port pin used as a D/Cn signal for OLED device,
	 * and the port pin used to enable power to the OLED panel.
	 */
	lm3s_gpio_pin_config(GPIO_PORTA_BASE, GPIO_OLEDEN_PIN,
		GPIO_DIR_MODE_HW, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPU);
	/* Enable the SSI port */
	lm3s_ssi_enable(SSI0_BASE);

	/* Drain the RX fifo */
	while (lm3s_ssi_read_frame_nonblocking(SSI0_BASE, &dummy));

	/* Turn on the OLED display */
	lm3s_lcd_on();
}

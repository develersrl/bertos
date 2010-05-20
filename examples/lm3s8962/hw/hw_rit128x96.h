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
 * \brief LM3S1986: OLED-RIT-128x96 (P14201) low-level hardware macros
 *
 * \author Andrea Righi <arighi@develer.com>
 */

#ifndef HW_RIT128x96_H
#define HW_RIT128x96_H

#include "cfg/macros.h"   /* BV() */
#include "cfg/debug.h"

#include <cpu/attr.h>
#include <cpu/irq.h>
#include <cpu/types.h>

#include <drv/clock_lm3s.h>
#include <drv/ssi_lm3s.h>
#include <drv/gpio_lm3s.h>

/**
 * \name LCD I/O pins/ports
 * @{
 */
/* OLED Data/Command control pin */
#define GPIO_OLEDDC_PIN             BV(6)

/* OLED enable pin */
#define GPIO_OLEDEN_PIN             BV(7)
/*@}*/

/**
 * \name LCD bus control macros
 * @{
 */
/* Enter command mode */
#define LCD_SET_COMMAND() \
	lm3s_gpioPinWrite(GPIO_PORTA_BASE, GPIO_OLEDDC_PIN, 0)

/* Enter data mode */
#define LCD_SET_DATA() \
	lm3s_gpioPinWrite(GPIO_PORTA_BASE, GPIO_OLEDDC_PIN, GPIO_OLEDDC_PIN)

/* Send data to the display */
#define LCD_WRITE(x)							\
	{								\
		uint32_t _x;						\
		while (!lm3s_ssiWriteFrameNonBlocking(SSI0_BASE, x));	\
		/* Dummy read to drain the FIFO */			\
		while (!lm3s_ssiReadFrameNonBlocking(SSI0_BASE, &_x));	\
	}
/*@}*/

INLINE void lcd_rit128x96_hw_bus_init(void)
{
	uint32_t dummy;

	/* Enable the peripheral clock */
	SYSCTL_RCGC1_R |= SYSCTL_RCGC1_SSI0;
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOA;
	lm3s_busyWait(512);

	/* Configure the SSI0CLK SSI0FS and SSI0TX pins for SSI operation. */
	lm3s_gpioPinConfig(GPIO_PORTA_BASE, BV(2) | BV(3) | BV(5),
		GPIO_DIR_MODE_HW, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPU);
	/*
	 * Configure the GPIO port pin used as a D/C# signal (data/command
	 * control) for OLED device, and the port pin used to enable power to
	 * the OLED panel.
	 */
	lm3s_gpioPinConfig(GPIO_PORTA_BASE, GPIO_OLEDDC_PIN | GPIO_OLEDEN_PIN,
		GPIO_DIR_MODE_OUT, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD);
	lm3s_gpioPinWrite(GPIO_PORTA_BASE, GPIO_OLEDDC_PIN | GPIO_OLEDEN_PIN,
			GPIO_OLEDDC_PIN | GPIO_OLEDEN_PIN);

	/* Configure the SSI0 port for master mode */
	lm3s_ssiOpen(SSI0_BASE, SSI_FRF_MOTO_MODE_2,
			SSI_MODE_MASTER, CPU_FREQ / 2, 8);

	/* Drain the SSI RX FIFO */
	while (lm3s_ssiReadFrameNonBlocking(SSI0_BASE, &dummy));
}

#endif /* HW_RIT128x96_H */

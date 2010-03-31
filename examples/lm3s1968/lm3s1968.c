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
#include "io/lm3s.h"

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

static void led_on(void)
{
	GPIO_PORTG_DATA_R |= 0x04;
}

static void led_off(void)
{
	GPIO_PORTG_DATA_R &= ~0x04;
}

static NORETURN void spinner_thread(void)
{
	char spinner[] = {'/', '-', '\\', '|'};
	int i;

	kputs("\n");
	for(i = 0; ; i++)
	{
		kprintf("BeRTOS is up & running: %c\r",
				spinner[i % countof(spinner)]);
		timer_delay(100);
	}
}

int main(void)
{
	IRQ_ENABLE;
	led_init();

	proc_testSetup();
	proc_testRun();

	proc_new(spinner_thread, NULL, KERN_MINSTACKSIZE, NULL);
	while(1)
	{
		led_on();
		timer_delay(250);
		led_off();
		timer_delay(250);
	}
}

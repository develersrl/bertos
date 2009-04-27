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
 * Copyright 2009 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \version $Id: at91sam7s.c 2663 2009-04-24 16:30:11Z asterix $
 *
 * \author Francesco Sacchi <batt@develer.com>
 * \author Daniele Basile <asterix@develer.com>
 *
 * \brief Simple BeRTOS test on AT91SAM7X-EK evaluation board.
 * 
 * This short program shows you a simple demo of some BeRTOS feature:
 * 
 * - Debug system
 * - Timer interrupt
 * - Serial
 * - Cooperative BeRTOS Kernel
 * 
 */

#include "cfg/cfg_ser.h"
#include <cfg/macros.h>

#include <kern/proc.h>

#include <drv/timer.h>
#include <drv/sysirq_at91.h>
#include <drv/ser.h>

#include <io/arm.h>

Timer leds_timer;
Serial ser_fd;
int roll = 0;

/*
 * Supercar leds effect..
 */
static void leds_toggle(void)
{
	uint32_t a = (~PIOB_ODSR & 0x780000);

	// Turn on led in forward direction
	if (roll == 1)
	{
		if(a == 0x200000)
			roll = 2;

		PIOB_SODR = a;
		PIOB_CODR = a << 1;
	}
	// Turn on led in backward direction
	else if (roll == 2)
	{
		if(a == 0x100000)
			roll = 1;

		PIOB_SODR = a;
		PIOB_CODR = a >> 1;
	}
	// Start to turn on first led
	else
	{
		PIOB_SODR  = 0x780000;
		/* turn first led on */
		PIOB_CODR  = 0x80000;
		roll = 1;
	}

	/* Wait for interval time */
	timer_setDelay(&leds_timer, ms_to_ticks(100));
	timer_add(&leds_timer);
}

int main(void)
{	char msg[]="BeRTOS, be fast be beatiful be realtime";


	kdbg_init();
	timer_init();
	proc_init();

	ASSERT(!IRQ_ENABLED());

	/* Open the main communication port */
	ser_init(&ser_fd, 0);
	ser_setbaudrate(&ser_fd, 115200);
	ser_setparity(&ser_fd, SER_PARITY_NONE);

	IRQ_ENABLE;
	ASSERT(IRQ_ENABLED());

	/* Disable all pullups */
	PIOB_PUDR = 0xffffffff;
	/* Set PB0..3 connected to PIOA */
	PIOB_PER  = 0x780000;
	/* Set PB0..3 as output */
	PIOB_OER  = 0x780000;
	/* Disable multidrive on all pins */
	PIOB_MDDR = 0xffffffff;

	/* Set PA0..3 to 1 to turn off leds */
	PIOB_SODR  = 0x780000;
	
	/* turn first led on */
	PIOB_CODR  = 0x80000;

	/*
	 * Register timer and arm timer interupt.
	 */
	timer_setSoftint(&leds_timer, (Hook)leds_toggle, 0);
	timer_setDelay(&leds_timer, ms_to_ticks(100));
	timer_add(&leds_timer);

	/*
	 * Run process test.
	 */
	if(!proc_testRun())
		kfile_printf(&ser_fd.fd, "ProcTest..ok!\n");
	else
		kfile_printf(&ser_fd.fd, "ProcTest..FAIL!\n");

	// Main loop
	for(;;)
	{
		kfile_printf(&ser_fd.fd, "From serial 0: %s\r\n", msg);
	}
	return 0;
}

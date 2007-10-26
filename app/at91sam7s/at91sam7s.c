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
 * Copyright 2007 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \version $Id$
 *
 * \author Francesco Sacchi <batt@develer.com>
 *
 * \brief AT91SAM7S-EK porting test.
 */

#include <cfg/macros.h>
#include <drv/timer.h>
#include <drv/sysirq_at91.h>
#include <kern/proc.h>
#include <drv/ser.h>
#include <cfg/macros.h>
#include <io/arm.h>

Timer leds_timer;

static void leds_toggle(void)
{
	uint8_t a = (~PIOA_ODSR & 0x0f);

	if (a)
	{
		PIOA_SODR = a;
		PIOA_CODR = a << 1;
	}
	else
	{
		PIOA_SODR  =  0x0f;
		/* turn first led on */
		PIOA_CODR  = 0x00000001;
	}

	/* Wait for interval time */
	timer_setDelay(&leds_timer, ms_to_ticks(100));
	timer_add(&leds_timer);
}


int main(void)
{
	kdbg_init();
	sysirq_init();
	timer_init();

//  proc_init();
	ASSERT(!IRQ_ENABLED());



	/* Open the main communication port */
	Serial *host_port = ser_open(0);
	ser_setbaudrate(host_port, 115200);
	ser_setparity(host_port, SER_PARITY_NONE);

	/* Open the main communication port */
	Serial *host_port1 = ser_open(1);
	ser_setbaudrate(host_port1, 115200);
	ser_setparity(host_port1, SER_PARITY_NONE);

	IRQ_ENABLE;
	ASSERT(IRQ_ENABLED());

	/* Disable all pullups */
	PIOA_PUDR = 0xffffffff;
	/* Set PA0..3 connected to PIOA */
	PIOA_PER  = 0x0000001f;
	/* Set PA0..3 as output */
	PIOA_OER  = 0x0000001f;
	/* Disable multidrive on all pins */
	PIOA_MDDR = 0x0000001f;

	/* Set PA0..3 to 1 to turn off leds */
	PIOA_SODR  = 0x0000000f;
	/* turn first led on */
	PIOA_CODR  = 0x00000001;


// 	timer_set_event_softint(&leds_timer, (Hook)leds_toggle, 0);
// 	timer_setDelay(&leds_timer, ms_to_ticks(100));
// 	timer_add(&leds_timer);

	int a=0;
	// Main loop
	for(;;)
	{
//		proc_test();

		a = ser_getchar(host_port1);

		if(a == 49)
			PIOA_CODR  = 0x0000000F;
		if(a == 50)
		{
			PIOA_SODR  = 0x0000000A;
			ser_printf(host_port, "%c", a);
		}

		if(a == 51)
			PIOA_CODR  = 0x0000000F;
		if(a == 52)
		{
			PIOA_SODR  = 0x00000005;
			ser_printf(host_port1, "%c", a);
		}

	}

	return 0;
}

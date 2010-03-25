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
 * \version $Id$
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
#include <kern/signal.h>

#include <cpu/detect.h>

#include <drv/timer.h>
#include <drv/sysirq_at91.h>
#include <drv/ser.h>

#include <io/arm.h>

Timer leds_timer;
Serial ser_fd;

enum
{
	FORWARD,
	BACKWARD,
};

int direction = FORWARD;

static void leds_init(void)
{
	#if CPU_ARM_AT91SAM7X256
		/* Set PB19..22 connected to PIOB */
		PIOB_PER  = 0x780000;
		/* Set PB19..22 as output */
		PIOB_OER  = 0x780000;

		/* Set PB19..22 to 1 to turn off leds */
		PIOB_SODR  = 0x780000;

		/* turn first led on (PB19) */
		PIOB_CODR  = 0x80000;
	#elif CPU_ARM_AT91SAM7S256
		/* Set PA0..3 connected to PIOA */
		PIOA_PER  = 0x0000001f;
		/* Set PA0..3 as output */
		PIOA_OER  = 0x0000001f;

		/* Set PA0..3 to 1 to turn off leds */
		PIOA_SODR  = 0x0000000f;
		/* turn first led on (PA0) */
		PIOA_CODR  = 0x00000001;
	#endif
}

#if CPU_ARM_AT91SAM7X256
	#define GET_PIO_STATUS()  (~PIOB_ODSR & 0x780000)
	#define LAST_LED                        0x200000
	#define FIRST_LED                       0x100000
	#define SET_PIO_BITS                    PIOB_SODR
	#define CLEAR_PIO_BITS                  PIOB_CODR
	#define AT91SAM7_MSG      "BeRTOS is running on AT91SAM7X256..\n"
#elif CPU_ARM_AT91SAM7S256
	#define GET_PIO_STATUS()  (~PIOA_ODSR & 0x0000000f)
	#define LAST_LED                        0x00000004
	#define FIRST_LED                       0x00000002
	#define SET_PIO_BITS                    PIOA_SODR
	#define CLEAR_PIO_BITS                  PIOA_CODR
	#define AT91SAM7_MSG      "BeRTOS is running on AT91SAM7S256..\n"
#endif

/*
 * Knight Rider leds effect..
 */
static void leds_toggle(void)
{
	uint32_t led_status = GET_PIO_STATUS();

	// Turn on led in forward direction
	if (direction == FORWARD)
	{
		if(led_status == LAST_LED)
			direction = BACKWARD;

		SET_PIO_BITS = led_status;
		CLEAR_PIO_BITS = led_status << 1;
	}
	// Turn on led in backward direction
	else if (direction == BACKWARD)
	{
		if(led_status == FIRST_LED)
			direction = FORWARD;

		SET_PIO_BITS = led_status;
		CLEAR_PIO_BITS = led_status >> 1;
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
	leds_init();

	ASSERT(!IRQ_ENABLED());

	/* Open the main communication port */
	ser_init(&ser_fd, 0);
	ser_setbaudrate(&ser_fd, 115200);
	ser_setparity(&ser_fd, SER_PARITY_NONE);

	IRQ_ENABLE;
	ASSERT(IRQ_ENABLED());

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
	/*
	 * Run signal test.
	 */
	if(!signal_testRun())
		kfile_printf(&ser_fd.fd, "SignalTest..ok!\n");
	else
		kfile_printf(&ser_fd.fd, "SignalTest..FAIL!\n");

	kputs(AT91SAM7_MSG);

	// Main loop
	for(;;)
	{
		kfile_printf(&ser_fd.fd, "From serial 0: %s\r\n", msg);
	}
	return 0;
}

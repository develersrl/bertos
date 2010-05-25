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
 * Copyright 2003, 2004, 2006, 2008, 2010 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2000 Bernie Innocenti <bernie@codewiz.org>
 *
 * -->
 *
 * \author Marco Benelli <marco@develer.com>
 * \author Bernie Innocenti <bernie@codewiz.org>
 * \author Daniele Basile <asterix@develer.com>
 *
 * \brief Triface application.
 *
 * This application manage some devices, like buzzer, digital in/out or analogic input.
 * To controll all devices the application provide the interactive shell
 * that allow the user to send commands to the board. All commands are generate
 * throuth the macro, that convert its parameters, at compile time, to C function.
 * In this way is more simple to implement the new command. Other that, the application,
 * automaticly, parse this new command from the shell and call the our function.
 * Actually this application runs on custom hardware, that you can see the scheme on bertos
 * site, this hardware is able to read the tags string that come from rfid reader and
 * print this on another serial port. The main scope of this function is to create
 * the automatic opendoor, reading the tag key that will pass on the rfid reader.
 */

#include "protocol.h"

#include "hw/hw_input.h"

#include <cfg/macros.h>

#include <drv/timer.h>
#include <drv/buzzer.h>
#include <drv/ser.h>
#include <drv/adc.h>
#include <drv/wdt_avr.h>

#include <mware/parser.h>
#include <net/keytag.h>

static Serial fd_ser;
static Serial tag_ser;

static void init(void)
{
	IRQ_ENABLE;
	kdbg_init();
	timer_init();
	adc_init();
	buz_init();
	MCUSR = 0;
	wdt_disable();

	INPUT_INIT();
}

int main(void)
{
	init();

	/* Initialize Tag serial port and data structure */
	TagPacket pkt;

	/* Open the main communication port */
	ser_init(&fd_ser, SER_UART1);
	ser_setbaudrate(&fd_ser, 115200);

	/* Tag reader serial port */
	ser_init(&tag_ser, SER_UART0);
	ser_setbaudrate(&tag_ser, 9600);

	/* Init the tag parser */
	keytag_init(&pkt, &fd_ser.fd, &tag_ser.fd);

	protocol_init(&fd_ser.fd);

	while (1)
	{
		protocol_run(&fd_ser.fd);

		uint8_t buf[CONFIG_TAG_MAX_LEN];
		int len;
		/* Read the new tag, if this is avaible */
		if ((len = keytag_recv(&pkt, buf, sizeof(buf))) != EOF)
			kfile_write(&fd_ser.fd, buf, len);
	}

	return 0;
}


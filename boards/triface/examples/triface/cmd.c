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
 * Copyright 2003, 2004, 2006, 2012 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2000 Bernie Innocenti <bernie@codewiz.org>
 *
 * -->
 *
 * \brief Implementation of the command protocol between the board and the host
 *
 *
 * \author Giovanni Bajo <rasky@develer.com>
 * \author Marco Benelli <marco@develer.com>
 * \author Bernie Innocenti <bernie@codewiz.org>
 * \author Daniele Basile <asterix@develer.com>
 */

#include "cmd.h"
#include "verstag.h"

#include "hw/hw_input.h"

#include "cfg/cfg_parser.h"

#include <cfg/compiler.h>
#include <cfg/debug.h>

#include <mware/parser.h>

#include <drv/adc.h>
#include <drv/timer.h>
#include <drv/ser.h>
#include <drv/sipo.h>
#include <drv/buzzer.h>

#include <avr/wdt.h>

// Define the format string for ADC
#define ADC_FORMAT_STR "dddd"
#define ADC_CHANNEL_NUM    4

static Sipo *local_sipo;
static uint8_t reg_status_dout;


MAKE_CMD(ver, "", "ddd",
({
	args[1].l = VERS_MAJOR;
	args[2].l = VERS_MINOR;
	args[3].l = VERS_REV;
	0;
}), 0);

/* Sleep. Example of declaring function body directly in macro call.  */
MAKE_CMD(sleep, "d", "",
({
	timer_delay((mtime_t)args[1].l);
	0;
}), 0)

/* Ping.  */
MAKE_CMD(ping, "", "",
({
	//Silence "args not used" warning.
	(void)args;
	0;
}), 0)

/* Dout  */
MAKE_CMD(dout, "d", "",
({
	kfile_putc((uint8_t)args[1].l, &local_sipo->fd);

	//Store status of dout ports.
	reg_status_dout = (uint8_t)args[1].l;
	0;
}), 0)

/* rdout  read the status of out ports.*/
MAKE_CMD(rdout, "", "d",
({
	args[1].l = reg_status_dout;
	0;
}), 0)


/* Reset */
MAKE_CMD(reset, "", "",
({
	//Silence "args not used" warning.
	(void)args;
	wdt_enable(WDTO_2S);

	/*We want to have an infinite loop that lock access on watchdog timer.
	This piece of code it's equivalent to a while(true), but we have done this because
	gcc generate a warning message that suggest to use "noreturn" parameter in function reset.*/
	ASSERT(args);
	while(args);
	0;

}), 0)

/* Din */
MAKE_CMD(din, "", "d",
({
	args[1].l = INPUT_GET();
	0;
}), 0)


/* Ain */
MAKE_CMD(ain, "", ADC_FORMAT_STR,
({
	STATIC_ASSERT((sizeof(ADC_FORMAT_STR) - 1) == ADC_CHANNEL_NUM);
	for(int i = 0; i < ADC_CHANNEL_NUM; i++)
		args[i+1].l = adc_read(i);

	0;
}), 0)

/* Beep  */
MAKE_CMD(beep, "d", "",
({
	buz_beep(args[1].l);
	0;
}), 0)

/* Register commands.  */
void cmd_register(void)
{
	REGISTER_CMD(ver);
	REGISTER_CMD(sleep);
	REGISTER_CMD(ping);
	REGISTER_CMD(dout);
	REGISTER_CMD(rdout);
	REGISTER_CMD(reset);
	REGISTER_CMD(din);
	REGISTER_CMD(ain);
	REGISTER_CMD(beep);
}

void cmd_init(Sipo *sipo)
{
	ASSERT(sipo);
	local_sipo = sipo;
	reg_status_dout = 0;
}

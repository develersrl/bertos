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
 * Copyright 2003, 2004, 2006 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2000 Bernie Innocenti <bernie@codewiz.org>
 *
 * -->
 *
 * \brief Flash boot loader main.
 * This is a simple generic bootloader app.
 * It requires only a serial port to work.
 * Try to receive a file through XMODEM protocol
 * and flash it on program memory.
 * BOOT_INIT, BOOT_END are macros used to perform special operations
 * respectively at boot start and boot end and are CPU dependant.
 * The macro START_APP() jumps to main application start.
 *
 * \version $Id$
 * \author Stefano Fedrigo <aleph@develer.com>
 * \author Francesco Sacchi <batt@develer.com>
 * \author Daniele Basile <asterix@develer.com>
 */

#include "hw/hw_boot.h"
#include "cfg/cfg_ser.h"

#include <net/xmodem.h>

#include <cfg/compiler.h>
#include <cfg/debug.h>
#include <cfg/macros.h> /* BV() */

#include <drv/ser.h>
#include <drv/timer.h>
#include <drv/flash.h>

#include <string.h>

#include <avr/wdt.h>

/*
 * Watchdog disable.
 *
 * This function disable the watchdog timer early after a reset.
 * We must do it very soon because new AVR cores do not disable
 * the watchdog timer after a cpu reset. In this way the watchdog
 * timer is still enabled, continuously resetting the cpu. This is
 * necessary only with new AVR cores, for other cores this code has
 * no effect.
 *
 * \{
 */
// Function prototype of watchdog reset.
void wdt_init(void) __attribute__((naked)) __attribute__((section(".init3")));
// Function implementation of watchdog reset.
void wdt_init(void)
{
    MCUSR = 0;
    wdt_disable();

    return;
}
/* \} */

int main(void)
{
	Flash flash;
	Serial ser;


	// Set up flash programming functions.
	flash_init(&flash);

	IRQ_ENABLE;

	BOOT_INIT;

	kdbg_init();
	timer_init();

	/* Open the main communication port */

	ser_init(&ser, CONFIG_BOOT_PORT);
	ser_setbaudrate(&ser, CONFIG_BOOT_BAUDRATE);

	xmodem_recv(&ser.fd, &flash.fd);

	kfile_close(&flash.fd);
	kfile_close(&ser.fd);

	IRQ_DISABLE;

	BOOT_END;

	START_APP();

}


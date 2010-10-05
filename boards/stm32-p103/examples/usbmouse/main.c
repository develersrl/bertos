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
 * \author Andrea Righi <arighi@develer.com>
 *
 * \brief BeRTOS Virtual USB Mouse.
 *
 * This project implements a HID USB device using the Olimex STM32-P103
 * evaluation board. The virtual mouse can be connected to the USB port of a
 * normal PC.
 *
 * The WAKE_UP button on the board can be pressed to send to the host the mouse
 * movement to draw a rectangle shape (taking the left button pressed).
 *
 * Example (mouse detection on Linux):
 *
 * $ dmesg | tail
 * [25361.710435] usb 1-8.1.3: new full speed USB device using ehci_hcd and address 33
 * [25361.943901] usb 1-8.1.3: configuration #1 chosen from 1 choice
 * [25362.004002] input: BeRTOS USB Mouse as /devices/pci0000:00/0000:00:1d.7/usb1/1-8/1-8.1/1-8.1.3/1-8.1.3:1.0/input/input19
 * [25362.004738] generic-usb 0003:FFFF:0000.0008: input,hidraw3: USB HID v1.10 Mouse [BeRTOS USB Mouse] on usb-0000:00:1d.7-8.1.3/input0
 *
 */

#include "hw/hw_led.h"

#include <cfg/debug.h>

#include <cpu/irq.h>

#include <drv/kbd.h>
#include <drv/timer.h>
#include <drv/usbmouse.h>

static void init(void)
{
	/* Enable all the interrupts */
	IRQ_ENABLE;

	/* Initialize debugging module (allow kprintf(), etc.) */
	kdbg_init();
	/* Initialize system timer */
	timer_init();
	/* Initialize LED driver */
	LED_INIT();
	/* Enable the WAKE_UP button */
	kbd_init();
	/* Initialize the USB mouse device */
	usbmouse_init(0);
}

int main(void)
{
	int i;

	/* Hardware initialization */
	init();

	/* Put your code here... */
	kprintf("USB HID Mouse configured\n");
	kbd_setRepeatMask(K_WAKEUP);
	while (1)
	{
		/* Wait until WAKE_UP is pressed */
		kbd_get();

		/* Left button pressed down */
		usbmouse_sendEvent(0, 0, BV(0));
		timer_delay(100);

		/* Move left */
		for (i = 0; i < 25; i++)
		{
			usbmouse_sendEvent(-2, 0, BV(0));
			timer_delay(10);
		}
		/* Move down */
		for (i = 0; i < 25; i++)
		{
			usbmouse_sendEvent(0, 2, BV(0));
			timer_delay(10);
		}
		/* Move right */
		for (i = 0; i < 25; i++)
		{
			usbmouse_sendEvent(2, 0, BV(0));
			timer_delay(10);
		}
		/* Move up */
		for (i = 0; i < 25; i++)
		{
			usbmouse_sendEvent(0, -2, BV(0));
			timer_delay(10);
		}
		/* Left button released */
		usbmouse_sendEvent(0, 0, 0);
	}
}

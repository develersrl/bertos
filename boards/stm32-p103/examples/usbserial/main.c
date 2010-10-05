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
 * \brief USB/Serial converter for the STM32-P103 evaluation board.
 *
 * This example implements a real USB/serial converter using the STM32-P103
 * evaluation board. Two independent processes are created: one that reads from
 * the USB port and writes to the UART port, the other does the opposite.
 *
 * Compile and flash the firmware image to the STM32-P103. Then, the board can
 * be connected to any USB port of a standard PC and it will be recognized as a
 * generic USB/serial converter.
 *
 * For example, on a Linux PC:
 *
 * $ dmesg
 * [18058.280545] usb 1-8.1.3: new full speed USB device using ehci_hcd and address 25
 * [18058.392775] usb 1-8.1.3: configuration #1 chosen from 1 choice
 * [18058.393118] usbserial_generic 1-8.1.3:1.0: generic converter detected
 * [18058.393332] usb 1-8.1.3: generic converter now attached to ttyUSB1
 *
 */

#include "hw/hw_led.h"

#include <cfg/debug.h>
#include <cfg/log.h>

#include <cpu/irq.h>

#include <drv/timer.h>
#include <drv/ser.h>
#include <drv/usbser.h>

#include <kern/monitor.h>

enum {
	USB_TO_SERIAL,
	SERIAL_TO_USB,
};

static Serial ser_port;
static USBSerial usb_port;

INLINE void LED_TOGGLE(void)
{
	static int led_status;

	if ((led_status = !led_status) != 0)
		ATOMIC(LED_ON());
	else
		ATOMIC(LED_OFF());
}

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
	LED_OFF();

	/* Kernel initialization */
	proc_init();

	/* Initialize the serial driver */
	ser_init(&ser_port, SER_UART2);
	/*
	 * Hard-code the baud rate to 115.200 bps.
	 *
	 * TODO: implement the baud rate settings as well as other UART
	 * settings over the USB connection.
	 */
	ser_setbaudrate(&ser_port, 115200);

	/* Initialize usb-serial driver */
	usbser_init(&usb_port, 0);
}

/* Process that reads from the USB port and writes to the UART port */
static void NORETURN usb_serial_process(void)
{
	iptr_t type = proc_currentUserData();

	KFile *in_fd = (type == USB_TO_SERIAL) ? &usb_port.fd : &ser_port.fd;
	KFile *out_fd = (type == USB_TO_SERIAL) ? &ser_port.fd : &usb_port.fd;

	while (1)
	{
		int c;

		c = kfile_getc(in_fd);
		if (UNLIKELY(c == EOF))
		{
			kfile_clearerr(in_fd);
			continue;
		}
		kfile_putc(c, out_fd);

		/*
		 * Toggle the STAT LED when some data passes through the
		 * usb-seral link
		 */
		LED_TOGGLE();
	}
}

int main(void)
{
	/* Hardware initialization */
	init();

	proc_new(usb_serial_process, (iptr_t)USB_TO_SERIAL,
			KERN_MINSTACKSIZE * 2, NULL);
	proc_new(usb_serial_process, (iptr_t)SERIAL_TO_USB,
			KERN_MINSTACKSIZE * 2, NULL);

	/* Main process will never be scheduled. */
	sig_wait(SIG_USER0);
	ASSERT(0);
}

/**
 * \file
 * <!--
 * Copyright 2006 Develer S.r.l. (http://www.develer.com/)
 * This file is part of DevLib - See README.devlib for information.
 * -->
 *
 * \version $Id$
 *
 * \author Marco Benelli <marco@develer.com>
 * \author Bernardo Innocenti <bernie@develer.com>
 *
 * \brief Windowing system test.
 */

/*#*
 *#* $Log$
 *#* Revision 1.1  2006/05/18 00:41:47  bernie
 *#* New triface devlib application.
 *#*
 *#*/

#include <drv/timer.h>
#include <drv/buzzer.h>
#include <drv/ser.h>
#include <cfg/macros.h>

int main(void)
{
	kdbg_init();
	timer_init();
//	buz_init();

	IRQ_ENABLE;

	Serial *host_port = ser_open(0);
	ser_setbaudrate(host_port, 38400);


	// Main loop
	for(;;)
	{
		ser_print(host_port, "Hello, world!\r\n");
	}

	return 0;
}

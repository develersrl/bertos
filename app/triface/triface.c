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
 *#* Revision 1.3  2006/06/12 21:37:02  marco
 *#* implemented some commands (ver and sleep)
 *#*
 *#* Revision 1.2  2006/06/01 12:29:21  marco
 *#* Add first simple protocol command (version request).
 *#*
 *#* Revision 1.1  2006/05/18 00:41:47  bernie
 *#* New triface devlib application.
 *#*
 *#*/

#include <drv/timer.h>
#include <drv/buzzer.h>
#include <drv/ser.h>
#include <cfg/macros.h>
#include <mware/parser.h>

#include "protocol.h"

int main(void)
{
	kdbg_init();
	timer_init();
//	buz_init();

	IRQ_ENABLE;

	Serial *host_port = ser_open(0);
	ser_setbaudrate(host_port, 38400);

	protocol_init(host_port);
	timer_delay(2000);

	// Main loop
	for(;;)
	{
		protocol_run(host_port);
	}

	return 0;
}

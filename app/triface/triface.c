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
 *#* Revision 1.4  2006/06/14 00:28:52  marco
 *#* Removed initial delay.
 *#*
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

	// Main loop
	for(;;)
	{
		protocol_run(host_port);
	}

	return 0;
}

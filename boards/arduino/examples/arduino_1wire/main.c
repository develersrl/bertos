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
 * \brief Empty project.
 *
 * This is a minimalist project, it just initializes the hardware of the
 * supported board and proposes an empty main.
 */

#include "hw/hw_led.h"

#include <cfg/debug.h>

#include <cpu/irq.h>
#include <cpu/power.h>

#include <algo/crc8.h>

#include <drv/timer.h>
#include <drv/ser.h>
#include <drv/ow_1wire.h>
#include <drv/ow_ds2438.h>
#include <drv/ow_ds2413.h>
#include <drv/ow_ds18x20.h>

uint8_t ids[4][OW_ROMCODE_SIZE];	// only expect to find 3 actually!!
int8_t battid = -1, gpioid = -1, thermid = -1;

static Serial ser;
static CTX2438_t BatteryCtx;



static void
init (void)
{
	uint8_t diff, cnt = 0;

	/* Enable all the interrupts */
	IRQ_ENABLE;

	/* Initialize debugging module (allow kprintf(), etc.) */
	kdbg_init ();
	/* Initialize system timer */
	timer_init ();

	/* Initialize serial port, we are going to use it to show APRS messages */
	ser_init (&ser, SER_UART0);
	ser_setbaudrate (&ser, 115200L);


	for (diff = OW_SEARCH_FIRST, cnt = 0; diff != OW_LAST_DEVICE; cnt++)
	{
		diff = ow_rom_search (diff, ids[cnt]);

		if ((diff == OW_PRESENCE_ERR) || (diff == OW_DATA_ERR))
		{
			break;	// <--- early exit!
		}
		kfile_printf(&ser.fd, "Found device %02x:%02x%02x%02x%02x%02x%02x:%02x\r\n", ids[cnt][0],ids[cnt][1],ids[cnt][2],ids[cnt][3],ids[cnt][4],ids[cnt][5],ids[cnt][6],ids[cnt][7]);
		if (crc8 (ids[cnt], 8))
			kfile_print(&ser.fd, "CRC suspect\r\n");

		switch (ids[cnt][0])
		{
		case SBATTERY_FAM:
			battid = cnt;
			break;
		case SSWITCH_FAM:
			gpioid = cnt;
			break;
		case DS18S20_FAMILY_CODE:
		case DS18B20_FAMILY_CODE:
		case DS1822_FAMILY_CODE:
			thermid = cnt;
			break;
		}
	}
}

// current sense shunt resistor
#define RSHUNT 0.1
// initial amount of charge in battery
#define CHARGE 50

int
main (void)
{
	uint8_t read = 0xff;
	int16_t temperature, diff;
	uint8_t bits = 9;
	ticks_t start;
	ticks_t polltime = timer_clock();

	init ();

	/* Put your code here... */
	while (1)
	{
		if (timer_clock() - polltime > ms_to_ticks(1500L))
		{
			polltime = timer_clock();
			if (thermid >= 0)
			{
				ow_ds18x20_resolution(ids[thermid], bits);
				start = timer_clock();
				ow_ds18X20_start (ids[thermid], false);
				while (ow_busy ());
				ow_ds18X20_read_temperature (ids[thermid], &temperature);
				diff = timer_clock() - start;
				kfile_printf(&ser.fd, "DS18x20 Resolution: %d, Conversion: %dmS, Temperature: %2.2f\r\n", bits, diff, (float)(temperature / 100.0));
				if (++bits > 12)
					bits = 9;
			}
			if (battid >= 0)
			{
				ow_ds2438_init (ids[battid], &BatteryCtx, RSHUNT, CHARGE);
				start = timer_clock();
				ow_ds2438_doconvert (ids[battid]);
				if (!ow_ds2438_readall (ids[battid], &BatteryCtx))
					continue;                   // bad read - exit fast!!
				diff = timer_clock() - start;
				kfile_printf(&ser.fd, "DS2438 Conversion: %dmS, Temperature: %2.2f, voltage %2.2f, current %2.2f\r\n", diff, (float)BatteryCtx.Temp / 100.0, (float)BatteryCtx.Volts / 100.0, (float)BatteryCtx.Amps / 100.0);
				kfile_printf(&ser.fd, "                         Charge now %d, total in %d, total out %d amp-hrs\r\n", BatteryCtx.Charge, BatteryCtx.CCA, BatteryCtx.DCA);
			}
			if (gpioid >= 0)
			{
				read = ow_ds2413_read (ids[gpioid]);
				if (read & 1)
				{
					if (!ow_ds2413_write (ids[gpioid], 2))     // set PIO on other port
						continue;
				}
				else
				{
					if (!ow_ds2413_write (ids[gpioid], 0))     // clear PIO on other port
						continue;
				}
				kfile_printf(&ser.fd, "DS2413 GPIO %d\r\n", read);
			}
		}
		cpu_relax();
	}
}

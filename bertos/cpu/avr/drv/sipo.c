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
 * Copyright 2008 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \version $Id$
 *
 * \author Andrea Grandi <andrea@develer.com>
 *
 * \brief SIPO Module
 *
 * The SIPO module trasform a serial input in
 * a parallel output. Please check hw_sipo.h
 * file to customize hardware relative parameters.
 *
 */

#include "sipo.h"
#include "hw/hw_sipo.h"

#include <drv/ser.h>

Serial *sipo_port;

/** Initialize the SIPO port */
void sipo_init(void)
{
	CLOCK_LOW;
	SET_SOUT_LOW;
	LOAD_LOW;
	SET_SCK_OUT;
	SET_SOUT_OUT;
	LOAD_INIT;
	sipo_putchar(0x0);
	OE_OUT;
	OE_LOW;
}

/** Write a char in the SIPO port and manage the LOAD pin */
void sipo_putchar(uint8_t c)
{
	for(int i = 0; i <= 7; i++)
	{
		if((c & BV(i)) == 0)
		{
			SET_SOUT_LOW;
		}
		else
		{
			SET_SOUT_HIGH;
		}

		CLOCK_PULSE;
	}

	LOAD_HIGH;
	LOAD_LOW;
}


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
 * Copyright 2011 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \defgroup pcint Pin change interrupt implementation
 *
 * \author Tóth Balázs <balazs.toth@jarkon.hu>
 *
 * notest:avr
 * notest:arm
 */

#include "pcint.h"

#include <cfg/debug.h>


struct Pcint *pcint_handles[PCINT_CNT];


void pcint_init(struct Pcint *handle, uint8_t unit)
{
	ASSERT(unit < countof(pcint_handles));

	/* Register the handle */
	pcint_handles[unit] = handle;

	/** \note Every handle can be inited once. */
	ASSERT(!(handle->inited));
	DB(handle->inited = true);

	/* Physical assertion */
	handle->unit = unit;

	pcint_setmask(handle, 0);
	pcint_hw_init(unit);
}



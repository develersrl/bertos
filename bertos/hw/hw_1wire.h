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
 * Copyright 2004, 2005 Develer S.r.l. (http://www.develer.com/)
 * All Rights Reserved.
 * -->
 *
 * \brief Driver for Dallas 1-wire devices
 *
 *
 * \author Peter Dannegger (danni(at)specs.de)
 * \author Martin Thomas (mthomas(at)rhrk.uni-kl.de)
 * \author Robin Gilks
 *
 * $WIZ$ module_name = "hw_1wire"
 */

#ifndef HW_1WIRE_H_
#define HW_1WIRE_H_

#include <stdint.h>

#include "cfg/cfg_arch.h"
#include "cfg/compiler.h"

#warning TODO:This is an example implementation, you must implement it!


/**
 * Get the state of an input pin
 *
 * \return I/O pin value
 */
INLINE uint8_t ow_input_pin_state (void)
{
	return 0;
}

/**
 * Enable parasitic mode (set line high to power device)
 *
 */
INLINE void ow_parasite_enable (void)
{
}

/**
 * Disable parasitic mode
 *
 */
INLINE void ow_parasite_disable (void)
{
}

/**
 * Reset the bus, disable parasitic mode
 *
 * \return non zero = error code
 */
INLINE uint8_t ow_reset_intern (void)
{
	return 0;
}

/**
 * Function to output a bit
 *
 * \param b bit to output
 * \param with_parasite_enable flag to indicate leave the data line active high
 * \return bit read from I/O
 */
INLINE uint8_t ow_bit_io_intern (uint8_t b, uint8_t with_parasite_enable)
{
	(void) (b);
   (void) (with_parasite_enable);
	return 0;
}


/**
 * Set the port/data direction input pin dynamically
 *
 * \param in input port
 * \param out output port
 * \param ddr data direction register
 * \param pin I/O pin (bit number on port)
 *
 */
void ow_set_bus (volatile uint8_t * in, volatile uint8_t * out, volatile uint8_t * ddr, uint8_t pin)
{
	(void) in;
	(void) out;
	(void) ddr;
	(void) pin;
}








#endif

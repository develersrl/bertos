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
 *  Copyright (C) 2012 Robin Gilks
 * -->
 *
 * \addtogroup ow_driver 1-wire driver
 * \ingroup drivers
 * \{
 *
 *
 * \brief Driver for Dallas 1-wire devices
 *
 *
 * \author Peter Dannegger (danni(at)specs.de)
 * \author Martin Thomas (mthomas(at)rhrk.uni-kl.de)
 * \author Robin Gilks <g8ecj@gilks.org>
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
	 * \defgroup 1wirehw_api Hardware API
	 * Access to this low level driver is mostly from the device specific layer. However, some functions - especially the 
	 * ow_set_bus() function operates at the lowest level.
	 *
	 * This functionality is especially useful when devices are hardwired and so removes the need to scan them for their addresses.
	 *
	 * API usage example:
	 * \code
	 * switch (sensor)
	 * {
	 * case SENSOR_LOW:
	 *    // low level sensor (ground) on PE4
	 *    ow_set_bus (&PINE, &PORTE, &DDRE, PE4);
	 *    if (!ow_busy ())                 // see if the conversion is complete
	 *    {
	 *       ow_ds18X20_read_temperature (NULL, &temperature_low);       // read the result
	 *       ow_ds18X20_start (NULL, false]);            // start the conversion process again
	 *    }
	 *    break;
	 * case SENSOR_HIGH:
	 *    // high level (roof) sensor on PE5
	 *    ow_set_bus (&PINE, &PORTE, &DDRE, PE5);
	 *    if (!ow_busy ())                 // see if the conversion is complete
	 *    {
	 *       ow_ds18X20_read_temperature (NULL, &temperature_hi);       // read the result
	 *       ow_ds18X20_start (NULL, false);            // start the conversion process again
	 *    }
	 *    break;
	 * \endcode
	 * \{
	 */

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

	/** \} */ //defgroup 1wirehw_api

/** \} */ //addtogroup ow_driver

#endif

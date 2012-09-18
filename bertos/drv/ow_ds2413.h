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
 * \brief Driver for Dallas dual GPIO 1-wire device
 *
 * \author Robin Gilks <g8ecj@gilks.org>
 *
 * $WIZ$ module_name = "ow_ds2413"
 * $WIZ$ module_depends = "1wire"
 */

#ifndef OW_DS2413_H_
#define OW_DS2413_H_


#include <stdint.h>

#define SSWITCH_FAM  0x3A

	/**
	 * \defgroup gpio_api DS2413 API
	 * With this driver you can read and set the state of the I/O lines
	 * No initialisation is required.
	 *
	 * API usage example:
	 * \code
	 * read = ow_ds2413_read (ids[gpioid]);       // read state of both lines
	 * if (!ow_ds2413_write (ids[gpioid], 2))     // set output bit on one of the lines low
	 * \endcode
	 * \{
	 */

uint8_t ow_ds2413_read(uint8_t id[]);
uint8_t ow_ds2413_write(uint8_t id[], uint8_t state);

	/** \} */ //defgroup gpio_api

/** \} */ //addtogroup ow_driver

#endif

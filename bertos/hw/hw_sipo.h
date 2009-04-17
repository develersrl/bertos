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
 * Copyright 2003, 2004, 2006, 2008, 2009 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2000 Bernie Innocenti <bernie@codewiz.org>
 * All Rights Reserved.
 * -->
 *
 * \brief Macro for HW_SIPO_H
 *
 *
 * \version $Id$
 *
 * \author Andrea Grandi <andrea@develer.com>
 * \author Daniele Basile <asterix@develer.com>
 */

#ifndef HW_SIPO_H
#define HW_SIPO_H

#warning TODO:This is an example implementation, you must implement it!


/**
 * Define the macros needed to set the serial input bit of SIPO device
 * low or high.
 */
#define SIPO_SI_HIGH() do { /* Implement me! */ } while (0)
#define SIPO_SI_LOW()  do { /* Implement me! */ } while (0)

/**
 * Drive pin to load the bit, presented in serial-in pin,
 * into sipo shift register.
 */
#define SIPO_SI_CLOCK() /* Implement me! */

/**
 * Clock the content of shift register to output.
 */
#define SIPO_LOAD()   /* Implement me! */

/**
 * Enable the shift register output.
 */
#define SIPO_ENABLE() /* Implement me! */


/**
 * Do everything needed in order to init the SIPO pins.
 */
#define SIPO_INIT_PIN() \
	do { \
		/* Implement me! */ \
	} while(0)

#endif /* HW_SIPO_H */

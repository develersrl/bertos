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
 * Copyright 2003, 2004, 2006, 2009 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2000 Bernie Innocenti <bernie@codewiz.org>
 *
 * -->
 *
 * \brief Macro for HW_SIPO_H
 *
 *
 * \author Andrea Grandi <andrea@develer.com>
 * \author Daniele Basile <asterix@develer.com>
 */

#ifndef HW_SIPO_H
#define HW_SIPO_H

#include <cfg/macros.h>

#include <avr/io.h>

/**
 * Sipo ids map.
 * See schematics for more info.
 */
typedef enum SipoMap
{
	TRIFACE_DOUT = 0,

	SIPO_CNT
} SipoMap;



//Sets pin for sipo
#define SCK_OUT            (DDRB |= BV(PB1))  // Clock pin
#define SOUT_OUT           (DDRB |= BV(PB2))  // Serial data input pin
#define SLOAD_OUT          (DDRB |= BV(PB3))  // Storage register pin
#define OE_OUT             (DDRG |= BV(PG3))  // Output enable pin

//Define logic levels
#define SIPO_SCK           PB1
#define SIPO_SLOAD         PB3

/**
 * Macros to drive serial input of sipo.
 */
#define SIPO_SI_HIGH()    (PORTB |= BV(PB2))
#define SIPO_SI_LOW()     (PORTB &= ~BV(PB2))

/**
 * Define generic macro to set pins logic level
 */
#define SIPO_SET_LEVEL_LOW(pin)  do { PORTB &= ~BV((pin)); } while (0)
#define SIPO_SET_LEVEL_HIGH(pin) do { PORTB |= BV((pin)); } while (0)


/**
 * Generate one low pulse on select line.
 */
#define PULSE_LOW(pin) \
	do { \
		SIPO_SET_LEVEL_LOW(pin); \
		NOP;NOP;NOP;NOP; \
		SIPO_SET_LEVEL_HIGH(pin); \
	} while (0)

/**
 * Generate one hight pulse on select line.
 */
#define PULSE_HIGH(pin) \
	do { \
		SIPO_SET_LEVEL_HIGH(pin); \
		NOP;NOP;NOP;NOP; \
		SIPO_SET_LEVEL_LOW(pin); \
	} while (0)

/**
 * Clock pulse to shift bits into sipo
 */
#define SIPO_SI_CLOCK(clk_pol) \
	do { \
		if(clk_pol) \
			PULSE_LOW(SIPO_SCK); \
		else \
			PULSE_HIGH(SIPO_SCK); \
	} while (0)

/**
 * Load stored sipo bits to output.
 */
#define SIPO_LOAD(device, load_pol) \
	do { \
		if(load_pol) \
			PULSE_LOW(SIPO_SLOAD); \
		else \
			PULSE_HIGH(SIPO_SLOAD); \
	} while(0)


/**
 * Enable the sipo output
 */
#define SIPO_ENABLE() (PORTG &= BV(PG3));

/**
 * Set load signal level.
 */
#define SIPO_SET_LD_LEVEL(device, load_pol) \
	do { \
		(void)device; \
		if(load_pol) \
			SIPO_SET_LEVEL_HIGH(SIPO_SLOAD); \
		else \
			SIPO_SET_LEVEL_LOW(SIPO_SLOAD); \
	} while (0)


/**
 * Set clock signal level.
 */
#define SIPO_SET_CLK_LEVEL(clock_pol) \
	do { \
		if(clock_pol) \
			SIPO_SET_LEVEL_HIGH(SIPO_SCK); \
		else \
			SIPO_SET_LEVEL_LOW(SIPO_SCK); \
	} while (0)

/**
 * Set serial sipo input level.
 */
#define SIPO_SET_SI_LEVEL()     SIPO_SI_LOW()

/**
 * Do everything needed in order to init the SIPO pins.
 */
#define SIPO_INIT_PIN() \
	do { \
		OE_OUT; \
		SOUT_OUT; \
		SCK_OUT; \
		SLOAD_OUT; \
		SIPO_ENABLE(); \
	} while(0)


#endif /* HW_SIPO_H */

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
 * Mapping sipo connection on board.
 * See schematics for more info.
 */
typedef enum SipoMap
{
	TRIFACE_DOUT = 0,

	SIPO_CNT
} SipoMap;



//Set output pin for sipo
#define SCK_OUT            (DDRB |= BV(PB1))  // Shift register clock input pin
#define SOUT_OUT           (DDRB |= BV(PB2))  // Serial data input pin
#define SLOAD_OUT          (DDRB |= BV(PB3))  // Storage register clock input pin
#define OE_OUT             (DDRG |= BV(PG3))  // Output enable pin

//Define output level
#define SCK_HIGH           (PORTB |= BV(PB1))
#define SCK_LOW            (PORTB &= ~BV(PB1))
#define SOUT_OUT_HIGH      (PORTB |= BV(PB2))
#define SOUT_OUT_LOW       (PORTB &= ~BV(PB2))
#define SLOAD_OUT_HIGH     (PORTB |= BV(PB3))
#define SLOAD_OUT_LOW      (PORTB &= ~BV(PB3))
#define OE_LOW             (PORTG &= BV(PG3))

/**
 * Define the macros needed to set the serial input bit of SIPO device
 * low or high.
 */
#define SIPO_SI_HIGH()    SOUT_OUT_HIGH
#define SIPO_SI_LOW()     SOUT_OUT_LOW

/**
 * Drive pin to load the bit, presented in serial-in pin,
 * into sipo shift register.
 */
#define SIPO_SI_CLOCK(clk_pol) \
	do{ \
		(void)clk_pol; \
		SCK_HIGH; \
		SCK_LOW; \
	}while(0)

/**
 * Clock the content of shift register to output.
 */
#define SIPO_LOAD(device, load_pol) \
	do { \
		(void)device; \
		(void)load_pol; \
		SLOAD_OUT_HIGH; \
		SLOAD_OUT_LOW; \
	}while(0)

/**
 * Enable the shift register output.
 */
#define SIPO_ENABLE() OE_LOW;

/**
 * Set logic level for load signal
 */
#define SIPO_SET_LD_LEVEL(device, load_pol) \
	do { \
		(void)device; \
		if(load_pol) \
			SLOAD_OUT_HIGH; \
		else \
			SLOAD_OUT_LOW; \
	} while (0)


/**
 * Sel logic level for clock signal
 */
#define SIPO_SET_CLK_LEVEL(clock_pol) \
	do { \
		if(clock_pol) \
			SCK_HIGH; \
		else \
			SCK_LOW; \
	} while (0)

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

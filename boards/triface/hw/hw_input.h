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
 * Copyright 2003, 2004, 2006, 2010 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2000 Bernie Innocenti <bernie@codewiz.org>
 *
 * -->
 *
 * \brief Macro for HW_INPUT_H
 *
 * \author Andrea Grandi <andrea@develer.com>
 */

#ifndef HW_INPUT_H
#define HW_INPUT_H

#include <avr/io.h>

/* Set pins as input and enable pull-up */
#define INPUT_INIT_D \
	do{	\
	DDRD &= ~(BV(PD4) | BV(PD5) | BV(PD6) | BV(PD7)); \
	PORTD |= (BV(PD4) | BV(PD5) | BV(PD6) | BV(PD7)); \
	} while(0)

#define INPUT_INIT_E \
	do{	\
		DDRE &= ~(BV(PE4) | BV(PE5) | BV(PE6) | BV(PE7)); \
		PORTE |= (BV(PE4) | BV(PE5) | BV(PE6) | BV(PE7)); \
	} while(0)

INLINE uint8_t INPUT_GET(void)
{
	uint8_t out_d, out_e;
	out_d = PIND;
	out_e = PINE;

	/* Select our input (see scheme) */
	out_d >>= 4;
	out_e = out_e & 0xF0;

	return (out_e | out_d);
}

#define INPUT_INIT() \
	do { \
		INPUT_INIT_D; \
		INPUT_INIT_E; \
	} while(0)

#endif /* HW_INPUT_H */

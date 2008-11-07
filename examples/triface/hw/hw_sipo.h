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
 * Copyright 2003, 2004, 2006 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2000 Bernie Innocenti <bernie@codewiz.org>
 *
 * -->
 *
 * \brief Macro for HW_SIPO_H
 *
 *
 * \version $Id$
 *
 * \author Andrea Grandi <andrea@develer.com>
 */
#ifndef HW_SIPO_H
#define HW_SIPO_H

#define LOAD_HIGH     (PORTB |= BV(PB3))
#define LOAD_LOW      (PORTB &= ~BV(PB3))
#define LOAD_INIT     (DDRB |= BV(PB3))
#define SET_SCK_OUT   (DDRB |= BV(PB1))
#define SET_SOUT_OUT  (DDRB |= BV(PB2))
#define CLOCK_HIGH    (PORTB |= BV(PB1))
#define CLOCK_LOW     (PORTB &= ~BV(PB1))
#define SET_SOUT_HIGH (PORTB |= BV(PB2))
#define SET_SOUT_LOW  (PORTB &= ~BV(PB2))
#define CLOCK_PULSE   do { CLOCK_HIGH; CLOCK_LOW; } while(0)

#define OE_OUT        (DDRG |= BV(PG3))
#define OE_LOW        (PORTG &= BV(PG3))


#endif // HW_SIPO_H

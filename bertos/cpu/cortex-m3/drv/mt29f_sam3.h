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
 * \brief Micron MT29F serial NAND driver for SAM3's static memory controller (interface).
 *
 * \author Stefano Fedrigo <aleph@develer.com>
 */

#ifndef MT29F_SAM3_H
#define MT29F_SAM3_H

// MT29F2G08AAD, FIXME: configurable
#define MT29F_PAGE_SIZE  2048
#define MT29F_SIZE       0x10000000  // 256 MB

/*
 * PIO definitions.
 */
#define MT29F_PIN_CE        BV(6)
#define MT29F_PIN_RB        BV(2)
#define MT29F_PINS_PORTA    (MT29F_PIN_CE | MT29F_PIN_RB)
#define MT29F_PERIPH_PORTA  PIO_PERIPH_B

#define MT29F_PIN_OE        BV(19)
#define MT29F_PIN_WE        BV(20)
#define MT29F_PIN_IO        0x0000FFFF
#define MT29F_PINS_PORTC    (MT29F_PIN_OE | MT29F_PIN_WE | MT29F_PIN_IO)
#define MT29F_PERIPH_PORTC  PIO_PERIPH_A

#define MT29F_PIN_CLE       BV(22)
#define MT29F_PIN_ALE       BV(21)
#define MT29F_PINS_PORTD    (MT29F_PIN_CLE | MT29F_PIN_ALE)
#define MT29F_PERIPH_PORTD  PIO_PERIPH_A

#endif /* MT29F_SAM3_H */


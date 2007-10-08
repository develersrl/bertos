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
 * Copyright 2005 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 * \version $Id$
 *
 * \brief TC520 hardware-specific definitions
 *
 * \version $Id$
 * \author Francesco Sacchi <batt@develer.com>
 */

#ifndef HW_TC520_H
#define HW_TC520_H

#include <cfg/compiler.h>
#include <avr/io.h>

#define CE_PIN   PE6
#define DV_PIN   PE5
#define LOAD_PIN PE3
#define READ_PIN PE4

#define TC520_DDR  DDRE
#define TC520_PORT PORTE
#define TC520_PIN  PINE

#define DV_HIGH() (TC520_PIN & BV(DV_PIN))
#define DV_LOW()  (!DV_HIGH())

#define CE_HIGH() (TC520_PORT |= BV(CE_PIN))
#define CE_LOW()  (TC520_PORT &= ~BV(CE_PIN))

#define LOAD_HIGH() (TC520_PORT |= BV(LOAD_PIN))
#define LOAD_LOW()  (TC520_PORT &= ~BV(LOAD_PIN))

#define READ_HIGH() (TC520_PORT |= BV(READ_PIN))
#define READ_LOW()  (TC520_PORT &= ~BV(READ_PIN))

#define TC520_HW_INIT \
do\
{\
	TC520_PORT |= (BV(CE_PIN) | BV(LOAD_PIN) | BV(READ_PIN));\
	TC520_DDR  |= (BV(CE_PIN) | BV(LOAD_PIN) | BV(READ_PIN));\
	TC520_DDR  &= ~BV(DV_PIN);\
}\
while(0)

#endif /* HW_TC520_H */

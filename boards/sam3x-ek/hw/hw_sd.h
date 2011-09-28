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
 * Copyright 2009 Develer S.r.l. (http://www.develer.com/)
 * All Rights Reserved.
 * -->
 *
 * \brief SD driver hardware-specific definitions.
 *
 *
 * \author Luca Ottaviano <lottaviano@develer.com>
 */

#ifndef HW_SD_H
#define HW_SD_H

#define SD_CS_INIT() do { /* implement me */} while(0)
#define SD_CS_ON()   do { /* implement me */} while(0)
#define SD_CS_OFF()  do { /* implement me */} while(0)

#include <io/cm3.h>

#define SD_STROBE_ON()   do { PIOE_SODR = BV(0); } while(0)
#define SD_STROBE_OFF()  do { PIOE_CODR = BV(0); } while(0)

#define SD_STROBE_INIT() \
	do { \
		PIOE_PER = BV(0); \
		PIOE_OER = BV(0);\
		PIOE_CODR = BV(0); \
	} while(0)

#define SD_PIN_INIT()  \
do { \
	PIOA_PDR = BV(19) | BV(20) | BV(21) | BV(22) | BV(23) | BV(24); \
	PIO_PERIPH_SEL(PIOA_BASE, BV(19) | BV(20) | BV(21) | BV(22) | BV(23) | BV(24), PIO_PERIPH_A); \
	pmc_periphEnable(PIOE_ID); \
	PIOE_PUER = BV(6); \
	PIOE_ODR = BV(6); \
	PIOE_PER = BV(6); \
} while (0)

#define SD_CARD_PRESENT() (!(PIOE_PDSR & BV(6)))
#define SD_WRITE_PROTECT() false /* implement me */

#endif /* HW_SD_H */

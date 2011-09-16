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
 * All Rights Reserved.
 * -->
 *
 * \brief HW pin handling for WM8731 audio codec.
 *
 *
 * \author Daniele Basile <asterix@develer.com>
 */

#ifndef HW_WM8731_H
#define HW_WM8731_H

#include <cfg/macros.h>
#include <io/cm3.h>

#define WM8731_PIN_INIT() \
	do { \
		/* implement me */ \
	} while (0)

#define WM8731_MCLK_INIT() \
	do { \
		/* Disable PIO on PB22 connected to mclk of the wm8731 codec */ \
		PIOB_PDR = BV(22); \
		/* Select the output clock on same pin */ \
		PIO_PERIPH_SEL(PIOB_BASE, BV(22), PIO_PERIPH_B); \
		/* Prescale the cpu clock to the frequenzy that we want. */\
		/* (multiply value of audio sample  rate) */\
		PMC_PCK0 = PMC_PCK_CSS_PLL | PMC_PCK_PRES_CLK_8; \
		/* Enable the clock and wait until it starts correctly */ \
		PMC_SCER = BV(PMC_PCK0_ID); \
		while ((PMC_SR & BV(PMC_PCKRDY0)) == 0); \
	} while(0)


#endif /* HW_WM8731_H */

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
 * \brief HW pin handling.
 *
 * \version $Id$
 *
 * \author Francesco Sacchi <batt@develer.com>
 */

#ifndef HW_TAS5706A_H
#define HW_TAS5706A_H

#include <io/arm.h>
#include <cfg/macros.h>

#define TAS5706A_SETPOWERDOWN(val) do { if (val) PIOA_CODR = BV(6); else PIOA_SODR = BV(6); } while (0)
#define TAS5706A_SETRESET(val)     do { if (val) PIOA_CODR = BV(7); else PIOA_SODR = BV(7); } while (0)
#define TAS5706A_SETMUTE(val)      do { if (val) PIOA_CODR = BV(8); else PIOA_SODR = BV(8); } while (0)

#define TAS5706A_PIN_INIT() \
	do { \
		TAS5706A_SETPOWERDOWN(true); \
		TAS5706A_SETRESET(true); \
		TAS5706A_SETMUTE(true); \
		PIOA_PER = BV(6) | BV(7) | BV(8); \
		PIOA_OER = BV(6) | BV(7) | BV(8); \
	} while (0)

#define TAS5706A_MCLK_INIT() \
	do { \
		PIOA_PDR = BV(2); /* enable PWM pin */ \
		PWM_CMR2 = 0; /* set prescaler to MCK, left aligned, start with low level */ \
		PWM_CPRD2 = 4; /* 11.2896 MHz MCLK */ \
		PWM_CDTY2 = 2; /* 50% duty */ \
		PWM_ENA = BV(2); /* Enable PWM on MCLK pin */ \
	} while(0)


#endif /* HW_TAS5706A_H */

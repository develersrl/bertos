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
 * Copyright 2010 Develer S.r.l. (http://www.develer.com/)
 * All Rights Reserved.
 * -->
 *
 * \brief Led on/off macros.
 *
 * \author Daniele Basile <asterix@develer.com>
 */

#ifndef HW_LED_H
#define HW_LED_H

#include <cfg/macros.h>

#include <io/lm3s.h>

#define LED_ON()   GPIO_PORTG_DATA_R |= BV(2)
#define LED_OFF()  GPIO_PORTG_DATA_R &= ~BV(2)

#define LED_INIT() \
	do { \
		SYSCTL_RCGC2_R = SYSCTL_RCGC2_GPIOG; \
		(void)SYSCTL_RCGC2_R; \
		GPIO_PORTG_DIR_R = BV(2); \
		GPIO_PORTG_DEN_R = BV(2); \
	} while(0)

#endif /* HW_LED_H */

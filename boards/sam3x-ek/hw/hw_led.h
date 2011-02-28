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
 * \brief Led on/off macros.
 *
 * \author Stefano Fedrigo <aleph@develer.com>
 */

#ifndef HW_LED_H
#define HW_LED_H

#include <io/cm3.h>

#include <cfg/macros.h>

/*
 * Note that power led (RED) is on by default.
 */
#define LED_GREEN    0
#define LED_AMBER    1
#define LED_BLUE     2
#define LED_RED      3
#define LED_DEFAULT  LED_GREEN

// Leds
#define LED_GREEN_PIN      BV(13)  // Port B
#define LED_AMBER_PIN      BV(12)  // Port B
#define LED_BLUE_PIN       BV(12)  // Port A
#define LED_RED_PIN        BV(13)  // Port A

#define LED_ON(led) \
	do { \
		if (led == LED_GREEN) \
			PIOB_CODR = LED_GREEN_PIN; \
		else if (led == LED_AMBER) \
			PIOB_CODR = LED_AMBER_PIN; \
		else if (led == LED_BLUE) \
			PIOA_CODR = LED_BLUE_PIN; \
		else if (led == LED_RED) \
			PIOA_SODR = LED_RED_PIN; \
		else \
			ASSERT(0); \
	} while (0)


#define LED_OFF(led) \
	do { \
		if (led == LED_GREEN) \
			PIOB_SODR = LED_GREEN_PIN; \
		else if (led == LED_AMBER) \
			PIOB_SODR = LED_AMBER_PIN; \
		else if (led == LED_BLUE) \
			PIOA_SODR = LED_BLUE_PIN; \
		else if (led == LED_RED) \
			PIOA_CODR = LED_RED_PIN; \
		else \
			ASSERT(0); \
	} while (0)


#define LED_INIT() \
	do { \
		PIOA_SODR = LED_BLUE_PIN; \
		PIOA_CODR = LED_RED_PIN; \
		PIOA_OER = LED_BLUE_PIN | LED_RED_PIN; \
		PIOA_PER = LED_BLUE_PIN | LED_RED_PIN; \
		PIOB_SODR = LED_GREEN_PIN | LED_AMBER_PIN; \
		PIOB_OER = LED_GREEN_PIN | LED_AMBER_PIN; \
		PIOB_PER = LED_GREEN_PIN | LED_AMBER_PIN; \
	} while(0)

#endif /* HW_LED_H */

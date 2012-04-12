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
 * Copyright 2007, 2010 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 *
 * \author Tóth Balázs <balazs.toth@jarkon.hu>
 *
 * \brief Low-level PCINT module header for AVR MEGA.
 *
 */

#ifndef DRV_PCINT_MEGA_H
#define DRV_PCINT_MEGA_H


#include "cfg/cfg_pcint.h"
#include "hw/hw_pcint.h"

#include <cfg/macros.h> /* BV() */

#include <drv/pcint.h>

#include <avr/io.h>

#if CPU_AVR_ATMEGA1281
	#define AVR_HAS_PCINT0 1
	#define AVR_HAS_PCINT1 0
	#define AVR_HAS_PCINT2 0
	#define AVR_HAS_PCINT3 0
#elif CPU_AVR_ATMEGA88P || CPU_AVR_ATMEGA168 || CPU_AVR_ATMEGA328P \
	|| CPU_AVR_ATMEGA1280 || CPU_AVR_ATMEGA2560
	#define AVR_HAS_PCINT0 1
	#define AVR_HAS_PCINT1 1
	#define AVR_HAS_PCINT2 1
	#define AVR_HAS_PCINT3 0
#elif CPU_AVR_ATMEGA324P || CPU_AVR_ATMEGA644P
	#define AVR_HAS_PCINT0 1
	#define AVR_HAS_PCINT1 1
	#define AVR_HAS_PCINT2 1
	#define AVR_HAS_PCINT3 1
#else // #elif CPU_AVR_ATMEGA103 || CPU_AVR_ATMEGA128
	// || CPU_AVR_ATMEGA32 || CPU_AVR_ATMEGA64 || CPU_AVR_ATMEGA8
	#define AVR_HAS_PCINT0 0
	#define AVR_HAS_PCINT1 0
	#define AVR_HAS_PCINT2 0
	#define AVR_HAS_PCINT3 0

	#error unsupported device

#endif


/**
 * \name Pin change interrupt numbers
 *
 * \{
 */
enum
{
#if CPU_AVR_ATMEGA1281
	PCINT_0,
#elif CPU_AVR_ATMEGA88P || CPU_AVR_ATMEGA168 || CPU_AVR_ATMEGA328P \
	|| CPU_AVR_ATMEGA1280 || CPU_AVR_ATMEGA2560
	PCINT_0,
	PCINT_1,
	PCINT_2,
#elif CPU_AVR_ATMEGA324P || CPU_AVR_ATMEGA644P
	PCINT_0,
	PCINT_1,
	PCINT_2,
	PCINT_3,
#else  // CPU_AVR_ATMEGA103 || CPU_AVR_ATMEGA128 || CPU_AVR_ATMEGA32 || CPU_AVR_ATMEGA64 || CPU_AVR_ATMEGA8
	#error unsupported device
#endif
	PCINT_CNT    /**< Number of pin change interrupts */

};
/*\}*/




#if	AVR_HAS_PCINT0

INLINE void pcint0_init(void)
{

#if CONFIG_PCINT_STROBE
	PCINT_STROBE_INIT;
#endif

	PCMSK0 = 0;
	PCICR |= BV(PCIE0);

}

INLINE void pcint0_setmask(pcint_mask_t mask)
{
	PCMSK0 = mask;
}

#endif /* AVR_HAS_PCINT0 */


#if	AVR_HAS_PCINT1

INLINE void pcint1_init(void)
{

#if CONFIG_PCINT_STROBE
	PCINT_STROBE_INIT;
#endif

	PCMSK1 = 0;
	PCICR |= BV(PCIE1);

}

INLINE void pcint1_setmask(pcint_mask_t mask)
{
	PCMSK1 = mask;
}

#endif /* AVR_HAS_PCINT1 */


#if	AVR_HAS_PCINT2

INLINE void pcint2_init(void)
{

#if CONFIG_PCINT_STROBE
	PCINT_STROBE_INIT;
#endif

	PCMSK2 = 0;
	PCICR |= BV(PCIE2);

}

INLINE void pcint2_setmask(pcint_mask_t mask)
{
	PCMSK2 = mask;
}

#endif /* AVR_HAS_PCINT2 */


#if	AVR_HAS_PCINT3

INLINE void pcint3_init(void)
{

#if CONFIG_PCINT_STROBE
	PCINT_STROBE_INIT;
#endif

	PCMSK3 = 0;
	PCICR |= BV(PCIE3);

}

INLINE void pcint3_setmask(pcint_mask_t mask)
{
	PCMSK3 = mask;
}

#endif /* AVR_HAS_PCINT3 */

#endif /* DRV_PCINT_MEGA_H */

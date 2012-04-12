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
 * \brief Low-level PCINT module for AVR MEGA.
 *
 * \author Tóth Balázs <balazs.toth@jarkon.hu>
 *
 * notest:avr
 * notest:arm
 */

#include <drv/pcint.h>

#include "cfg/cfg_pcint.h"
#include "hw/hw_pcint.h"

#include <cpu/irq.h>

#include <avr/io.h>

#if defined(__AVR_LIBC_VERSION__) && (__AVR_LIBC_VERSION__ >= 10400UL)
	#include <avr/interrupt.h>
#else
	#include <avr/signal.h>
#endif


/**
 * Hardware dependent init.
 */
void pcint_hw_init(uint8_t unit)
{
	switch(unit)
	{
		#if	AVR_HAS_PCINT0
		case PCINT_0:
		{
			pcint0_init();
			break;
		}
		#endif
		#if	AVR_HAS_PCINT1
		case PCINT_1:
		{
			pcint1_init();
			break;
		}
		#endif
		#if	AVR_HAS_PCINT2
		case PCINT_2:
		{
			pcint2_init();
			break;
		}
		#endif
		#if	AVR_HAS_PCINT3
		case PCINT_3:
		{
			pcint3_init();
			break;
		}
		#endif
	}
}


/**
 * Hardware dependent mask setting.
 */
void pcint_hw_setmask(uint8_t unit, pcint_mask_t mask)
{
	switch(unit)
	{
		#if	AVR_HAS_PCINT0
		case PCINT_0:
		{
			pcint0_setmask(mask);
			break;
		}
		#endif
		#if	AVR_HAS_PCINT1
		case PCINT_1:
		{
			pcint1_setmask(mask);
			break;
		}
		#endif
		#if	AVR_HAS_PCINT2
		case PCINT_2:
		{
			pcint2_setmask(mask);
			break;
		}
		#endif
		#if	AVR_HAS_PCINT3
		case PCINT_3:
		{
			pcint3_setmask(mask);
			break;
		}
		#endif
	}
}



#if	AVR_HAS_PCINT0

DECLARE_ISR(PCINT0_vect)
{

#if CONFIG_PCINT_STROBE
	PCINT_STROBE_ON;
#endif

	PCINT0_ISR;

#if CONFIG_PCINT_STROBE
	PCINT_STROBE_OFF;
#endif

	IRQ_ENABLE;
}

#endif /* AVR_HAS_PCINT0 */


#if	AVR_HAS_PCINT1

DECLARE_ISR(PCINT1_vect)
{

#if CONFIG_PCINT_STROBE
	PCINT_STROBE_ON;
#endif

	PCINT1_ISR;

#if CONFIG_PCINT_STROBE
	PCINT_STROBE_OFF;
#endif

	IRQ_ENABLE;
}

#endif /* AVR_HAS_PCINT1 */


#if	AVR_HAS_PCINT2

DECLARE_ISR(PCINT2_vect)
{

#if CONFIG_PCINT_STROBE
	PCINT_STROBE_ON;
#endif

	PCINT2_ISR;

#if CONFIG_PCINT_STROBE
	PCINT_STROBE_OFF;
#endif

	IRQ_ENABLE;
}

#endif /* AVR_HAS_PCINT2 */


#if	AVR_HAS_PCINT3

DECLARE_ISR(PCINT3_vect)
{

#if CONFIG_PCINT_STROBE
	PCINT_STROBE_ON;
#endif

	PCINT3_ISR;

#if CONFIG_PCINT_STROBE
	PCINT_STROBE_OFF;
#endif

	IRQ_ENABLE;
}

#endif /* AVR_HAS_PCINT3 */

/*!
 * \file
 * <!--
 * Copyright 2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2004 Giovanni Bajo
 * This file is part of DevLib - See devlib/README for information.
 * -->
 *
 * \brief Hardware support for buzzers and leds in DSP56K-based boards
 *
 * \version $Id$
 *
 * \author Giovanni Bajo <rasky@develer.com>
 */

/*#*
 *#* $Log$
 *#* Revision 1.3  2004/08/25 14:12:08  rasky
 *#* Aggiornato il comment block dei log RCS
 *#*
 *#* Revision 1.2  2004/06/03 11:27:09  bernie
 *#* Add dual-license information.
 *#*
 *#* Revision 1.1  2004/05/23 18:36:05  bernie
 *#* Import buzzerled driver.
 *#*
 *#*/

#include <compiler.h>
#include <hw.h>
#include "pwm.h"

INLINE void bld_hw_init(void)
{
}

INLINE void bld_hw_set(enum BLD_DEVICE device, bool enable)
{
	if (bld_is_inverted_intensity(device))
		enable = !enable;

	// Handle a BLD connected to a PWM
	if (bld_is_pwm(device))
	{
		struct PWM* pwm = pwm_get_handle(bld_get_pwm(device));

		pwm_set_enable(pwm, false);
		pwm_set_dutycycle_percent(pwm, (enable ? 50 : 0));
		pwm_set_enable(pwm, true);
	}
	else if (bld_is_timer(device))
	{
		struct REG_TIMER_STRUCT* timer = bld_get_timer(device);

		// Check that the timer is currently stopped, and the OFLAG is not
		//  controlled by another timer. Otherwise, the led is already 
		//  controlled by the timer, and we cannot correctly set it 
		//  on/off without reprogramming the timer.
		ASSERT((timer->CTRL & REG_TIMER_CTRL_MODE_MASK) == REG_TIMER_CTRL_MODE_STOP);
		ASSERT(!(timer->SCR & REG_TIMER_SCR_EEOF));

		// Check also that polarity is correct
		ASSERT(!(timer->SCR & REG_TIMER_SCR_OPS));

		// Without programming the timer, we have a way to manually force a certain
		//  value on the external pin. We also need to enable the output pin.
		timer->SCR &= ~REG_TIMER_SCR_VAL_1;
		timer->SCR |= REG_TIMER_SCR_OEN |
		              REG_TIMER_SCR_FORCE |
		              (!enable ? REG_TIMER_SCR_VAL_0 : REG_TIMER_SCR_VAL_1);
	}
	else
		ASSERT(0);
}


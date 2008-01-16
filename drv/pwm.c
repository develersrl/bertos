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
 * \brief PWM driver (implementation)
 *
 * \version $Id$
 * \author Francesco Sacchi <batt@develer.com>
 */

#include <hw_pwm.h>
#include <drv/pwm.h>
#include <cfg/macros.h>

/**
 * Set duty of pwm channel \a dev.
 */
void pwm_setDuty(PwmDev dev, pwm_duty_t duty)
{
	duty = MIN(duty, (pwm_duty_t)PWM_MAX_DUTY);

	pwm_hw_setDutyUnlock(dev, duty);
}

/**
 * Initialize PWM hw.
 */
void pwm_init(void)
{
	cpuflags_t flags;
	PwmDev dev;

	IRQ_SAVE_DISABLE(flags);

	/* set all pwm to 0 */
	for (dev = 0; dev < PWM_CNT; dev++)
		pwm_setDuty(dev, 0);

	PWM_HW_INIT;

	IRQ_RESTORE(flags);
}

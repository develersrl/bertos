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
 *
 * \brief PWM driver (implementation)
 *
 *
 * \author Francesco Sacchi <batt@develer.com>
 * \author Daniele Basile <asterix@develer.com>
 */

#include "cfg/cfg_pwm.h"

#include <cfg/macros.h>
#include <cfg/module.h>

// Define logging setting (for cfg/log.h module).
#define LOG_LEVEL         PWM_LOG_LEVEL
#define LOG_VERBOSITY     PWM_LOG_FORMAT

#include <cfg/log.h>
#include <cfg/debug.h>

#include <drv/pwm.h>

#include CPU_HEADER(pwm)

#include <cpu/types.h>
#include <cpu/irq.h>


/**
 * Set duty of pwm channel \p dev.
 */
void pwm_setDuty(PwmDev dev, pwm_duty_t duty)
{
	pwm_period_t period = 0;
	pwm_duty_t real_duty = 0;

	duty = MIN(duty, PWM_MAX_DUTY);

	period = pwm_hw_getPeriod(dev);

	real_duty = (uint64_t)(duty * period) >> (uint64_t)PWM_MAX_PERIOD_LOG2;

	LOG_INFO("real_duty[%d] duty[%d], period[%d]\n", real_duty, duty, period);
	pwm_hw_setDutyUnlock(dev, real_duty);
}

/**
 * Set frequency of pwm channel \p dev at \p freq in Hz.
 */
void pwm_setFrequency(PwmDev dev, pwm_freq_t freq)
{
	pwm_hw_setFrequency(dev, freq);
}

/**
 * Set duty of pwm channel \p dev.
 */
void pwm_enable(PwmDev dev, bool state)
{
	if (state)
		pwm_hw_enable(dev);
	else
		pwm_hw_disable(dev);
}

MOD_DEFINE(pwm);

/**
 * Initialize PWM hw.
 */
void pwm_init(void)
{
	cpu_flags_t flags;
	PwmDev dev;

	IRQ_SAVE_DISABLE(flags);

	pwm_hw_init();

	/* set all pwm to 0 */
	for (dev = 0; dev < PWM_CNT; dev++)
		pwm_setDuty(dev, 0);

	IRQ_RESTORE(flags);
	MOD_INIT(pwm);
}





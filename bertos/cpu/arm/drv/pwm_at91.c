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
 * Copyright 2008 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 *
 * \brief PWM hardware-specific implementation
 *
 * \author Daniele Basile <asterix@develer.com>
 */

#include "pwm_at91.h"
#include "hw/pwm_map.h"
#include <hw/hw_cpufreq.h>
#include "cfg/cfg_pwm.h"

// Define logging setting (for cfg/log.h module).
#define LOG_LEVEL         PWM_LOG_LEVEL
#define LOG_FORMAT        PWM_LOG_FORMAT
#include <cfg/log.h>

#include <cfg/macros.h>
#include <cfg/debug.h>

#include <io/arm.h>


/**
 * Register structure for pwm driver.
 * This array content all data and register pointer
 * to manage pwm peripheral device.
 */
static PwmChannel pwm_map[PWM_CNT] =
{
	{//PWM Channel 0
		.duty_zero = false,
		.pol = false,
		.pwm_pin = BV(PWM0),
		.mode_reg = &PWM_CMR0,
		.duty_reg = &PWM_CDTY0,
		.period_reg = &PWM_CPRD0,
		.update_reg = &PWM_CUPD0,
	},
	{//PWM Channel 1
		.duty_zero = false,
		.pol = false,
		.pwm_pin = BV(PWM1),
		.mode_reg = &PWM_CMR1,
		.duty_reg = &PWM_CDTY1,
		.period_reg = &PWM_CPRD1,
		.update_reg = &PWM_CUPD1,
	},
	{//PWM Channel 2
		.duty_zero = false,
        .pol = false,
		.pwm_pin = BV(PWM2),
		.mode_reg = &PWM_CMR2,
		.duty_reg = &PWM_CDTY2,
		.period_reg = &PWM_CPRD2,
		.update_reg = &PWM_CUPD2,
	},
	{//PWM Channel 3
		.duty_zero = false,
		.pol = false,
		.pwm_pin = BV(PWM3),
		.mode_reg = &PWM_CMR3,
		.duty_reg = &PWM_CDTY3,
		.period_reg = &PWM_CPRD3,
		.update_reg = &PWM_CUPD3,
	}
};


/**
 * Get preiod from select channel
 *
 * \a dev channel
 */
pwm_period_t pwm_hw_getPeriod(PwmDev dev)
{
	return *pwm_map[dev].period_reg;
}

/**
 * Set pwm waveform frequecy.
 *
 * \a freq in Hz
 */
void pwm_hw_setFrequency(PwmDev dev, uint32_t freq)
{
	uint32_t period = 0;

	for(int i = 0; i <= PWM_HW_MAX_PRESCALER_STEP; i++)
	{
		period = CPU_FREQ / (BV(i) * freq);
 		LOG_INFO("period[%ld], prescale[%d]\n", period, i);
		if ((period < PWM_HW_MAX_PERIOD) && (period != 0))
		{
			//Clean previous channel prescaler, and set new
			*pwm_map[dev].mode_reg &= ~PWM_CPRE_MCK_MASK;
			*pwm_map[dev].mode_reg |= i;
			//Set pwm period
			*pwm_map[dev].period_reg = period;
			break;
		}
	}

 	LOG_INFO("PWM ch[%d] period[%ld]\n", dev, period);
}

/**
 * Set pwm duty cycle.
 *
 * \a duty value 0 - 2^16
 */
void pwm_hw_setDutyUnlock(PwmDev dev, uint16_t duty)
{
	ASSERT(duty <= (uint16_t)*pwm_map[dev].period_reg);


	/*
	 * If polarity flag is true we must invert
	 * PWM polarity.
	 */
	if (pwm_map[dev].pol)
	{
		duty = (uint16_t)*pwm_map[dev].period_reg - duty;
		LOG_INFO("Inverted duty[%d], pol[%d]\n", duty, pwm_map[dev].pol);
	}

	/*
	 * WARNING: is forbidden to write 0 to duty cycle value,
	 * and so for duty = 0 we must enable PIO and clear output!
	 */
	if (!duty)
	{
		PWM_PIO_CODR = pwm_map[dev].pwm_pin;
		PWM_PIO_PER  = pwm_map[dev].pwm_pin;
		pwm_map[dev].duty_zero = true;
	}
	else
	{
		PWM_PIO_PDR = pwm_map[dev].pwm_pin;
		PWM_PIO_ABSR = pwm_map[dev].pwm_pin;

		*pwm_map[dev].update_reg = duty;
		pwm_map[dev].duty_zero = false;
	}

	PWM_ENA = BV(dev);
	LOG_INFO("PWM ch[%d] duty[%d], period[%ld]\n", dev, duty, *pwm_map[dev].period_reg);
}


/**
 * Enable select pwm channel
 */
void pwm_hw_enable(PwmDev dev)
{
	if (!pwm_map[dev].duty_zero)
	{
		PWM_PIO_PDR  = pwm_map[dev].pwm_pin;
		PWM_PIO_ABSR = pwm_map[dev].pwm_pin;
	}
}

/**
 * Disable select pwm channel
 */
void pwm_hw_disable(PwmDev dev)
{
	PWM_PIO_PER = pwm_map[dev].pwm_pin;
}

/**
 * Set PWM polarity to select pwm channel
 */
void pwm_hw_setPolarity(PwmDev dev, bool pol)
{
        pwm_map[dev].pol = pol;
		LOG_INFO("Set pol[%d]\n", pwm_map[dev].pol);
}

/**
 * Init pwm.
 */
void pwm_hw_init(void)
{

	/*
	 * Init pwm:
	 * WARNING: is forbidden to write 0 to duty cycle value,
	 * and so for duty = 0 we must enable PIO and clear output!
	 * - clear PIO outputs
	 * - enable PIO outputs
	 * - Disable PIO and enable PWM functions
	 * - Power on PWM
	 */
	PWM_PIO_CODR = BV(PWM0) | BV(PWM1) | BV(PWM2) | BV(PWM3);
	PWM_PIO_OER  = BV(PWM0) | BV(PWM1) | BV(PWM2) | BV(PWM3);
	PWM_PIO_PDR  = BV(PWM0) | BV(PWM1) | BV(PWM2) | BV(PWM3);
	PWM_PIO_ABSR = BV(PWM0) | BV(PWM1) | BV(PWM2) | BV(PWM3);
	PMC_PCER |= BV(PWMC_ID);

	/* Disable all channels. */
	PWM_DIS = 0xFFFFFFFF;
	/* Disable prescalers A and B */
	PWM_MR = 0;

	/*
	 * Set pwm mode:
	 * - set period alidned to left
	 * - set output waveform to start at high level
	 * - allow duty cycle modify at next period event
	 */
	for (int ch = 0; ch < PWM_CNT; ch++)
	{
		*pwm_map[ch].mode_reg = 0;
		*pwm_map[ch].mode_reg = BV(PWM_CPOL);
	}

}


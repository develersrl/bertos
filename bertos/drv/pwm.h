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
 * \brief Pulse Width Modulation (PWM) driver.
 *
 * \author Francesco Sacchi <batt@develer.com>
 * \author Daniele Basile <asterix@develer.com>
 *
 * $WIZ$ module_name = "pwm"
 * $WIZ$ module_configuration = "bertos/cfg/cfg_pwm.h"
 * $WIZ$ module_hw = "bertos/hw/pwm_map.h"
 * $WIZ$ module_supports = "not avr and not cm3"
 */

#ifndef DRV_PWM_H
#define DRV_PWM_H

#include "hw/pwm_map.h"

#include <cpu/attr.h>

#include CPU_HEADER(pwm)

#include <cfg/compiler.h>

#define PWM_MAX_DUTY              ((pwm_duty_t)0xFFFF)
#define PWM_MAX_PERIOD                         0xFFFF
#define PWM_MAX_PERIOD_LOG2                        16

/**
 * PWM type define.
 */
typedef uint16_t pwm_duty_t;
typedef uint32_t pwm_freq_t;



/**
 * Set PWM polarity of pwm \p dev.
 */
INLINE void pwm_setPolarity(PwmDev dev, bool pol)
{
    pwm_hw_setPolarity(dev, pol);
}

void pwm_setDuty(PwmDev dev, pwm_duty_t duty);
void pwm_setFrequency(PwmDev dev, pwm_freq_t freq);
void pwm_setPolarity(PwmDev dev, bool pol);
void pwm_enable(PwmDev dev, bool state);
void pwm_init(void);

/**
 * Test function prototypes.
 *
 * See pwm_test.c for implemntation of these functions.
 */
void pwm_testRun(void);
int pwm_testSetup(void);
/* For backward compatibility */
#define pwm_testSetUp() pwm_testSetup()
int pwm_testTearDown(void);

#endif /* DRV_PWM_H */

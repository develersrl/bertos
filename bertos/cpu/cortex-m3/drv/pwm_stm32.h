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
 * Copyright 2012 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 *
 * \brief PWM hardware-specific definition
 *
 *
 * \author Mattia Barbon <mattia@develer.com>
 */

#ifndef DRV_PWM_STM32_H
#define DRV_PWM_STM32_H

#include <cfg/compiler.h>

#include "cfg/cfg_pwm.h"

#include <io/stm32.h>

#if CFG_PWM_ENABLE_OLD_API

	#error "Old PWM API not supported"

#else
	/*
	 * This driver allows up to 36 channels (6 timers with 4 channels, 6 with 2 channels)
	 * however the output frequency must be the same for channels on the same timer
	 * and if using a timer for another purpose, the timer frequency will also set
	 * PWM frequency
	 */
	#include <drv/pwm.h>

	typedef uint16_t pwm_hwreg_t;

	struct PwmTimer;

	typedef struct PwmHardware
	{
		uint8_t gpio;
		uint8_t gpio_pin;
		uint8_t pwm_ctr;
		struct PwmTimer *base;
	} PwmHardware;

	pwm_hwreg_t pwm_hw_getPeriod(Pwm *ctx);
	void pwm_hw_setFrequency(struct Pwm *ctx, pwm_freq_t freq);
	void pwm_hw_setDuty(Pwm *ctx, pwm_hwreg_t duty);
	void pwm_hw_init(struct Pwm *ctx, unsigned ch);

#endif

#endif /* DRV_PWM_STM32_H */

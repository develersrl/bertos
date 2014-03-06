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
 * \brief PWM hardware-specific implementation
 *
 * \author Mattia Barbon <mattia@develer.com>
 */

#include <drv/pwm.h>
#include "clock_stm32.h"
#include "gpio_stm32.h"
#include "pwm_stm32.h"

// Define logging setting (for cfg/log.h module).
#define LOG_LEVEL         PWM_LOG_LEVEL
#define LOG_FORMAT        PWM_LOG_FORMAT
#include <cfg/log.h>

#if CFG_PWM_ENABLE_OLD_API

	#error "Old PWM API not supported"

#else
	#define PWM_HW_MAX_PRESCALER  0xFFFF
	#define PWM_HW_MAX_PERIOD     0XFFFF
	#define PWM_HW_PRESCALER_STEP 1

	typedef struct PwmTimer
	{
		reg32_t *peripheral;
		uint32_t enable;
		uint32_t freq;
		uint8_t gpio_af;
		struct stm32_tim *tim;
	} PwmTimer;

	/*
	 * Set pwm waveform frequecy.
	 */
	void pwm_hw_setFrequency(Pwm *ctx, pwm_freq_t freq)
	{
		uint32_t period = 0;

		for(int i = 0; i <= PWM_HW_MAX_PRESCALER; i += PWM_HW_PRESCALER_STEP)
		{
			period = ctx->hw->base->freq / ((i + 1) * freq);
			LOG_INFO("period[%ld], prescale[%d]\n", period, i + 1);
			if ((period < PWM_HW_MAX_PERIOD) && (period != 0))
			{
				// set prescaler and period
				ctx->hw->base->tim->PSC = i;
				ctx->hw->base->tim->ARR = period;
				/* reload preload registers (sets counters to 0) */
				ctx->hw->base->tim->EGR |= TIM_EGR_UG;
				break;
			}
		}

		LOG_INFO("PWM ch[%d] period[%ld]\n", ctx->ch, period);
	}

	pwm_hwreg_t pwm_hw_getPeriod(Pwm *ctx)
	{
		return ctx->hw->base->tim->ARR;
	}

	/*
	 * Set pwm duty cycle.
	 *
	 * duty value 0 - (2^16 - 1)
	 */
	void pwm_hw_setDuty(Pwm *ctx, pwm_hwreg_t hw_duty)
	{
		ctx->hw->base->tim->CCR[ctx->hw->pwm_ctr] = hw_duty;
		/* (re)start counter */
		ctx->hw->base->tim->CR1 |= TIM_CR1_CEN;
	}

	static PwmTimer pwm_timers[] =
	{
		{
			.peripheral = &RCC->APB2ENR,
			.enable = RCC_APB2ENR_TIM1EN,
			.freq = APB2_TIMER_FREQ,
			.tim = TIM1,
			.gpio_af = GPIO_AF_TIM1,
		},
		{
			.peripheral = &RCC->APB1ENR,
			.enable = RCC_APB1ENR_TIM2EN,
			.freq = APB1_TIMER_FREQ,
			.tim = TIM2,
			.gpio_af = GPIO_AF_TIM2,
		},
		{
			.peripheral = &RCC->APB1ENR,
			.enable = RCC_APB1ENR_TIM3EN,
			.freq = APB1_TIMER_FREQ,
			.tim = TIM3,
			.gpio_af = GPIO_AF_TIM3,
		},
		{
			.peripheral = &RCC->APB1ENR,
			.enable = RCC_APB1ENR_TIM4EN,
			.freq = APB1_TIMER_FREQ,
			.tim = TIM4,
			.gpio_af = GPIO_AF_TIM4,
		},
		{
			.peripheral = &RCC->APB1ENR,
			.enable = RCC_APB1ENR_TIM5EN,
			.freq = APB1_TIMER_FREQ,
			.tim = TIM5,
			.gpio_af = GPIO_AF_TIM5,
		},
		/* TIM6 and TIM7 can't be used for PWM, kept here to preserve indices */
		{
			.peripheral = &RCC->APB1ENR,
			.enable = RCC_APB1ENR_TIM6EN,
			.freq = APB1_TIMER_FREQ,
			.tim = TIM6,
		},
		{
			.peripheral = &RCC->APB1ENR,
			.enable = RCC_APB1ENR_TIM7EN,
			.freq = APB1_TIMER_FREQ,
			.tim = TIM7,
		},
		{
			.peripheral = &RCC->APB2ENR,
			.enable = RCC_APB2ENR_TIM8EN,
			.freq = APB2_TIMER_FREQ,
			.tim = TIM8,
			.gpio_af = GPIO_AF_TIM8,
		},
		{
			.peripheral = &RCC->APB2ENR,
			.enable = RCC_APB2ENR_TIM9EN,
			.freq = APB2_TIMER_FREQ,
			.tim = TIM9,
			.gpio_af = GPIO_AF_TIM9,
		},
		{
			.peripheral = &RCC->APB2ENR,
			.enable = RCC_APB2ENR_TIM10EN,
			.freq = APB2_TIMER_FREQ,
			.tim = TIM10,
			.gpio_af = GPIO_AF_TIM10,
		},
		{
			.peripheral = &RCC->APB2ENR,
			.enable = RCC_APB2ENR_TIM11EN,
			.freq = APB2_TIMER_FREQ,
			.tim = TIM11,
			.gpio_af = GPIO_AF_TIM11,
		},
		{
			.peripheral = &RCC->APB1ENR,
			.enable = RCC_APB1ENR_TIM12EN,
			.freq = APB1_TIMER_FREQ,
			.tim = TIM12,
			.gpio_af = GPIO_AF_TIM12,
		},
		{
			.peripheral = &RCC->APB1ENR,
			.enable = RCC_APB1ENR_TIM13EN,
			.freq = APB1_TIMER_FREQ,
			.tim = TIM13,
			.gpio_af = GPIO_AF_TIM13,
		},
		{
			.peripheral = &RCC->APB1ENR,
			.enable = RCC_APB1ENR_TIM14EN,
			.freq = APB1_TIMER_FREQ,
			.tim = TIM14,
			.gpio_af = GPIO_AF_TIM14,
		},
	};

	static PwmHardware pwm_channels[] =
	{
		// DCDC PA9
		{
			// TIM1, CH2, PA9
			.gpio = 0,
			.gpio_pin = 9,
			.pwm_ctr = 1,
			.base = &pwm_timers[0],
		},
		// SHUTTER PE5
		{
			// TIM9, CH1 PE5
			.gpio = 4,
			.gpio_pin = 5,
			.pwm_ctr = 0,
			.base = &pwm_timers[8],
		},
		// MIRROR PH9
		{
			// TIM12, CH2 PH9
			.gpio = 7,
			.gpio_pin = 9,
			.pwm_ctr = 1,
			.base = &pwm_timers[11],
		},
#if STM3220GEVAL
		// HEATER PB9
		{
			// TIM4 CH4 on PD15
			.gpio = 3,
			.gpio_pin = 15,
			.pwm_ctr = 3,
			.base = &pwm_timers[3],
		},
#else
		// HEATER PB9
		{
			// TIM4 CH4 on PB9
			.gpio = 1,
			.gpio_pin = 9,
			.pwm_ctr = 3,
			.base = &pwm_timers[3],
		},
#endif
	};

	/*
	 * Init pwm.
	 */
	void pwm_hw_init(Pwm *ctx, unsigned ch)
	{
		ctx->hw = &pwm_channels[ch];

		/* enable timer peripheral */
		*ctx->hw->base->peripheral |= ctx->hw->base->enable;
		/* enable preload register */
		ctx->hw->base->tim->CR1 |= TIM_CR1_ARPE;
		/* main output enable (TIM1/8) */
		ctx->hw->base->tim->BDTR |= TIM_BDTR_MOE;

		/* edge-aligned PWM, enable preload register */
		ctx->hw->base->tim->CCMR[TIM_CCMR_IDX(ctx->hw->pwm_ctr)].VAL |= TIM_CCMR_VAL(ctx->hw->pwm_ctr, TIM_CCMR_CCS_OUT | TIM_CCMR_OCM_PWM1 | TIM_CCMR_OCPE);
		/* enable output starting with high polarity */
		ctx->hw->base->tim->CCER |= TIM_CCER_VAL(ctx->hw->pwm_ctr, TIM_CCER_CCEN);

		/* configure GPIO */
		RCC_GPIO_ENABLE(ctx->hw->gpio);

		stm32_gpioPinConfig(GPIOx(ctx->hw->gpio), BV(ctx->hw->gpio_pin),
				    GPIO_MODE_AF_PP | ctx->hw->base->gpio_af, GPIO_SPEED_50MHZ);
	}

#endif

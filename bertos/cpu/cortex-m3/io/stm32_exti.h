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
 * \brief STM32 EXTI definition.
 */

#ifndef STM32_EXTI_H
#define STM32_EXTI_H

#include <cfg/compiler.h>

#if CPU_CM3_STM32F1 || CPU_CM3_STM32F2

#include "stm32_rcc.h"
#include "stm32_syscfg.h"

struct stm32_exti
{
	reg32_t IMR;    /*!< EXTI Interrupt mask register,            Address offset: 0x00 */
	reg32_t EMR;    /*!< EXTI Event mask register,                Address offset: 0x04 */
	reg32_t RTSR;   /*!< EXTI Rising trigger selection register,  Address offset: 0x08 */
	reg32_t FTSR;   /*!< EXTI Falling trigger selection register, Address offset: 0x0C */
	reg32_t SWIER;  /*!< EXTI Software interrupt event register,  Address offset: 0x10 */
	reg32_t PR;     /*!< EXTI Pending register,                   Address offset: 0x14 */
};

#define EXTI ((struct stm32_exti *) EXTI_BASE)

/* bit offsets for the various EXTI registers */
#define EXTI_GPIO0_BIT         0
#define EXTI_GPIO1_BIT         1
#define EXTI_GPIO2_BIT         2
#define EXTI_GPIO3_BIT         3
#define EXTI_GPIO4_BIT         4
#define EXTI_GPIO5_BIT         5
#define EXTI_GPIO6_BIT         6
#define EXTI_GPIO7_BIT         7
#define EXTI_GPIO8_BIT         8
#define EXTI_GPIO9_BIT         9
#define EXTI_GPIO10_BIT       10
#define EXTI_GPIO11_BIT       11
#define EXTI_GPIO12_BIT       12
#define EXTI_GPIO13_BIT       13
#define EXTI_GPIO14_BIT       14
#define EXTI_GPIO15_BIT       15

#define EXTI_PVD_BIT          16
#define EXTI_RTC_ALARM_BIT    17
#define EXTI_USB_FS_WKUP_BIT  18
#define EXTI_ETH_WKUP_BIT     19
#define EXTI_USB_HS_WKUP_BIT  20
#define EXTI_RTC_TAMPER_BIT   21
#define EXTI_RTC_WKUP_BIT     22

/* GPIO number for SYSCFG register */
#define EXTI_GPIOA       0
#define EXTI_GPIOB       1
#define EXTI_GPIOC       2
#define EXTI_GPIOD       3
#define EXTI_GPIOE       4
#define EXTI_GPIOF       5
#define EXTI_GPIOG       6
#define EXTI_GPIOH       7
#define EXTI_GPIOI       8


#if CPU_CM3_STM32F1
	#warning __FILTER_NEXT_WARNING__
	#warning Not supported: Use AFIO instead of SYSCFG
#elif CPU_CM3_STM32F2
/* enable GPIOx pin as input source for EXTIx interrupt */
INLINE void stm32_extiGpioConfig(uint8_t gpio, uint8_t pin)
{
	/* enable SYSCFG clock */
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

	/* configure EXTI source */
	int offset = (pin & 0x3) * 4;
	int reg = pin >> 2;

	SYSCFG->EXTICR[reg] &= ~(0xf << offset);
	SYSCFG->EXTICR[reg] |= gpio << offset;
}
#else
#error Unknown CPU
#endif

#else
#error Unknown CPU
#endif

#endif /* STM32_EXTI_H */

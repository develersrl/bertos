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
 * Copyright 2010 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief STM32 Clocking driver.
 *
 * \author Andrea Righi <arighi@develer.com>
 */

#include "clock_stm32.h"

#include <cfg/compiler.h>
#include <cfg/debug.h>

#include <io/stm32.h>

#if CPU_CM3_STM32F1

struct RCC *RCC;

INLINE int rcc_get_flag_status(uint32_t flag)
{
	uint32_t id;
	reg32_t reg;

	/* Get the RCC register index */
	id = flag >> 5;
	/* The flag to check is in CR register */
	if (id == 1)
		reg = RCC->CR;
	/* The flag to check is in BDCR register */
	else if (id == 2)
		reg = RCC->BDCR;
	/* The flag to check is in CSR register */
	else
		reg = RCC->CSR;
	/* Get the flag position */
	id = flag & FLAG_MASK;

	return reg & (1 << id);
}

INLINE uint16_t pll_clock(void)
{
	unsigned int div, mul;

	/* Hopefully this is evaluate at compile time... */
	for (div = 2; div; div--)
		for (mul = 2; mul <= 16; mul++)
			if (CPU_FREQ <= (PLL_VCO / div * mul))
				break;
	return mul << 8 | div;
}

INLINE void rcc_pll_config(void)
{
	reg32_t reg = RCC->CFGR & CFGR_PLL_MASK;

	/* Evaluate clock parameters */
	uint16_t clock = pll_clock();
	uint32_t pll_mul = ((clock >> 8) - 2) << 18;
	uint32_t pll_div = ((clock & 0xff) << 1 | 1) << 16;

	/* Set the PLL configuration bits */
	reg |= pll_div | pll_mul;

	/* Store the new value */
	RCC->CFGR = reg;

	/* Enable PLL */
	*CR_PLLON_BB = 1;
}

INLINE void rcc_set_clock_source(uint32_t source)
{
	reg32_t reg;

	reg = RCC->CFGR & CFGR_SW_MASK;
	reg |= source;
	RCC->CFGR = reg;
}

void clock_init(void)
{
	/* Initialize global RCC structure */
	RCC = (struct RCC *)RCC_BASE;

	/* Enable the internal oscillator */
	*CR_HSION_BB = 1;
	while (!rcc_get_flag_status(RCC_FLAG_HSIRDY));

	/* Clock the system from internal HSI RC (8 MHz) */
	rcc_set_clock_source(RCC_SYSCLK_HSI);

	/* Enable external oscillator */
	RCC->CR &= CR_HSEON_RESET;
	RCC->CR &= CR_HSEBYP_RESET;
	RCC->CR |= CR_HSEON_SET;
	while (!rcc_get_flag_status(RCC_FLAG_HSERDY));

	/* Initialize PLL according to CPU_FREQ */
	rcc_pll_config();
	while(!rcc_get_flag_status(RCC_FLAG_PLLRDY));

	/* Configure USB clock (48MHz) */
	*CFGR_USBPRE_BB = RCC_USBCLK_PLLCLK_1DIV5;
	/* Configure ADC clock: PCLK2 (9MHz) */
	RCC->CFGR &= CFGR_ADCPRE_RESET_MASK;
	RCC->CFGR |= RCC_PCLK2_DIV8;
	/* Configure system clock dividers: PCLK2 (72MHz) */
	RCC->CFGR &= CFGR_PPRE2_RESET_MASK;
	RCC->CFGR |= RCC_HCLK_DIV1 << 3;
	/* Configure system clock dividers: PCLK1 (36MHz) */
	RCC->CFGR &= CFGR_PPRE1_RESET_MASK;
	RCC->CFGR |= RCC_HCLK_DIV2;
	/* Configure system clock dividers: HCLK */
	RCC->CFGR &= CFGR_HPRE_RESET_MASK;
	RCC->CFGR |= RCC_SYSCLK_DIV1;

	/* Set 1 wait state for the flash memory */
	*(reg32_t *)FLASH_BASE = 0x12;

	/* Clock the system from the PLL */
	rcc_set_clock_source(RCC_SYSCLK_PLLCLK);
}
#else /* CPU_CM3_STM32F2 */

/* PLL_VCO = (HSE_VALUE or HSI_VALUE / PLL_M) * PLL_N */
#define PLL_M      25
#define PLL_N      212

/* SYSCLK = PLL_VCO / PLL_P */
#define PLL_P      2

/* USB OTG FS, SDIO and RNG Clock =  PLL_VCO / PLLQ */
#define PLL_Q      5

/* PLLI2S_VCO = (HSE_VALUE Or HSI_VALUE / PLL_M) * PLLI2S_N
   I2SCLK = PLLI2S_VCO / PLLI2S_R */
#define PLLI2S_N   212
#define PLLI2S_R   5



#define HSE_STARTUP_TIMEOUT    ((uint16_t)0x0500)   /*!< Time out for HSE start up */

void clock_init(void)
{
	/* Reset the RCC clock configuration to the default reset state ------------*/
	/* Set HSION bit */
	RCC->CR |= (uint32_t)0x00000001;

	/* Reset CFGR register */
	RCC->CFGR = 0x00000000;

	/* Reset HSEON, CSSON and PLLON bits */
	RCC->CR &= (uint32_t)0xFEF6FFFF;

	/* Reset PLLCFGR register */
	RCC->PLLCFGR = 0x24003010;

	/* Reset HSEBYP bit */
	RCC->CR &= (uint32_t)0xFFFBFFFF;

	/* Disable all interrupts */
	RCC->CIR = 0x00000000;

	uint32_t StartUpCounter = 0, HSEStatus = 0;

	/* Enable HSE */
	RCC->CR |= ((uint32_t)RCC_CR_HSEON);

	/* Wait till HSE is ready and if Time out is reached exit */
	do
	{
		HSEStatus = RCC->CR & RCC_CR_HSERDY;
		StartUpCounter++;
	} while((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));

	if ((RCC->CR & RCC_CR_HSERDY) != 0)
	{
		HSEStatus = (uint32_t)0x01;
	}
	else
	{
		HSEStatus = (uint32_t)0x00;
	}

	if (HSEStatus == (uint32_t)0x01)
	{
		/* HCLK = SYSCLK / 1*/
		RCC->CFGR |= RCC_CFGR_HPRE_DIV1;

		/* PCLK2 = HCLK / 2*/
		RCC->CFGR |= RCC_CFGR_PPRE2_DIV2;

		/* PCLK1 = HCLK / 4*/
		RCC->CFGR |= RCC_CFGR_PPRE1_DIV4;

		/* Configure the main PLL */
		RCC->PLLCFGR = PLL_M | (PLL_N << 6) | (((PLL_P >> 1) -1) << 16) |
					   (RCC_PLLCFGR_PLLSRC_HSE) | (PLL_Q << 24);

		/* Enable the main PLL */
		RCC->CR |= RCC_CR_PLLON;

		/* Wait till the main PLL is ready */
		while((RCC->CR & RCC_CR_PLLRDY) == 0)
		{
		}

		/* Configure Flash prefetch, Instruction cache, Data cache and wait state */
		FLASH->ACR = FLASH_ACR_PRFTEN |FLASH_ACR_ICEN |FLASH_ACR_DCEN |FLASH_ACR_LATENCY_3WS;

		/* Select the main PLL as system clock source */
		RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
		RCC->CFGR |= RCC_CFGR_SW_PLL;

		/* Wait till the main PLL is used as system clock source */
		while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS ) != RCC_CFGR_SWS_PLL);
		{
		}
	}
	else
	{ /* If HSE fails to start-up, the application will have wrong clock
		 configuration. User can add here some code to deal with this error */
	}


	/******************************************************************************/
	/*            I2S clock configuration (For devices Rev B and Y)               */
	/******************************************************************************/
	/* PLLI2S clock used as I2S clock source */
	RCC->CFGR &= ~RCC_CFGR_I2SSRC;

	/* Configure PLLI2S */
	RCC->PLLI2SCFGR = (PLLI2S_N << 6) | (PLLI2S_R << 28);

	/* Enable PLLI2S */
	RCC->CR |= ((uint32_t)RCC_CR_PLLI2SON);

	/* Wait till PLLI2S is ready */
	while((RCC->CR & RCC_CR_PLLI2SRDY) == 0)
	{
	}
}

#endif

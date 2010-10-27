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
 * \brief Atmel SAM3 clock setup.
 *
 * \author Stefano Fedrigo <aleph@develer.com>
 */

#include "clock_sam3.h"
#include <cfg/compiler.h>
#include <cfg/macros.h>
#include <io/sam3.h>


/* Frequency of board main oscillator */
#define BOARDOSC_FREQ  12000000

/* Main crystal oscillator startup time, optimal value for CPU_FREQ == 48 MHz */
#define BOARD_OSC_COUNT  (CKGR_MOR_MOSCXTST(0x8))

/* Timer countdown timeout for clock initialization operations */
#define CLOCK_TIMEOUT    0xFFFFFFFF


/*
 * Try to evaluate the correct divider and multiplier value depending
 * on the desired CPU frequency.
 *
 * We try all combinations in a certain range of divider and multiplier
 * values.  The range can change, with better match with "strange"
 * frequencies, but boot time will be longer.
 *
 * Limits for SAM3N: divider [1,255], multiplier [1,2047].
 */
INLINE uint32_t evaluate_pll(void)
{
	int mul, div, best_mul, best_div;
	int best_delta = CPU_FREQ;
	int freq = 0;

	for (mul = 1; mul <= 8; mul++)
	{
		for (div = 1; div <= 24; div++)
		{
			freq = BOARDOSC_FREQ / div * (1 + mul);
			if (ABS((int)CPU_FREQ - freq) < best_delta) {
				best_delta = ABS((int)CPU_FREQ - freq);
				best_mul = mul;
				best_div = div;
			}
		}
	}

	// Bit 29 must always be set to 1
	return CKGR_PLLR_DIV(best_div) | CKGR_PLLR_MUL(best_mul) | BV(29);
}


void clock_init(void)
{
	uint32_t timeout;

	/* Disable watchdog */
	WDT_MR = BV(WDT_WDDIS);

	/* Set 4 wait states for flash access, needed for higher CPU clock rates */
	EEFC_FMR = EEFC_FMR_FWS(3);

	// Select external slow clock
	if (!(SUPC_SR & BV(SUPC_SR_OSCSEL)))
	{
		SUPC_CR = BV(SUPC_CR_XTALSEL) | SUPC_CR_KEY(0xA5);
		while (!(SUPC_SR & BV(SUPC_SR_OSCSEL)));
	}

	// Initialize main oscillator
	if (!(CKGR_MOR & BV(CKGR_MOR_MOSCSEL)))
	{
		CKGR_MOR = CKGR_MOR_KEY(0x37) | BOARD_OSC_COUNT | BV(CKGR_MOR_MOSCRCEN) | BV(CKGR_MOR_MOSCXTEN);
		timeout = CLOCK_TIMEOUT;
		while (!(PMC_SR & BV(PMC_SR_MOSCXTS)) && --timeout);
	}

	// Switch to external oscillator
	CKGR_MOR = CKGR_MOR_KEY(0x37) | BOARD_OSC_COUNT | BV(CKGR_MOR_MOSCRCEN) | BV(CKGR_MOR_MOSCXTEN) | BV(CKGR_MOR_MOSCSEL);
	timeout = CLOCK_TIMEOUT;
	while (!(PMC_SR & BV(PMC_SR_MOSCSELS)) && --timeout);

	PMC_MCKR = (PMC_MCKR & ~(uint32_t)PMC_MCKR_CSS_MASK) | PMC_MCKR_CSS_MAIN_CLK;
	timeout = CLOCK_TIMEOUT;
	while (!(PMC_SR & BV(PMC_SR_MCKRDY)) && --timeout);

	// Initialize and enable PLL clock
	CKGR_PLLR = evaluate_pll() | BV(CKGR_PLLR_STUCKTO1) | CKGR_PLLR_PLLCOUNT(0x1);
	timeout = CLOCK_TIMEOUT;
	while (!(PMC_SR & BV(PMC_SR_LOCK)) && --timeout);

	PMC_MCKR = PMC_MCKR_CSS_MAIN_CLK;
	timeout = CLOCK_TIMEOUT;
	while (!(PMC_SR & BV(PMC_SR_MCKRDY)) && --timeout);

	PMC_MCKR = PMC_MCKR_CSS_PLL_CLK;
	timeout = CLOCK_TIMEOUT;
	while (!(PMC_SR & BV(PMC_SR_MCKRDY)) && --timeout);

	/* Enable clock on PIO for inputs */
	PMC_PCER = BV(PIOA_ID) | BV(PIOB_ID) | BV(PIOC_ID);
}

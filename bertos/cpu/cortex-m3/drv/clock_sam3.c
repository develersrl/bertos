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
 * \brief AT91SAM3 clocking driver.
 *
 * \author Stefano Fedrigo <aleph@develer.com>
 */

#include "clock_sam3.h"
#include <io/sam3_pmc.h>
#include <cfg/compiler.h>
#include <cfg/macros.h>

/* Value to use when writing CKGR_MOR, to unlock write */
#define CKGR_KEY  0x37

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
			// RC oscillator set to 12 MHz
			freq = 12000000 / div * (1 + mul);
			if (ABS(CPU_FREQ - freq) < best_delta) {
				best_delta = ABS(CPU_FREQ - freq);
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
	/* Enable and configure internal Fast RC oscillator */
	CKGR_MOR_R =
		CKGR_MOR_KEY(CKGR_KEY)       // Unlock key
		| CKGR_MOR_MOSCRCEN          // Main On-Chip RC oscillator enable
		| CKGR_MOR_MOSCRCF_12MHZ;    // RC oscillator frequency

	/* Master clock: select PLL clock and no prescaling */
	PMC_MCKR_R = PMC_MCKR_CSS_PLL_CLK;

	CKGR_PLLR_R = evaluate_pll();
}

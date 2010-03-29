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
 * \brief LM3S1968 Clocking driver.
 *
 * \author Andrea Righi <arighi@develer.com>
 */

#include <cfg/compiler.h>
#include <cfg/debug.h>
#include "io/lm3s.h"
#include "clock.h"

/* See: LM3S1968 Microcontroller DATASHEET, p.80 */
static const unsigned long xtal_clk[] =
{
	1000000,
	1843200,
	2000000,
	2457600,
	3579545,
	3686400,
	4000000,
	4096000,
	4915200,
	5000000,
	5120000,
	6000000,
	6144000,
	7372800,
	8000000,
	8192000,
	10000000,
	12000000,
	12288000,
	13560000,
	14318180,
	16000000,
	16384000,
};

/* Extract the main oscillator frequency from the RCC register */
#define RCC_TO_CLK(rcc) \
		(xtal_clk[(((rcc) & SYSCTL_RCC_XTAL_MASK) >> \
				SYSCTL_RCC_XTAL_SHIFT)])

/* Extract the main oscillator frequency from the RCC register */
#define RCC_TO_SYSDIV(rcc) \
		(((rcc & SYSCTL_RCC_SYSDIV_MASK) >> \
			SYSCTL_RCC_SYSDIV_SHIFT) + 1)

/*
 * Very small delay: each loop takes 3 cycles.
 */
INLINE void __delay(unsigned long iterations)
{
	asm volatile (
		"1:	subs	%0, #1\n\t"
		"	bne 1b\n\t"
		: "=r"(iterations) : : "memory", "cc");
}

unsigned long clock_get_rate(void)
{
	unsigned long rcc, clk;

	rcc = HWREG(SYSCTL_RCC);

	/* Get the main oscillator frequency */
	clk = RCC_TO_CLK(rcc);
	/* Apply system clock divider */
	clk /= RCC_TO_SYSDIV(rcc);

	return clk;
}

void clock_set_rate(void)
{
	reg32_t rcc, rcc2;
	int i;

	rcc = HWREG(SYSCTL_RCC);
	rcc2 = HWREG(SYSCTL_RCC2);

	/*
	 * Step #1: bypass the PLL and system clock divider by setting the
	 * BYPASS bit and clearing the USESYS bit in the RCC register. This
	 * configures the system to run off a “raw” clock source (using the
	 * main oscillator or internal oscillator) and allows for the new PLL
	 * configuration to be validated before switching the system clock to
	 * the PLL.
	 */
	rcc |= SYSCTL_RCC_BYPASS;
	rcc &= ~SYSCTL_RCC_USESYSDIV;
	rcc2 |= SYSCTL_RCC2_BYPASS2;

	/* Write back RCC/RCC2 registers */
	HWREG(SYSCTL_RCC) = rcc;
	HWREG(SYSCTL_RCC) = rcc2;

	/*
	 * Step #2: select the crystal value (XTAL) and oscillator source
	 * (OSCSRC), and clear the PWRDN bit in RCC/RCC2. Setting the XTAL
	 * field automatically pulls valid PLL configuration data for the
	 * appropriate crystal, and clearing the PWRDN bit powers and enables
	 * the PLL and its output.
	 */

	/* Enable the main oscillator first. */
	rcc &= ~(SYSCTL_RCC_IOSCDIS | SYSCTL_RCC_MOSCDIS);
	rcc |= SYSCTL_RCC_IOSCDIS;

	/* Do not override RCC register fields */
	rcc2 &= ~SYSCTL_RCC2_USERCC2;

	rcc &= ~(SYSCTL_RCC_XTAL_M | SYSCTL_RCC_OSCSRC_M | SYSCTL_RCC_PWRDN);
	rcc |= XTAL_FREQ | SYSCTL_RCC_OSCSRC_MAIN;

	/* Clear the PLL lock interrupt. */
	HWREG(SYSCTL_MISC) = SYSCTL_INT_PLL_LOCK;

        HWREG(SYSCTL_RCC) = rcc;

	__delay(16);

	/*
	 * Step #3: select the desired system divider (SYSDIV) in RCC/RCC2 and
	 * set the USESYS bit in RCC. The SYSDIV field determines the system
	 * frequency for the microcontroller.
	 */
	rcc &= ~(SYSCTL_RCC_SYSDIV_M | SYSCTL_RCC_USESYSDIV);
	for (i = 0; i < 15; i++)
	{
		if (CPU_FREQ == RCC_TO_CLK(rcc))
			break;
		rcc |= SYSCTL_RCC_USESYSDIV;
	}
	if (i)
		rcc |= i << SYSCTL_RCC_SYSDIV_SHIFT;

	/*
	 * Step #4: wait for the PLL to lock by polling the PLLLRIS bit in the
	 * Raw Interrupt Status (RIS) register.
	 */
        for (i = 0; i < 32768; i++)
		if (HWREG(SYSCTL_RIS) & SYSCTL_INT_PLL_LOCK)
			break;

	/*
	 * Step #5: enable use of the PLL by clearing the BYPASS bit in
	 * RCC/RCC2.
	 */
        rcc &= ~SYSCTL_RCC_BYPASS;

	HWREG(SYSCTL_RCC) = rcc;

	__delay(16);
}

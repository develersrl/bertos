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
 * \brief SAM3 PMC hardware.
 */

#ifndef SAM3_PMC_H
#define SAM3_PMC_H

/** PMC registers base. */
#define PMC_BASE  0x400E0400

/**
 * PMC register offsets.
 */
/*\{*/
#define PMC_SCER_OFF  0x00   ///< System Clock Enable Register
#define PMC_SCDR_OFF  0x04   ///< System Clock Disable Register
#define PMC_SCSR_OFF  0x08   ///< System Clock Status Register
#define PMC_PCER_OFF  0x10   ///< Peripheral Clock Enable Register
#define PMC_PCDR_OFF  0x14   ///< Peripheral Clock Disable Register
#define PMC_PCSR_OFF  0x18   ///< Peripheral Clock Status Register
#define PMC_MOR_OFF   0x20   ///< Main Oscillator Register
#define PMC_MCFR_OFF  0x24   ///< Main Clock Frequency Register
#define PMC_PLLR_OFF  0x28   ///< PLL Register
#define PMC_MCKR_OFF  0x30   ///< Master Clock Register
#define PMC_PCK_OFF   0x40   ///< Programmable Clock 0 Register
#define PMC_IER_OFF   0x60   ///< Interrupt Enable Register
#define PMC_IDR_OFF   0x64   ///< Interrupt Disable Register
#define PMC_SR_OFF    0x68   ///< Status Register
#define PMC_IMR_OFF   0x6C   ///< Interrupt Mask Register
#define PMC_FSMR_OFF  0x70   ///< Fast Startup Mode Register
#define PMC_FSPR_OFF  0x74   ///< Fast Startup Polarity Register
#define PMC_FOCR_OFF  0x78   ///< Fault Output Clear Register
#define PMC_WPMR_OFF  0xE4   ///< Write Protect Mode Register
#define PMC_WPSR_OFF  0xE8   ///< Write Protect Status Register
#define PMC_OCR_OFF   0x110  ///< Oscillator Calibration Register
/*\}*/

/**
 * PMC registers.
 */
/*\{*/
#define PMC_SCER  (*((reg32_t *)(PMC_BASE + PMC_SCER_OFF)))   ///< System Clock Enable Register
#define PMC_SCDR  (*((reg32_t *)(PMC_BASE + PMC_SCDR_OFF)))   ///< System Clock Disable Register
#define PMC_SCSR  (*((reg32_t *)(PMC_BASE + PMC_SCSR_OFF)))   ///< System Clock Status Register
#define PMC_PCER  (*((reg32_t *)(PMC_BASE + PMC_PCER_OFF)))   ///< Peripheral Clock Enable Register
#define PMC_PCDR  (*((reg32_t *)(PMC_BASE + PMC_PCDR_OFF)))   ///< Peripheral Clock Disable Register
#define PMC_PCSR  (*((reg32_t *)(PMC_BASE + PMC_PCSR_OFF)))   ///< Peripheral Clock Status Register
#define CKGR_MOR  (*((reg32_t *)(PMC_BASE + PMC_MOR_OFF )))   ///< Main Oscillator Register
#define CKGR_MCFR (*((reg32_t *)(PMC_BASE + PMC_MCFR_OFF)))   ///< Main Clock Frequency Register
#define CKGR_PLLR (*((reg32_t *)(PMC_BASE + PMC_PLLR_OFF)))   ///< PLL Register
#define PMC_MCKR  (*((reg32_t *)(PMC_BASE + PMC_MCKR_OFF)))   ///< Master Clock Register
#define PMC_PCK   (*((reg32_t *)(PMC_BASE + PMC_PCK_OFF )))   ///< Programmable Clock 0 Register
#define PMC_IER   (*((reg32_t *)(PMC_BASE + PMC_IER_OFF )))   ///< Interrupt Enable Register
#define PMC_IDR   (*((reg32_t *)(PMC_BASE + PMC_IDR_OFF )))   ///< Interrupt Disable Register
#define PMC_SR    (*((reg32_t *)(PMC_BASE + PMC_SR_OFF  )))   ///< Status Register
#define PMC_IMR   (*((reg32_t *)(PMC_BASE + PMC_IMR_OFF )))   ///< Interrupt Mask Register
#define PMC_FSMR  (*((reg32_t *)(PMC_BASE + PMC_FSMR_OFF)))   ///< Fast Startup Mode Register
#define PMC_FSPR  (*((reg32_t *)(PMC_BASE + PMC_FSPR_OFF)))   ///< Fast Startup Polarity Register
#define PMC_FOCR  (*((reg32_t *)(PMC_BASE + PMC_FOCR_OFF)))   ///< Fault Output Clear Register
#define PMC_WPMR  (*((reg32_t *)(PMC_BASE + PMC_WPMR_OFF)))   ///< Write Protect Mode Register
#define PMC_WPSR  (*((reg32_t *)(PMC_BASE + PMC_WPSR_OFF)))   ///< Write Protect Status Register
#define PMC_OCR   (*((reg32_t *)(PMC_BASE + PMC_OCR_OFF )))   ///< Oscillator Calibration Register
/*\}*/

/**
 * Defines for bit fields in PMC_SCER register.
 */
/*\{*/
#define PMC_SCER_PCK0  8   ///< Programmable Clock 0 Output Enable
#define PMC_SCER_PCK1  9   ///< Programmable Clock 1 Output Enable
#define PMC_SCER_PCK2  10  ///< Programmable Clock 2 Output Enable
/*\}*/

/**
 * Defines for bit fields in PMC_SCDR register.
 */
/*\{*/
#define PMC_SCDR_PCK0  8   ///< Programmable Clock 0 Output Disable
#define PMC_SCDR_PCK1  9   ///< Programmable Clock 1 Output Disable
#define PMC_SCDR_PCK2  10  ///< Programmable Clock 2 Output Disable
/*\}*/

/**
 * Defines for bit fields in PMC_SCSR register.
 */
/*\{*/
#define PMC_SCSR_PCK0  8   ///< Programmable Clock 0 Output Status
#define PMC_SCSR_PCK1  9   ///< Programmable Clock 1 Output Status
#define PMC_SCSR_PCK2  10  ///< Programmable Clock 2 Output Status
/*\}*/

/**
 * Defines for bit fields in PMC_PCER register.
 */
/*\{*/
#define PMC_PCER_PID2    2    ///< Peripheral Clock 2 Enable
#define PMC_PCER_PID3    3    ///< Peripheral Clock 3 Enable
#define PMC_PCER_PID4    4    ///< Peripheral Clock 4 Enable
#define PMC_PCER_PID5    5    ///< Peripheral Clock 5 Enable
#define PMC_PCER_PID6    6    ///< Peripheral Clock 6 Enable
#define PMC_PCER_PID7    7    ///< Peripheral Clock 7 Enable
#define PMC_PCER_PID8    8    ///< Peripheral Clock 8 Enable
#define PMC_PCER_PID9    9    ///< Peripheral Clock 9 Enable
#define PMC_PCER_PID10  10   ///< Peripheral Clock 10 Enable
#define PMC_PCER_PID11  11   ///< Peripheral Clock 11 Enable
#define PMC_PCER_PID12  12   ///< Peripheral Clock 12 Enable
#define PMC_PCER_PID13  13   ///< Peripheral Clock 13 Enable
#define PMC_PCER_PID14  14   ///< Peripheral Clock 14 Enable
#define PMC_PCER_PID15  15   ///< Peripheral Clock 15 Enable
#define PMC_PCER_PID16  16   ///< Peripheral Clock 16 Enable
#define PMC_PCER_PID17  17   ///< Peripheral Clock 17 Enable
#define PMC_PCER_PID18  18   ///< Peripheral Clock 18 Enable
#define PMC_PCER_PID19  19   ///< Peripheral Clock 19 Enable
#define PMC_PCER_PID20  20   ///< Peripheral Clock 20 Enable
#define PMC_PCER_PID21  21   ///< Peripheral Clock 21 Enable
#define PMC_PCER_PID22  22   ///< Peripheral Clock 22 Enable
#define PMC_PCER_PID23  23   ///< Peripheral Clock 23 Enable
#define PMC_PCER_PID24  24   ///< Peripheral Clock 24 Enable
#define PMC_PCER_PID25  25   ///< Peripheral Clock 25 Enable
#define PMC_PCER_PID26  26   ///< Peripheral Clock 26 Enable
#define PMC_PCER_PID27  27   ///< Peripheral Clock 27 Enable
#define PMC_PCER_PID28  28   ///< Peripheral Clock 28 Enable
#define PMC_PCER_PID29  29   ///< Peripheral Clock 29 Enable
#define PMC_PCER_PID30  30   ///< Peripheral Clock 30 Enable
#define PMC_PCER_PID31  31   ///< Peripheral Clock 31 Enable
/*\}*/

/**
 * Defines for bit fields in PMC_PCDR register.
 */
/*\{*/
#define PMC_PCDR_PID2    2   ///< Peripheral Clock 2 Disable
#define PMC_PCDR_PID3    3   ///< Peripheral Clock 3 Disable
#define PMC_PCDR_PID4    4   ///< Peripheral Clock 4 Disable
#define PMC_PCDR_PID5    5   ///< Peripheral Clock 5 Disable
#define PMC_PCDR_PID6    6   ///< Peripheral Clock 6 Disable
#define PMC_PCDR_PID7    7   ///< Peripheral Clock 7 Disable
#define PMC_PCDR_PID8    8   ///< Peripheral Clock 8 Disable
#define PMC_PCDR_PID9    9   ///< Peripheral Clock 9 Disable
#define PMC_PCDR_PID10  10  ///< Peripheral Clock 10 Disable
#define PMC_PCDR_PID11  11  ///< Peripheral Clock 11 Disable
#define PMC_PCDR_PID12  12  ///< Peripheral Clock 12 Disable
#define PMC_PCDR_PID13  13  ///< Peripheral Clock 13 Disable
#define PMC_PCDR_PID14  14  ///< Peripheral Clock 14 Disable
#define PMC_PCDR_PID15  15  ///< Peripheral Clock 15 Disable
#define PMC_PCDR_PID16  16  ///< Peripheral Clock 16 Disable
#define PMC_PCDR_PID17  17  ///< Peripheral Clock 17 Disable
#define PMC_PCDR_PID18  18  ///< Peripheral Clock 18 Disable
#define PMC_PCDR_PID19  19  ///< Peripheral Clock 19 Disable
#define PMC_PCDR_PID20  20  ///< Peripheral Clock 20 Disable
#define PMC_PCDR_PID21  21  ///< Peripheral Clock 21 Disable
#define PMC_PCDR_PID22  22  ///< Peripheral Clock 22 Disable
#define PMC_PCDR_PID23  23  ///< Peripheral Clock 23 Disable
#define PMC_PCDR_PID24  24  ///< Peripheral Clock 24 Disable
#define PMC_PCDR_PID25  25  ///< Peripheral Clock 25 Disable
#define PMC_PCDR_PID26  26  ///< Peripheral Clock 26 Disable
#define PMC_PCDR_PID27  27  ///< Peripheral Clock 27 Disable
#define PMC_PCDR_PID28  28  ///< Peripheral Clock 28 Disable
#define PMC_PCDR_PID29  29  ///< Peripheral Clock 29 Disable
#define PMC_PCDR_PID30  30  ///< Peripheral Clock 30 Disable
#define PMC_PCDR_PID31  31  ///< Peripheral Clock 31 Disable
/*\}*/

/**
 * Defines for bit fields in PMC_PCSR register.
 */
/*\{*/
#define PMC_PCSR_PID2    2   ///< Peripheral Clock 2 Status
#define PMC_PCSR_PID3    3   ///< Peripheral Clock 3 Status
#define PMC_PCSR_PID4    4   ///< Peripheral Clock 4 Status
#define PMC_PCSR_PID5    5   ///< Peripheral Clock 5 Status
#define PMC_PCSR_PID6    6   ///< Peripheral Clock 6 Status
#define PMC_PCSR_PID7    7   ///< Peripheral Clock 7 Status
#define PMC_PCSR_PID8    8   ///< Peripheral Clock 8 Status
#define PMC_PCSR_PID9    9   ///< Peripheral Clock 9 Status
#define PMC_PCSR_PID10  10  ///< Peripheral Clock 10 Status
#define PMC_PCSR_PID11  11  ///< Peripheral Clock 11 Status
#define PMC_PCSR_PID12  12  ///< Peripheral Clock 12 Status
#define PMC_PCSR_PID13  13  ///< Peripheral Clock 13 Status
#define PMC_PCSR_PID14  14  ///< Peripheral Clock 14 Status
#define PMC_PCSR_PID15  15  ///< Peripheral Clock 15 Status
#define PMC_PCSR_PID16  16  ///< Peripheral Clock 16 Status
#define PMC_PCSR_PID17  17  ///< Peripheral Clock 17 Status
#define PMC_PCSR_PID18  18  ///< Peripheral Clock 18 Status
#define PMC_PCSR_PID19  19  ///< Peripheral Clock 19 Status
#define PMC_PCSR_PID20  20  ///< Peripheral Clock 20 Status
#define PMC_PCSR_PID21  21  ///< Peripheral Clock 21 Status
#define PMC_PCSR_PID22  22  ///< Peripheral Clock 22 Status
#define PMC_PCSR_PID23  23  ///< Peripheral Clock 23 Status
#define PMC_PCSR_PID24  24  ///< Peripheral Clock 24 Status
#define PMC_PCSR_PID25  25  ///< Peripheral Clock 25 Status
#define PMC_PCSR_PID26  26  ///< Peripheral Clock 26 Status
#define PMC_PCSR_PID27  27  ///< Peripheral Clock 27 Status
#define PMC_PCSR_PID28  28  ///< Peripheral Clock 28 Status
#define PMC_PCSR_PID29  29  ///< Peripheral Clock 29 Status
#define PMC_PCSR_PID30  30  ///< Peripheral Clock 30 Status
#define PMC_PCSR_PID31  31  ///< Peripheral Clock 31 Status
/*\}*/

/**
 * Defines for bit fields in CKGR_MOR register.
 */
/*\{*/
#define CKGR_MOR_MOSCXTEN         0   ///< Main Crystal Oscillator Enable
#define CKGR_MOR_MOSCXTBY         1   ///< Main Crystal Oscillator Bypass
#define CKGR_MOR_WAITMODE         2   ///< Wait Mode Command
#define CKGR_MOR_MOSCRCEN         3   ///< Main On-Chip RC Oscillator Enable
#define CKGR_MOR_MOSCRCF_SHIFT    4
#define CKGR_MOR_MOSCRCF_MASK     (0x7 << CKGR_MOR_MOSCRCF_SHIFT)   ///< Main On-Chip RC Oscillator Frequency Selection
#define CKGR_MOR_MOSCRCF(value)   ((CKGR_MOR_MOSCRCF_MASK & ((value) << CKGR_MOR_MOSCRCF_SHIFT)))
#define   CKGR_MOR_MOSCRCF_4MHZ   (0x0 << CKGR_MOR_MOSCRCF_SHIFT)
#define   CKGR_MOR_MOSCRCF_8MHZ   (0x1 << CKGR_MOR_MOSCRCF_SHIFT)
#define   CKGR_MOR_MOSCRCF_12MHZ  (0x2 << CKGR_MOR_MOSCRCF_SHIFT)
#define CKGR_MOR_MOSCXTST_SHIFT   8
#define CKGR_MOR_MOSCXTST_MASK    (0xff << CKGR_MOR_MOSCXTST_SHIFT)   ///< Main Crystal Oscillator Start-up Time
#define CKGR_MOR_MOSCXTST(value)  ((CKGR_MOR_MOSCXTST_MASK & ((value) << CKGR_MOR_MOSCXTST_SHIFT)))
#define CKGR_MOR_KEY_SHIFT        16
#define CKGR_MOR_KEY_MASK         (0xffu << CKGR_MOR_KEY_SHIFT)   ///< Password
#define CKGR_MOR_KEY(value)       ((CKGR_MOR_KEY_MASK & ((value) << CKGR_MOR_KEY_SHIFT)))
#define CKGR_MOR_MOSCSEL          24   ///< Main Oscillator Selection
#define CKGR_MOR_CFDEN            25   ///< Clock Failure Detector Enable
/*\}*/

/**
 * Defines for bit fields in CKGR_MCFR register.
 */
/*\{*/
#define CKGR_MCFR_MAINF_MASK  0xffff    ///< Main Clock Frequency mask
#define CKGR_MCFR_MAINFRDY    16        ///< Main Clock Ready
/*\}*/

/**
 * Defines for bit fields in CKGR_PLLR register.
 */
/*\{*/
#define CKGR_PLLR_DIV_MASK        0xff   ///< Divider mask
#define CKGR_PLLR_DIV(value)      (CKGR_PLLR_DIV_MASK & (value))
#define CKGR_PLLR_PLLCOUNT_SHIFT  8
#define CKGR_PLLR_PLLCOUNT_MASK   (0x3f << CKGR_PLLR_PLLCOUNT_SHIFT)   ///< PLL Counter mask
#define CKGR_PLLR_PLLCOUNT(value) (CKGR_PLLR_PLLCOUNT_MASK & ((value) << CKGR_PLLR_PLLCOUNT_SHIFT))
#define CKGR_PLLR_MUL_SHIFT       16
#define CKGR_PLLR_MUL_MASK        (0x7ff << CKGR_PLLR_MUL_SHIFT)   ///< PLL Multiplier mask
#define CKGR_PLLR_MUL(value)      (CKGR_PLLR_MUL_MASK & ((value) << CKGR_PLLR_MUL_SHIFT))
#define CKGR_PLLR_STUCKTO1        29
/*\}*/

/**
 * Defines for bit fields in PMC_MCKR register.
 */
/*\{*/
#define PMC_MCKR_CSS_MASK        0x3   ///< Master Clock Source Selection mask
#define   PMC_MCKR_CSS_SLOW_CLK  0x0   ///< Slow Clock is selected
#define   PMC_MCKR_CSS_MAIN_CLK  0x1   ///< Main Clock is selected
#define   PMC_MCKR_CSS_PLL_CLK   0x2   ///< PLL Clock is selected
#define PMC_MCKR_PRES_SHIFT      4
#define PMC_MCKR_PRES_MASK       (0x7 << PMC_MCKR_PRES_SHIFT)    ///< Processor Clock Prescaler mask
#define   PMC_MCKR_PRES_CLK      (0x0 << PMC_MCKR_PRES_SHIFT)   ///< Selected clock
#define   PMC_MCKR_PRES_CLK_2    (0x1 << PMC_MCKR_PRES_SHIFT)   ///< Selected clock divided by 2
#define   PMC_MCKR_PRES_CLK_4    (0x2 << PMC_MCKR_PRES_SHIFT)   ///< Selected clock divided by 4
#define   PMC_MCKR_PRES_CLK_8    (0x3 << PMC_MCKR_PRES_SHIFT)   ///< Selected clock divided by 8
#define   PMC_MCKR_PRES_CLK_16   (0x4 << PMC_MCKR_PRES_SHIFT)   ///< Selected clock divided by 16
#define   PMC_MCKR_PRES_CLK_32   (0x5 << PMC_MCKR_PRES_SHIFT)   ///< Selected clock divided by 32
#define   PMC_MCKR_PRES_CLK_64   (0x6 << PMC_MCKR_PRES_SHIFT)   ///< Selected clock divided by 64
#define   PMC_MCKR_PRES_CLK_3    (0x7 << PMC_MCKR_PRES_SHIFT)   ///< Selected clock divided by 3
#define PMC_MCKR_PLLDIV2         12   ///< PLL Divisor by 2
/*\}*/

/**
 * Defines for bit fields in PMC_PCK[3] register.
 */
/*\{*/
#define PMC_PCK_CSS_MASK       0x7   ///< Master Clock Source Selection mask
#define   PMC_PCK_CSS_SLOW     0x0   ///< Slow Clock is selected
#define   PMC_PCK_CSS_MAIN     0x1   ///< Main Clock is selected
#define   PMC_PCK_CSS_PLL      0x2   ///< PLL Clock is selected
#define   PMC_PCK_CSS_MCK      0x4   ///< Master Clock is selected
#define PMC_PCK_PRES_SHIFT 4
#define PMC_PCK_PRES_MASK      (0x7 << PMC_PCK_PRES_SHIFT)   ///< Programmable Clock Prescaler
#define   PMC_PCK_PRES_CLK     (0x0 << PMC_PCK_PRES_SHIFT)   ///< Selected clock
#define   PMC_PCK_PRES_CLK_2   (0x1 << PMC_PCK_PRES_SHIFT)   ///< Selected clock divided by 2
#define   PMC_PCK_PRES_CLK_4   (0x2 << PMC_PCK_PRES_SHIFT)   ///< Selected clock divided by 4
#define   PMC_PCK_PRES_CLK_8   (0x3 << PMC_PCK_PRES_SHIFT)   ///< Selected clock divided by 8
#define   PMC_PCK_PRES_CLK_16  (0x4 << PMC_PCK_PRES_SHIFT)   ///< Selected clock divided by 16
#define   PMC_PCK_PRES_CLK_32  (0x5 << PMC_PCK_PRES_SHIFT)   ///< Selected clock divided by 32
#define   PMC_PCK_PRES_CLK_64  (0x6 << PMC_PCK_PRES_SHIFT)   ///< Selected clock divided by 64
/*\}*/

/**
 * Defines for bit fields in PMC_IER register.
 */
/*\{*/
#define PMC_IER_MOSCXTS   0   ///< Main Crystal Oscillator Status Interrupt Enable
#define PMC_IER_LOCK      1   ///< PLL Lock Interrupt Enable
#define PMC_IER_MCKRDY    3   ///< Master Clock Ready Interrupt Enable
#define PMC_IER_PCKRDY0   8   ///< Programmable Clock Ready 0 Interrupt Enable
#define PMC_IER_PCKRDY1   9   ///< Programmable Clock Ready 1 Interrupt Enable
#define PMC_IER_PCKRDY2   10  ///< Programmable Clock Ready 2 Interrupt Enable
#define PMC_IER_MOSCSELS  16  ///< Main Oscillator Selection Status Interrupt Enable
#define PMC_IER_MOSCRCS   17  ///< Main On-Chip RC Status Interrupt Enable
#define PMC_IER_CFDEV     18  ///< Clock Failure Detector Event Interrupt Enable
/*\}*/

/**
 * Defines for bit fields in PMC_IDR register.
 */
/*\{*/
#define PMC_IDR_MOSCXTS   0   ///< Main Crystal Oscillator Status Interrupt Disable
#define PMC_IDR_LOCK      1   ///< PLL Lock Interrupt Disable
#define PMC_IDR_MCKRDY    3   ///< Master Clock Ready Interrupt Disable
#define PMC_IDR_PCKRDY0   8   ///< Programmable Clock Ready 0 Interrupt Disable
#define PMC_IDR_PCKRDY1   9   ///< Programmable Clock Ready 1 Interrupt Disable
#define PMC_IDR_PCKRDY2   10  ///< Programmable Clock Ready 2 Interrupt Disable
#define PMC_IDR_MOSCSELS  16  ///< Main Oscillator Selection Status Interrupt Disable
#define PMC_IDR_MOSCRCS   17  ///< Main On-Chip RC Status Interrupt Disable
#define PMC_IDR_CFDEV     18  ///< Clock Failure Detector Event Interrupt Disable
/*\}*/

/**
 * Defines for bit fields in PMC_SR register.
 */
/*\{*/
#define PMC_SR_MOSCXTS   0   ///< Main XTAL Oscillator Status
#define PMC_SR_LOCK      1   ///< PLL Lock Status
#define PMC_SR_MCKRDY    3   ///< Master Clock Status
#define PMC_SR_OSCSELS   7   ///< Slow Clock Oscillator Selection
#define PMC_SR_PCKRDY0   8   ///< Programmable Clock Ready Status
#define PMC_SR_PCKRDY1   9   ///< Programmable Clock Ready Status
#define PMC_SR_PCKRDY2   10  ///< Programmable Clock Ready Status
#define PMC_SR_MOSCSELS  16  ///< Main Oscillator Selection Status
#define PMC_SR_MOSCRCS   17  ///< Main On-Chip RC Oscillator Status
#define PMC_SR_CFDEV     18  ///< Clock Failure Detector Event
#define PMC_SR_CFDS      19  ///< Clock Failure Detector Status
#define PMC_SR_FOS       20  ///< Clock Failure Detector Fault Output Status
/*\}*/

/**
 * Defines for bit fields in PMC_IMR register.
 */
/*\{*/
#define PMC_IMR_MOSCXTS   0   ///< Main Crystal Oscillator Status Interrupt Mask
#define PMC_IMR_LOCK      1   ///< PLL Lock Interrupt Mask
#define PMC_IMR_MCKRDY    3   ///< Master Clock Ready Interrupt Mask
#define PMC_IMR_PCKRDY0   8   ///< Programmable Clock Ready 0 Interrupt Mask
#define PMC_IMR_PCKRDY1   9   ///< Programmable Clock Ready 1 Interrupt Mask
#define PMC_IMR_PCKRDY2   10  ///< Programmable Clock Ready 2 Interrupt Mask
#define PMC_IMR_MOSCSELS  16  ///< Main Oscillator Selection Status Interrupt Mask
#define PMC_IMR_MOSCRCS   17  ///< Main On-Chip RC Status Interrupt Mask
#define PMC_IMR_CFDEV     18  ///< Clock Failure Detector Event Interrupt Mask
/*\}*/

/**
 * Defines for bit fields in PMC_FSMR register.
 */
/*\{*/
#define PMC_FSMR_FSTT0   0   ///< Fast Startup Input Enable 0
#define PMC_FSMR_FSTT1   1   ///< Fast Startup Input Enable 1
#define PMC_FSMR_FSTT2   2   ///< Fast Startup Input Enable 2
#define PMC_FSMR_FSTT3   3   ///< Fast Startup Input Enable 3
#define PMC_FSMR_FSTT4   4   ///< Fast Startup Input Enable 4
#define PMC_FSMR_FSTT5   5   ///< Fast Startup Input Enable 5
#define PMC_FSMR_FSTT6   6   ///< Fast Startup Input Enable 6
#define PMC_FSMR_FSTT7   7   ///< Fast Startup Input Enable 7
#define PMC_FSMR_FSTT8   8   ///< Fast Startup Input Enable 8
#define PMC_FSMR_FSTT9   9   ///< Fast Startup Input Enable 9
#define PMC_FSMR_FSTT10  10  ///< Fast Startup Input Enable 10
#define PMC_FSMR_FSTT11  11  ///< Fast Startup Input Enable 11
#define PMC_FSMR_FSTT12  12  ///< Fast Startup Input Enable 12
#define PMC_FSMR_FSTT13  13  ///< Fast Startup Input Enable 13
#define PMC_FSMR_FSTT14  14  ///< Fast Startup Input Enable 14
#define PMC_FSMR_FSTT15  15  ///< Fast Startup Input Enable 15
#define PMC_FSMR_RTTAL   16  ///< RTT Alarm Enable
#define PMC_FSMR_RTCAL   17  ///< RTC Alarm Enable
#define PMC_FSMR_LPM     20  ///< Low Power Mode
/*\}*/

/**
 * Defines for bit fields in PMC_FSPR register.
 */
/*\{*/
#define PMC_FSPR_FSTP0   0   ///< Fast Startup Input Polarityx
#define PMC_FSPR_FSTP1   1   ///< Fast Startup Input Polarityx
#define PMC_FSPR_FSTP2   2   ///< Fast Startup Input Polarityx
#define PMC_FSPR_FSTP3   3   ///< Fast Startup Input Polarityx
#define PMC_FSPR_FSTP4   4   ///< Fast Startup Input Polarityx
#define PMC_FSPR_FSTP5   5   ///< Fast Startup Input Polarityx
#define PMC_FSPR_FSTP6   6   ///< Fast Startup Input Polarityx
#define PMC_FSPR_FSTP7   7   ///< Fast Startup Input Polarityx
#define PMC_FSPR_FSTP8   8   ///< Fast Startup Input Polarityx
#define PMC_FSPR_FSTP9   9   ///< Fast Startup Input Polarityx
#define PMC_FSPR_FSTP10  10  ///< Fast Startup Input Polarityx
#define PMC_FSPR_FSTP11  11  ///< Fast Startup Input Polarityx
#define PMC_FSPR_FSTP12  12  ///< Fast Startup Input Polarityx
#define PMC_FSPR_FSTP13  13  ///< Fast Startup Input Polarityx
#define PMC_FSPR_FSTP14  14  ///< Fast Startup Input Polarityx
#define PMC_FSPR_FSTP15  15  ///< Fast Startup Input Polarityx
/*\}*/

/**
 * Defines for bit fields in PMC_FOCR register.
 */
/*\{*/
#define PMC_FOCR_FOCLR  0   ///< Fault Output Clear
/*\}*/

/**
 * Defines for bit fields in PMC_WPMR register.
 */
/*\{*/
#define PMC_WPMR_WPEN          0   ///< Write Protect Enable
#define PMC_WPMR_WPKEY_SHIFT   8
#define PMC_WPMR_WPKEY_MASK    (0xffffff << PMC_WPMR_WPKEY_SHIFT)   ///< Write Protect key mask
#define PMC_WPMR_WPKEY(value)  ((PMC_WPMR_WPKEY_MASK & ((value) << PMC_WPMR_WPKEY_SHIFT)))
/*\}*/

/**
 * Defines for bit fields in PMC_WPSR register.
 */
/*\{*/
#define PMC_WPSR_WPVS          0   ///< Write Protect Violation Status
#define PMC_WPSR_WPVSRC_SHIFT  8
#define PMC_WPSR_WPVSRC_MASK   (0xffff << PMC_WPSR_WPVSRC_SHIFT)   ///< Write Protect Violation Source mask
/*\}*/

/**
 * Defines for bit fields in PMC_OCR register.
 */
/*\{*/
#define PMC_OCR_CAL4_MASK     0x7f  ///< RC Oscillator Calibration bits for 4 MHz mask
#define PMC_OCR_CAL4(value)   (PMC_OCR_CAL4_MASK & (value))
#define PMC_OCR_SEL4          7   ///< Selection of RC Oscillator Calibration bits for 4 MHz
#define PMC_OCR_CAL8_SHIFT    8
#define PMC_OCR_CAL8_MASK     (0x7f << PMC_OCR_CAL8_SHIFT)   ///< RC Oscillator Calibration bits for 8 MHz mask
#define PMC_OCR_CAL8(value)   ((PMC_OCR_CAL8_MASK & ((value) << PMC_OCR_CAL8_SHIFT)))
#define PMC_OCR_SEL8          15  ///< Selection of RC Oscillator Calibration bits for 8 MHz
#define PMC_OCR_CAL12_SHIFT   16
#define PMC_OCR_CAL12_MASK    (0x7f << PMC_OCR_CAL12_SHIFT)   ///< RC Oscillator Calibration bits for 12 MHz mask
#define PMC_OCR_CAL12(value)  ((PMC_OCR_CAL12_MASK & ((value) << PMC_OCR_CAL12_SHIFT)))
#define PMC_OCR_SEL12         23   ///< Selection of RC Oscillator Calibration bits for 12 MHz
/*\}*/


#endif /* SAM3_PMC_H */

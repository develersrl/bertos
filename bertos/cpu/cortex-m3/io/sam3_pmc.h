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
 * \brief AT91SAM3 PMC hardware.
 */

#ifndef SAM3_PMC_H
#define SAM3_PMC_H

// FIXME: move these in sam3(?)_int.h
#define INT_SUPC     0  ///< SAM3N Supply Controller (SUPC)
#define INT_RSTC     1  ///< SAM3N Reset Controller (RSTC)
#define INT_RTC      2  ///< SAM3N Real Time Clock (RTC)
#define INT_RTT      3  ///< SAM3N Real Time Timer (RTT)
#define INT_WDT      4  ///< SAM3N Watchdog Timer (WDT)
#define INT_PMC      5  ///< SAM3N Power Management Controller (PMC)
#define INT_EFC      6  ///< SAM3N Enhanced Flash Controller (EFC)
#define INT_UART0    8  ///< SAM3N UART 0 (UART0)
#define INT_UART1    9  ///< SAM3N UART 1 (UART1)
#define INT_PIOA    11  ///< SAM3N Parallel I/O Controller A (PIOA)
#define INT_PIOB    12  ///< SAM3N Parallel I/O Controller B (PIOB)
#define INT_PIOC    13  ///< SAM3N Parallel I/O Controller C (PIOC)
#define INT_USART0  14  ///< SAM3N USART 0 (USART0)
#define INT_USART1  15  ///< SAM3N USART 1 (USART1)
#define INT_TWI0    19  ///< SAM3N Two Wire Interface 0 (TWI0)
#define INT_TWI1    20  ///< SAM3N Two Wire Interface 1 (TWI1)
#define INT_SPI     21  ///< SAM3N Serial Peripheral Interface (SPI)
#define INT_TC0     23  ///< SAM3N Timer/Counter 0 (TC0)
#define INT_TC1     24  ///< SAM3N Timer/Counter 1 (TC1)
#define INT_TC2     25  ///< SAM3N Timer/Counter 2 (TC2)
#define INT_TC3     26  ///< SAM3N Timer/Counter 3 (TC3)
#define INT_TC4     27  ///< SAM3N Timer/Counter 4 (TC4)
#define INT_TC5     28  ///< SAM3N Timer/Counter 5 (TC5)
#define INT_ADC     29  ///< SAM3N Analog To Digital Converter (ADC)
#define INT_DACC    30  ///< SAM3N Digital To Analog Converter (DACC)
#define INT_PWM     31  ///< SAM3N Pulse Width Modulation (PWM)

/**
 * PMC registers.
 */
/*\{*/
#define PMC_SCER_R  (*((reg32_t *)0x400E0400))   ///< System Clock Enable Register
#define PMC_SCDR_R  (*((reg32_t *)0x400E0404))   ///< System Clock Disable Register
#define PMC_SCSR_R  (*((reg32_t *)0x400E0408))   ///< System Clock Status Register
#define PMC_PCER_R  (*((reg32_t *)0x400E0410))   ///< Peripheral Clock Enable Register
#define PMC_PCDR_R  (*((reg32_t *)0x400E0414))   ///< Peripheral Clock Disable Register
#define PMC_PCSR_R  (*((reg32_t *)0x400E0418))   ///< Peripheral Clock Status Register
#define PMC_MOR_R   (*((reg32_t *)0x400E0420))   ///< Main Oscillator Register
#define PMC_MCFR_R  (*((reg32_t *)0x400E0424))   ///< Main Clock Frequency Register
#define PMC_PLLR_R  (*((reg32_t *)0x400E0428))   ///< PLL Register
#define PMC_MCKR_R  (*((reg32_t *)0x400E0430))   ///< Master Clock Register
#define PMC_PCK_R   (*((reg32_t *)0x400E0440))   ///< Programmable Clock 0 Register
#define PMC_IER_R   (*((reg32_t *)0x400E0460))   ///< Interrupt Enable Register
#define PMC_IDR_R   (*((reg32_t *)0x400E0464))   ///< Interrupt Disable Register
#define PMC_SR_R    (*((reg32_t *)0x400E0468))   ///< Status Register
#define PMC_IMR_R   (*((reg32_t *)0x400E046C))   ///< Interrupt Mask Register
#define PMC_FSMR_R  (*((reg32_t *)0x400E0470))   ///< Fast Startup Mode Register
#define PMC_FSPR_R  (*((reg32_t *)0x400E0474))   ///< Fast Startup Polarity Register
#define PMC_FOCR_R  (*((reg32_t *)0x400E0478))   ///< Fault Output Clear Register
#define PMC_WPMR_R  (*((reg32_t *)0x400E04E4))   ///< Write Protect Mode Register
#define PMC_WPSR_R  (*((reg32_t *)0x400E04E8))   ///< Write Protect Status Register
#define PMC_OCR_R   (*((reg32_t *)0x400E0510))   ///< Oscillator Calibration Register
/*\}*/

/**
 * PMC register addresses.
 */
/*\{*/
#define PMC_SCER  0x400E0400   ///< System Clock Enable Register
#define PMC_SCDR  0x400E0404   ///< System Clock Disable Register
#define PMC_SCSR  0x400E0408   ///< System Clock Status Register
#define PMC_PCER  0x400E0410   ///< Peripheral Clock Enable Register
#define PMC_PCDR  0x400E0414   ///< Peripheral Clock Disable Register
#define PMC_PCSR  0x400E0418   ///< Peripheral Clock Status Register
#define PMC_MOR   0x400E0420   ///< Main Oscillator Register
#define PMC_MCFR  0x400E0424   ///< Main Clock Frequency Register
#define PMC_PLLR  0x400E0428   ///< PLL Register
#define PMC_MCKR  0x400E0430   ///< Master Clock Register
#define PMC_PCK   0x400E0440   ///< Programmable Clock 0 Register
#define PMC_IER   0x400E0460   ///< Interrupt Enable Register
#define PMC_IDR   0x400E0464   ///< Interrupt Disable Register
#define PMC_SR    0x400E0468   ///< Status Register
#define PMC_IMR   0x400E046C   ///< Interrupt Mask Register
#define PMC_FSMR  0x400E0470   ///< Fast Startup Mode Register
#define PMC_FSPR  0x400E0474   ///< Fast Startup Polarity Register
#define PMC_FOCR  0x400E0478   ///< Fault Output Clear Register
#define PMC_WPMR  0x400E04E4   ///< Write Protect Mode Register
#define PMC_WPSR  0x400E04E8   ///< Write Protect Status Register
#define PMC_OCR   0x400E0510   ///< Oscillator Calibration Register
/*\}*/

/**
 * Defines for bit fields in PMC_SCER register.
 */
/*\{*/
#define PMC_SCER_PCK0  BV(8)   ///< Programmable Clock 0 Output Enable
#define PMC_SCER_PCK1  BV(9)   ///< Programmable Clock 1 Output Enable
#define PMC_SCER_PCK2  BV(10)  ///< Programmable Clock 2 Output Enable
/*\}*/

/**
 * Defines for bit fields in PMC_SCDR register.
 */
/*\{*/
#define PMC_SCDR_PCK0  BV(8)   ///< Programmable Clock 0 Output Disable
#define PMC_SCDR_PCK1  BV(9)   ///< Programmable Clock 1 Output Disable
#define PMC_SCDR_PCK2  BV(10)  ///< Programmable Clock 2 Output Disable
/*\}*/

/**
 * Defines for bit fields in PMC_SCSR register.
 */
/*\{*/
#define PMC_SCSR_PCK0  BV(8)   ///< Programmable Clock 0 Output Status
#define PMC_SCSR_PCK1  BV(9)   ///< Programmable Clock 1 Output Status
#define PMC_SCSR_PCK2  BV(10)  ///< Programmable Clock 2 Output Status
/*\}*/

/**
 * Defines for bit fields in PMC_PCER register.
 */
/*\{*/
#define PMC_PCER_PID2   BV(2)    ///< Peripheral Clock 2 Enable
#define PMC_PCER_PID3   BV(3)    ///< Peripheral Clock 3 Enable
#define PMC_PCER_PID4   BV(4)    ///< Peripheral Clock 4 Enable
#define PMC_PCER_PID5   BV(5)    ///< Peripheral Clock 5 Enable
#define PMC_PCER_PID6   BV(6)    ///< Peripheral Clock 6 Enable
#define PMC_PCER_PID7   BV(7)    ///< Peripheral Clock 7 Enable
#define PMC_PCER_PID8   BV(8)    ///< Peripheral Clock 8 Enable
#define PMC_PCER_PID9   BV(9)    ///< Peripheral Clock 9 Enable
#define PMC_PCER_PID10  BV(10)   ///< Peripheral Clock 10 Enable
#define PMC_PCER_PID11  BV(11)   ///< Peripheral Clock 11 Enable
#define PMC_PCER_PID12  BV(12)   ///< Peripheral Clock 12 Enable
#define PMC_PCER_PID13  BV(13)   ///< Peripheral Clock 13 Enable
#define PMC_PCER_PID14  BV(14)   ///< Peripheral Clock 14 Enable
#define PMC_PCER_PID15  BV(15)   ///< Peripheral Clock 15 Enable
#define PMC_PCER_PID16  BV(16)   ///< Peripheral Clock 16 Enable
#define PMC_PCER_PID17  BV(17)   ///< Peripheral Clock 17 Enable
#define PMC_PCER_PID18  BV(18)   ///< Peripheral Clock 18 Enable
#define PMC_PCER_PID19  BV(19)   ///< Peripheral Clock 19 Enable
#define PMC_PCER_PID20  BV(20)   ///< Peripheral Clock 20 Enable
#define PMC_PCER_PID21  BV(21)   ///< Peripheral Clock 21 Enable
#define PMC_PCER_PID22  BV(22)   ///< Peripheral Clock 22 Enable
#define PMC_PCER_PID23  BV(23)   ///< Peripheral Clock 23 Enable
#define PMC_PCER_PID24  BV(24)   ///< Peripheral Clock 24 Enable
#define PMC_PCER_PID25  BV(25)   ///< Peripheral Clock 25 Enable
#define PMC_PCER_PID26  BV(26)   ///< Peripheral Clock 26 Enable
#define PMC_PCER_PID27  BV(27)   ///< Peripheral Clock 27 Enable
#define PMC_PCER_PID28  BV(28)   ///< Peripheral Clock 28 Enable
#define PMC_PCER_PID29  BV(29)   ///< Peripheral Clock 29 Enable
#define PMC_PCER_PID30  BV(30)   ///< Peripheral Clock 30 Enable
#define PMC_PCER_PID31  BV(31)   ///< Peripheral Clock 31 Enable
/*\}*/

/**
 * Defines for bit fields in PMC_PCDR register.
 */
/*\{*/
#define PMC_PCDR_PID2   BV(2)   ///< Peripheral Clock 2 Disable
#define PMC_PCDR_PID3   BV(3)   ///< Peripheral Clock 3 Disable
#define PMC_PCDR_PID4   BV(4)   ///< Peripheral Clock 4 Disable
#define PMC_PCDR_PID5   BV(5)   ///< Peripheral Clock 5 Disable
#define PMC_PCDR_PID6   BV(6)   ///< Peripheral Clock 6 Disable
#define PMC_PCDR_PID7   BV(7)   ///< Peripheral Clock 7 Disable
#define PMC_PCDR_PID8   BV(8)   ///< Peripheral Clock 8 Disable
#define PMC_PCDR_PID9   BV(9)   ///< Peripheral Clock 9 Disable
#define PMC_PCDR_PID10  BV(10)  ///< Peripheral Clock 10 Disable
#define PMC_PCDR_PID11  BV(11)  ///< Peripheral Clock 11 Disable
#define PMC_PCDR_PID12  BV(12)  ///< Peripheral Clock 12 Disable
#define PMC_PCDR_PID13  BV(13)  ///< Peripheral Clock 13 Disable
#define PMC_PCDR_PID14  BV(14)  ///< Peripheral Clock 14 Disable
#define PMC_PCDR_PID15  BV(15)  ///< Peripheral Clock 15 Disable
#define PMC_PCDR_PID16  BV(16)  ///< Peripheral Clock 16 Disable
#define PMC_PCDR_PID17  BV(17)  ///< Peripheral Clock 17 Disable
#define PMC_PCDR_PID18  BV(18)  ///< Peripheral Clock 18 Disable
#define PMC_PCDR_PID19  BV(19)  ///< Peripheral Clock 19 Disable
#define PMC_PCDR_PID20  BV(20)  ///< Peripheral Clock 20 Disable
#define PMC_PCDR_PID21  BV(21)  ///< Peripheral Clock 21 Disable
#define PMC_PCDR_PID22  BV(22)  ///< Peripheral Clock 22 Disable
#define PMC_PCDR_PID23  BV(23)  ///< Peripheral Clock 23 Disable
#define PMC_PCDR_PID24  BV(24)  ///< Peripheral Clock 24 Disable
#define PMC_PCDR_PID25  BV(25)  ///< Peripheral Clock 25 Disable
#define PMC_PCDR_PID26  BV(26)  ///< Peripheral Clock 26 Disable
#define PMC_PCDR_PID27  BV(27)  ///< Peripheral Clock 27 Disable
#define PMC_PCDR_PID28  BV(28)  ///< Peripheral Clock 28 Disable
#define PMC_PCDR_PID29  BV(29)  ///< Peripheral Clock 29 Disable
#define PMC_PCDR_PID30  BV(30)  ///< Peripheral Clock 30 Disable
#define PMC_PCDR_PID31  BV(31)  ///< Peripheral Clock 31 Disable
/*\}*/

/**
 * Defines for bit fields in PMC_PCSR register.
 */
/*\{*/
#define PMC_PCSR_PID2   BV(2)   ///< Peripheral Clock 2 Status
#define PMC_PCSR_PID3   BV(3)   ///< Peripheral Clock 3 Status
#define PMC_PCSR_PID4   BV(4)   ///< Peripheral Clock 4 Status
#define PMC_PCSR_PID5   BV(5)   ///< Peripheral Clock 5 Status
#define PMC_PCSR_PID6   BV(6)   ///< Peripheral Clock 6 Status
#define PMC_PCSR_PID7   BV(7)   ///< Peripheral Clock 7 Status
#define PMC_PCSR_PID8   BV(8)   ///< Peripheral Clock 8 Status
#define PMC_PCSR_PID9   BV(9)   ///< Peripheral Clock 9 Status
#define PMC_PCSR_PID10  BV(10)  ///< Peripheral Clock 10 Status
#define PMC_PCSR_PID11  BV(11)  ///< Peripheral Clock 11 Status
#define PMC_PCSR_PID12  BV(12)  ///< Peripheral Clock 12 Status
#define PMC_PCSR_PID13  BV(13)  ///< Peripheral Clock 13 Status
#define PMC_PCSR_PID14  BV(14)  ///< Peripheral Clock 14 Status
#define PMC_PCSR_PID15  BV(15)  ///< Peripheral Clock 15 Status
#define PMC_PCSR_PID16  BV(16)  ///< Peripheral Clock 16 Status
#define PMC_PCSR_PID17  BV(17)  ///< Peripheral Clock 17 Status
#define PMC_PCSR_PID18  BV(18)  ///< Peripheral Clock 18 Status
#define PMC_PCSR_PID19  BV(19)  ///< Peripheral Clock 19 Status
#define PMC_PCSR_PID20  BV(20)  ///< Peripheral Clock 20 Status
#define PMC_PCSR_PID21  BV(21)  ///< Peripheral Clock 21 Status
#define PMC_PCSR_PID22  BV(22)  ///< Peripheral Clock 22 Status
#define PMC_PCSR_PID23  BV(23)  ///< Peripheral Clock 23 Status
#define PMC_PCSR_PID24  BV(24)  ///< Peripheral Clock 24 Status
#define PMC_PCSR_PID25  BV(25)  ///< Peripheral Clock 25 Status
#define PMC_PCSR_PID26  BV(26)  ///< Peripheral Clock 26 Status
#define PMC_PCSR_PID27  BV(27)  ///< Peripheral Clock 27 Status
#define PMC_PCSR_PID28  BV(28)  ///< Peripheral Clock 28 Status
#define PMC_PCSR_PID29  BV(29)  ///< Peripheral Clock 29 Status
#define PMC_PCSR_PID30  BV(30)  ///< Peripheral Clock 30 Status
#define PMC_PCSR_PID31  BV(31)  ///< Peripheral Clock 31 Status
/*\}*/

/**
 * Defines for bit fields in CKGR_MOR register.
 */
/*\{*/
#define CKGR_MOR_MOSCXTEN         BV(0)   ///< Main Crystal Oscillator Enable
#define CKGR_MOR_MOSCXTBY         BV(1)   ///< Main Crystal Oscillator Bypass
#define CKGR_MOR_WAITMODE         BV(2)   ///< Wait Mode Command
#define CKGR_MOR_MOSCRCEN         BV(3)   ///< Main On-Chip RC Oscillator Enable
#define CKGR_MOR_MOSCRCF_S        4
#define CKGR_MOR_MOSCRCF_M        (0x7 << CKGR_MOR_MOSCRCF_S)   ///< Main On-Chip RC Oscillator Frequency Selection
#define CKGR_MOR_MOSCRCF(value)   ((CKGR_MOR_MOSCRCF_M & ((value) << CKGR_MOR_MOSCRCF_S)))
#define   CKGR_MOR_MOSCRCF_4MHZ   (0x0 << CKGR_MOR_MOSCRCF_S)
#define   CKGR_MOR_MOSCRCF_8MHZ   (0x1 << CKGR_MOR_MOSCRCF_S)
#define   CKGR_MOR_MOSCRCF_12MHZ  (0x2 << CKGR_MOR_MOSCRCF_S)
#define CKGR_MOR_MOSCXTST_S       8
#define CKGR_MOR_MOSCXTST_M       (0xff << CKGR_MOR_MOSCXTST_S)   ///< Main Crystal Oscillator Start-up Time
#define CKGR_MOR_MOSCXTST(value)  ((CKGR_MOR_MOSCXTST_M & ((value) << CKGR_MOR_MOSCXTST_S)))
#define CKGR_MOR_KEY_S            16
#define CKGR_MOR_KEY_M            (0xffu << CKGR_MOR_KEY_S)   ///< Password
#define CKGR_MOR_KEY(value)       ((CKGR_MOR_KEY_M & ((value) << CKGR_MOR_KEY_S)))
#define CKGR_MOR_MOSCSEL          BV(24)   ///< Main Oscillator Selection
#define CKGR_MOR_CFDEN            BV(25)   ///< Clock Failure Detector Enable
/*\}*/

/**
 * Defines for bit fields in CKGR_MCFR register.
 */
/*\{*/
#define CKGR_MCFR_MAINF_M   0xffffu   ///< Main Clock Frequency mask
#define CKGR_MCFR_MAINFRDY  BV(16)    ///< Main Clock Ready
/*\}*/

/**
 * Defines for bit fields in CKGR_PLLR register.
 */
/*\{*/
#define CKGR_PLLR_DIV_M           0xff   ///< Divider mask
#define CKGR_PLLR_DIV(value)      ((CKGR_PLLR_DIV_M & (value))
#define CKGR_PLLR_PLLCOUNT_S      8
#define CKGR_PLLR_PLLCOUNT_M      (0x3f << CKGR_PLLR_PLLCOUNT_S)   ///< PLL Counter mask
#define CKGR_PLLR_PLLCOUNT(value) ((CKGR_PLLR_PLLCOUNT_M & ((value) << CKGR_PLLR_PLLCOUNT_S)))
#define CKGR_PLLR_MUL_S           16
#define CKGR_PLLR_MUL_M           (0x7ff << CKGR_PLLR_MUL_S)   ///< PLL Multiplier mask
#define CKGR_PLLR_MUL(value)      ((CKGR_PLLR_MUL_M & ((value) << CKGR_PLLR_MUL_S)))
#define CKGR_PLLR_STUCKTO1        BV(29)
/*\}*/

/**
 * Defines for bit fields in PMC_MCKR register.
 */
/*\{*/
#define PMC_MCKR_CSS_M           0x3   ///< Master Clock Source Selection mask
#define   PMC_MCKR_CSS_SLOW_CLK  0x0   ///< Slow Clock is selected
#define   PMC_MCKR_CSS_MAIN_CLK  0x1   ///< Main Clock is selected
#define   PMC_MCKR_CSS_PLL_CLK   0x2   ///< PLL Clock is selected
#define PMC_MCKR_PRES_S          4
#define PMC_MCKR_PRES_M          (0x7u << PMC_MCKR_PRES_S)    ///< Processor Clock Prescaler mask
#define   PMC_MCKR_PRES_CLK      (0x0u << PMC_MCKR_PRES_S)   ///< Selected clock
#define   PMC_MCKR_PRES_CLK_2    (0x1u << PMC_MCKR_PRES_S)   ///< Selected clock divided by 2
#define   PMC_MCKR_PRES_CLK_4    (0x2u << PMC_MCKR_PRES_S)   ///< Selected clock divided by 4
#define   PMC_MCKR_PRES_CLK_8    (0x3u << PMC_MCKR_PRES_S)   ///< Selected clock divided by 8
#define   PMC_MCKR_PRES_CLK_16   (0x4u << PMC_MCKR_PRES_S)   ///< Selected clock divided by 16
#define   PMC_MCKR_PRES_CLK_32   (0x5u << PMC_MCKR_PRES_S)   ///< Selected clock divided by 32
#define   PMC_MCKR_PRES_CLK_64   (0x6u << PMC_MCKR_PRES_S)   ///< Selected clock divided by 64
#define   PMC_MCKR_PRES_CLK_3    (0x7u << PMC_MCKR_PRES_S)   ///< Selected clock divided by 3
#define PMC_MCKR_PLLDIV2         BV(12)   ///< PLL Divisor by 2
/*\}*/

/**
 * Defines for bit fields in PMC_PCK[3] register.
 */
/*\{*/
#define PMC_PCK_CSS_M          0x7   ///< Master Clock Source Selection mask
#define   PMC_PCK_CSS_SLOW     0x0   ///< Slow Clock is selected
#define   PMC_PCK_CSS_MAIN     0x1   ///< Main Clock is selected
#define   PMC_PCK_CSS_PLL      0x2   ///< PLL Clock is selected
#define   PMC_PCK_CSS_MCK      0x4   ///< Master Clock is selected
#define PMC_PCK_PRES_S 4
#define PMC_PCK_PRES_M         (0x7u << PMC_PCK_PRES_S)   ///< Programmable Clock Prescaler
#define   PMC_PCK_PRES_CLK     (0x0u << PMC_PCK_PRES_S)   ///< Selected clock
#define   PMC_PCK_PRES_CLK_2   (0x1u << PMC_PCK_PRES_S)   ///< Selected clock divided by 2
#define   PMC_PCK_PRES_CLK_4   (0x2u << PMC_PCK_PRES_S)   ///< Selected clock divided by 4
#define   PMC_PCK_PRES_CLK_8   (0x3u << PMC_PCK_PRES_S)   ///< Selected clock divided by 8
#define   PMC_PCK_PRES_CLK_16  (0x4u << PMC_PCK_PRES_S)   ///< Selected clock divided by 16
#define   PMC_PCK_PRES_CLK_32  (0x5u << PMC_PCK_PRES_S)   ///< Selected clock divided by 32
#define   PMC_PCK_PRES_CLK_64  (0x6u << PMC_PCK_PRES_S)   ///< Selected clock divided by 64
/*\}*/

/**
 * Defines for bit fields in PMC_IER register.
 */
/*\{*/
#define PMC_IER_MOSCXTS   BV(0)   ///< Main Crystal Oscillator Status Interrupt Enable
#define PMC_IER_LOCK      BV(1)   ///< PLL Lock Interrupt Enable
#define PMC_IER_MCKRDY    BV(3)   ///< Master Clock Ready Interrupt Enable
#define PMC_IER_PCKRDY0   BV(8)   ///< Programmable Clock Ready 0 Interrupt Enable
#define PMC_IER_PCKRDY1   BV(9)   ///< Programmable Clock Ready 1 Interrupt Enable
#define PMC_IER_PCKRDY2   BV(10)  ///< Programmable Clock Ready 2 Interrupt Enable
#define PMC_IER_MOSCSELS  BV(16)  ///< Main Oscillator Selection Status Interrupt Enable
#define PMC_IER_MOSCRCS   BV(17)  ///< Main On-Chip RC Status Interrupt Enable
#define PMC_IER_CFDEV     BV(18)  ///< Clock Failure Detector Event Interrupt Enable
/*\}*/

/**
 * Defines for bit fields in PMC_IDR register.
 */
/*\{*/
#define PMC_IDR_MOSCXTS   BV(0)   ///< Main Crystal Oscillator Status Interrupt Disable
#define PMC_IDR_LOCK      BV(1)   ///< PLL Lock Interrupt Disable
#define PMC_IDR_MCKRDY    BV(3)   ///< Master Clock Ready Interrupt Disable
#define PMC_IDR_PCKRDY0   BV(8)   ///< Programmable Clock Ready 0 Interrupt Disable
#define PMC_IDR_PCKRDY1   BV(9)   ///< Programmable Clock Ready 1 Interrupt Disable
#define PMC_IDR_PCKRDY2   BV(10)  ///< Programmable Clock Ready 2 Interrupt Disable
#define PMC_IDR_MOSCSELS  BV(16)  ///< Main Oscillator Selection Status Interrupt Disable
#define PMC_IDR_MOSCRCS   BV(17)  ///< Main On-Chip RC Status Interrupt Disable
#define PMC_IDR_CFDEV     BV(18)  ///< Clock Failure Detector Event Interrupt Disable
/*\}*/

/**
 * Defines for bit fields in PMC_SR register.
 */
/*\{*/
#define PMC_SR_MOSCXTS   BV(0)   ///< Main XTAL Oscillator Status
#define PMC_SR_LOCK      BV(1)   ///< PLL Lock Status
#define PMC_SR_MCKRDY    BV(3)   ///< Master Clock Status
#define PMC_SR_OSCSELS   BV(7)   ///< Slow Clock Oscillator Selection
#define PMC_SR_PCKRDY0   BV(8)   ///< Programmable Clock Ready Status
#define PMC_SR_PCKRDY1   BV(9)   ///< Programmable Clock Ready Status
#define PMC_SR_PCKRDY2   BV(10)  ///< Programmable Clock Ready Status
#define PMC_SR_MOSCSELS  BV(16)  ///< Main Oscillator Selection Status
#define PMC_SR_MOSCRCS   BV(17)  ///< Main On-Chip RC Oscillator Status
#define PMC_SR_CFDEV     BV(18)  ///< Clock Failure Detector Event
#define PMC_SR_CFDS      BV(19)  ///< Clock Failure Detector Status
#define PMC_SR_FOS       BV(20)  ///< Clock Failure Detector Fault Output Status
/*\}*/

/**
 * Defines for bit fields in PMC_IMR register.
 */
/*\{*/
#define PMC_IMR_MOSCXTS   BV(0)   ///< Main Crystal Oscillator Status Interrupt Mask
#define PMC_IMR_LOCK      BV(1)   ///< PLL Lock Interrupt Mask
#define PMC_IMR_MCKRDY    BV(3)   ///< Master Clock Ready Interrupt Mask
#define PMC_IMR_PCKRDY0   BV(8)   ///< Programmable Clock Ready 0 Interrupt Mask
#define PMC_IMR_PCKRDY1   BV(9)   ///< Programmable Clock Ready 1 Interrupt Mask
#define PMC_IMR_PCKRDY2   BV(10)  ///< Programmable Clock Ready 2 Interrupt Mask
#define PMC_IMR_MOSCSELS  BV(16)  ///< Main Oscillator Selection Status Interrupt Mask
#define PMC_IMR_MOSCRCS   BV(17)  ///< Main On-Chip RC Status Interrupt Mask
#define PMC_IMR_CFDEV     BV(18)  ///< Clock Failure Detector Event Interrupt Mask
/*\}*/

/**
 * Defines for bit fields in PMC_FSMR register.
 */
/*\{*/
#define PMC_FSMR_FSTT0   BV(0)   ///< Fast Startup Input Enable 0
#define PMC_FSMR_FSTT1   BV(1)   ///< Fast Startup Input Enable 1
#define PMC_FSMR_FSTT2   BV(2)   ///< Fast Startup Input Enable 2
#define PMC_FSMR_FSTT3   BV(3)   ///< Fast Startup Input Enable 3
#define PMC_FSMR_FSTT4   BV(4)   ///< Fast Startup Input Enable 4
#define PMC_FSMR_FSTT5   BV(5)   ///< Fast Startup Input Enable 5
#define PMC_FSMR_FSTT6   BV(6)   ///< Fast Startup Input Enable 6
#define PMC_FSMR_FSTT7   BV(7)   ///< Fast Startup Input Enable 7
#define PMC_FSMR_FSTT8   BV(8)   ///< Fast Startup Input Enable 8
#define PMC_FSMR_FSTT9   BV(9)   ///< Fast Startup Input Enable 9
#define PMC_FSMR_FSTT10  BV(10)  ///< Fast Startup Input Enable 10
#define PMC_FSMR_FSTT11  BV(11)  ///< Fast Startup Input Enable 11
#define PMC_FSMR_FSTT12  BV(12)  ///< Fast Startup Input Enable 12
#define PMC_FSMR_FSTT13  BV(13)  ///< Fast Startup Input Enable 13
#define PMC_FSMR_FSTT14  BV(14)  ///< Fast Startup Input Enable 14
#define PMC_FSMR_FSTT15  BV(15)  ///< Fast Startup Input Enable 15
#define PMC_FSMR_RTTAL   BV(16)  ///< RTT Alarm Enable
#define PMC_FSMR_RTCAL   BV(17)  ///< RTC Alarm Enable
#define PMC_FSMR_LPM     BV(20)  ///< Low Power Mode
/*\}*/

/**
 * Defines for bit fields in PMC_FSPR register.
 */
/*\{*/
#define PMC_FSPR_FSTP0   BV(0)   ///< Fast Startup Input Polarityx
#define PMC_FSPR_FSTP1   BV(1)   ///< Fast Startup Input Polarityx
#define PMC_FSPR_FSTP2   BV(2)   ///< Fast Startup Input Polarityx
#define PMC_FSPR_FSTP3   BV(3)   ///< Fast Startup Input Polarityx
#define PMC_FSPR_FSTP4   BV(4)   ///< Fast Startup Input Polarityx
#define PMC_FSPR_FSTP5   BV(5)   ///< Fast Startup Input Polarityx
#define PMC_FSPR_FSTP6   BV(6)   ///< Fast Startup Input Polarityx
#define PMC_FSPR_FSTP7   BV(7)   ///< Fast Startup Input Polarityx
#define PMC_FSPR_FSTP8   BV(8)   ///< Fast Startup Input Polarityx
#define PMC_FSPR_FSTP9   BV(9)   ///< Fast Startup Input Polarityx
#define PMC_FSPR_FSTP10  BV(10)  ///< Fast Startup Input Polarityx
#define PMC_FSPR_FSTP11  BV(11)  ///< Fast Startup Input Polarityx
#define PMC_FSPR_FSTP12  BV(12)  ///< Fast Startup Input Polarityx
#define PMC_FSPR_FSTP13  BV(13)  ///< Fast Startup Input Polarityx
#define PMC_FSPR_FSTP14  BV(14)  ///< Fast Startup Input Polarityx
#define PMC_FSPR_FSTP15  BV(15)  ///< Fast Startup Input Polarityx
/*\}*/

/**
 * Defines for bit fields in PMC_FOCR register.
 */
/*\{*/
#define PMC_FOCR_FOCLR  BV(0)   ///< Fault Output Clear
/*\}*/

/**
 * Defines for bit fields in PMC_WPMR register.
 */
/*\{*/
#define PMC_WPMR_WPEN          BV(0)   ///< Write Protect Enable
#define PMC_WPMR_WPKEY_S       8
#define PMC_WPMR_WPKEY_M       (0xffffff << PMC_WPMR_WPKEY_S)   ///< Write Protect key mask
#define PMC_WPMR_WPKEY(value)  ((PMC_WPMR_WPKEY_M & ((value) << PMC_WPMR_WPKEY_S)))
/*\}*/

/**
 * Defines for bit fields in PMC_WPSR register.
 */
/*\{*/
#define PMC_WPSR_WPVS      BV(0)   ///< Write Protect Violation Status
#define PMC_WPSR_WPVSRC_S  8
#define PMC_WPSR_WPVSRC_M  (0xffff << PMC_WPSR_WPVSRC_S)   ///< Write Protect Violation Source mask
/*\}*/

/**
 * Defines for bit fields in PMC_OCR register.
 */
/*\{*/
#define PMC_OCR_CAL4_M        0x7f  ///< RC Oscillator Calibration bits for 4 MHz mask
#define PMC_OCR_CAL4(value)   (PMC_OCR_CAL4_M & (value))
#define PMC_OCR_SEL4          BV(7)   ///< Selection of RC Oscillator Calibration bits for 4 MHz
#define PMC_OCR_CAL8_S        8
#define PMC_OCR_CAL8_M        (0x7f << PMC_OCR_CAL8_S)   ///< RC Oscillator Calibration bits for 8 MHz mask
#define PMC_OCR_CAL8(value)   ((PMC_OCR_CAL8_M & ((value) << PMC_OCR_CAL8_S)))
#define PMC_OCR_SEL8          BV(15)  ///< Selection of RC Oscillator Calibration bits for 8 MHz
#define PMC_OCR_CAL12_S       16
#define PMC_OCR_CAL12_M       (0x7f << PMC_OCR_CAL12_S)   ///< RC Oscillator Calibration bits for 12 MHz mask
#define PMC_OCR_CAL12(value)  ((PMC_OCR_CAL12_M & ((value) << PMC_OCR_CAL12_S)))
#define PMC_OCR_SEL12         BV(23)   ///< Selection of RC Oscillator Calibration bits for 12 MHz
/*\}*/


#endif /* SAM3_PMC_H */

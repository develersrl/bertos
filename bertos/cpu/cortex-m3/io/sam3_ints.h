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
 * \brief SAM3 interrupt definitions.
 */

#ifndef SAM3_INTS_H
#define SAM3_INTS_H

/**
 * Defines for the fault assignments.
 */
/*\{*/
#define FAULT_NMI               2           ///< NMI fault
#define FAULT_HARD              3           ///< Hard fault
#define FAULT_MPU               4           ///< MPU fault
#define FAULT_BUS               5           ///< Bus fault
#define FAULT_USAGE             6           ///< Usage fault
#define FAULT_SVCALL            11          ///< SVCall
#define FAULT_DEBUG             12          ///< Debug monitor
#define FAULT_PENDSV            14          ///< PendSV
#define FAULT_SYSTICK           15          ///< System Tick
/*\}*/

/**
 * Defines for the interrupt assignments.
 */
/*\{*/
#define INT_PERIPH_BASE  16

#define INT_SUPC    (INT_PERIPH_BASE + SUPC_ID)   ///< Supply Controller (SUPC)
#define INT_RSTC    (INT_PERIPH_BASE + RSTC_ID)   ///< Reset Controller (RSTC)
#define INT_RTC     (INT_PERIPH_BASE + RTC_ID)    ///< Real Time Clock (RTC)
#define INT_RTT     (INT_PERIPH_BASE + RTT_ID)    ///< Real Time Timer (RTT)
#define INT_WDT     (INT_PERIPH_BASE + WDT_ID)    ///< Watchdog Timer (WDT)
#define INT_PMC     (INT_PERIPH_BASE + PMC_ID)    ///< Power Management Controller (PMC)
#define INT_EFC     (INT_PERIPH_BASE + EFC_ID)    ///< Enhanced Flash Controller (EFC)
#define INT_UART0   (INT_PERIPH_BASE + UART0_ID)  ///< UART 0 (UART0)
#define INT_UART1   (INT_PERIPH_BASE + UART1_ID)  ///< UART 1 (UART1)
#define INT_PIOA    (INT_PERIPH_BASE + PIOA_ID)   ///< Parallel I/O Controller A (PIOA)
#define INT_PIOB    (INT_PERIPH_BASE + PIOB_ID)   ///< Parallel I/O Controller B (PIOB)
#define INT_PIOC    (INT_PERIPH_BASE + PIOC_ID)   ///< Parallel I/O Controller C (PIOC)
#define INT_US0     (INT_PERIPH_BASE + US0_ID)    ///< USART 0 (USART0)
#define INT_US1     (INT_PERIPH_BASE + US1_ID)    ///< USART 1 (USART1)
#define INT_TWI0    (INT_PERIPH_BASE + TWI0_ID)   ///< Two Wire Interface 0 (TWI0)
#define INT_TWI1    (INT_PERIPH_BASE + TWI1_ID)   ///< Two Wire Interface 1 (TWI1)
#define INT_SPI0    (INT_PERIPH_BASE + SPI0_ID)   ///< Serial Peripheral Interface (SPI)
#define INT_TC0     (INT_PERIPH_BASE + TC0_ID)    ///< Timer/Counter 0 (TC0)
#define INT_TC1     (INT_PERIPH_BASE + TC1_ID)    ///< Timer/Counter 1 (TC1)
#define INT_TC2     (INT_PERIPH_BASE + TC2_ID)    ///< Timer/Counter 2 (TC2)
#define INT_TC3     (INT_PERIPH_BASE + TC3_ID)    ///< Timer/Counter 3 (TC3)
#define INT_TC4     (INT_PERIPH_BASE + TC4_ID)    ///< Timer/Counter 4 (TC4)
#define INT_TC5     (INT_PERIPH_BASE + TC5_ID)    ///< Timer/Counter 5 (TC5)
#define INT_ADC     (INT_PERIPH_BASE + ADC_ID)    ///< Analog To Digital Converter (ADC)
#define INT_DACC    (INT_PERIPH_BASE + DACC_ID)   ///< Digital To Analog Converter (DACC)
#define INT_PWM     (INT_PERIPH_BASE + PWM_ID)    ///< Pulse Width Modulation (PWM)
/*\}*/

/**
 * Total number of interrupts.
 */
/*\{*/
#define NUM_INTERRUPTS  48
/*\}*/

#endif /* SAM3_INTS_H */

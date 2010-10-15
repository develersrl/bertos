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
 * \brief AT91SAM3 interrupt definitions.
 */

#ifndef SAM3_INTS_H
#define SAM3_INTS_H

/**
 * Defines for the interrupt assignments.
 */
/*\{*/
#define INT_SUPC     0  ///< Supply Controller (SUPC)
#define INT_RSTC     1  ///< Reset Controller (RSTC)
#define INT_RTC      2  ///< Real Time Clock (RTC)
#define INT_RTT      3  ///< Real Time Timer (RTT)
#define INT_WDT      4  ///< Watchdog Timer (WDT)
#define INT_PMC      5  ///< Power Management Controller (PMC)
#define INT_EFC      6  ///< Enhanced Flash Controller (EFC)
#define INT_UART0    8  ///< UART 0 (UART0)
#define INT_UART1    9  ///< UART 1 (UART1)
#define INT_PIOA    11  ///< Parallel I/O Controller A (PIOA)
#define INT_PIOB    12  ///< Parallel I/O Controller B (PIOB)
#define INT_PIOC    13  ///< Parallel I/O Controller C (PIOC)
#define INT_USART0  14  ///< USART 0 (USART0)
#define INT_USART1  15  ///< USART 1 (USART1)
#define INT_TWI0    19  ///< Two Wire Interface 0 (TWI0)
#define INT_TWI1    20  ///< Two Wire Interface 1 (TWI1)
#define INT_SPI     21  ///< Serial Peripheral Interface (SPI)
#define INT_TC0     23  ///< Timer/Counter 0 (TC0)
#define INT_TC1     24  ///< Timer/Counter 1 (TC1)
#define INT_TC2     25  ///< Timer/Counter 2 (TC2)
#define INT_TC3     26  ///< Timer/Counter 3 (TC3)
#define INT_TC4     27  ///< Timer/Counter 4 (TC4)
#define INT_TC5     28  ///< Timer/Counter 5 (TC5)
#define INT_ADC     29  ///< Analog To Digital Converter (ADC)
#define INT_DACC    30  ///< Digital To Analog Converter (DACC)
#define INT_PWM     31  ///< Pulse Width Modulation (PWM)
/*\}*/

/**
 * Total number of interrupts.
 */
/*\{*/
#define NUM_INTERRUPTS  32
/*\}*/

#endif /* SAM3_INTS_H */

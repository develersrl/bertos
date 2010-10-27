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
 * \author Stefano Fedrigo <aleph@develer.com>
 */

#ifndef SAM3_H
#define SAM3_H

#include <cpu/detect.h>
#include <cfg/compiler.h>

/*
 * Peripherals IDs.
 */
/*\{*/
#if CPU_CM3_SAM3N
	#define SUPC_ID      0   ///< Supply Controller (SUPC)
	#define RSTC_ID      1   ///< Reset Controller (RSTC)
	#define RTC_ID       2   ///< Real Time Clock (RTC)
	#define RTT_ID       3   ///< Real Time Timer (RTT)
	#define WDT_ID       4   ///< Watchdog Timer (WDT)
	#define PMC_ID       5   ///< Power Management Controller (PMC)
	#define EFC_ID       6   ///< Enhanced Flash Controller (EFC)
	#define UART0_ID     8   ///< UART 0 (UART0)
	#define UART1_ID     9   ///< UART 1 (UART1)
	#define PIOA_ID     11   ///< Parallel I/O Controller A (PIOA)
	#define PIOB_ID     12   ///< Parallel I/O Controller B (PIOB)
	#define PIOC_ID     13   ///< Parallel I/O Controller C (PIOC)
	#define US0_ID      14   ///< USART 0 (USART0)
	#define US1_ID      15   ///< USART 1 (USART1)
	#define TWI0_ID     19   ///< Two Wire Interface 0 (TWI0)
	#define TWI1_ID     20   ///< Two Wire Interface 1 (TWI1)
	#define SPI0_ID     21   ///< Serial Peripheral Interface (SPI)
	#define TC0_ID      23   ///< Timer/Counter 0 (TC0)
	#define TC1_ID      24   ///< Timer/Counter 1 (TC1)
	#define TC2_ID      25   ///< Timer/Counter 2 (TC2)
	#define TC3_ID      26   ///< Timer/Counter 3 (TC3)
	#define TC4_ID      27   ///< Timer/Counter 4 (TC4)
	#define TC5_ID      28   ///< Timer/Counter 5 (TC5)
	#define ADC_ID      29   ///< Analog To Digital Converter (ADC)
	#define DACC_ID     30   ///< Digital To Analog Converter (DACC)
	#define PWM_ID      31   ///< Pulse Width Modulation (PWM)
#else
	#error Peripheral IDs undefined
#endif
/*\}*/

/*
 * Hardware features for drivers.
 */
#define USART_HAS_PDC  1
#define SPI_HAS_PDC    1

/* PDC registers */
#define PERIPH_RPR_OFF  0x100  // Receive Pointer Register.
#define PERIPH_RCR_OFF  0x104  // Receive Counter Register.
#define PERIPH_TPR_OFF  0x108  // Transmit Pointer Register.
#define PERIPH_TCR_OFF  0x10C  // Transmit Counter Register.
#define PERIPH_RNPR_OFF 0x110  // Receive Next Pointer Register.
#define PERIPH_RNCR_OFF 0x114  // Receive Next Counter Register.
#define PERIPH_TNPR_OFF 0x118  // Transmit Next Pointer Register.
#define PERIPH_TNCR_OFF 0x11C  // Transmit Next Counter Register.
#define PERIPH_PTCR_OFF 0x120  // PDC Transfer Control Register.
#define PERIPH_PTSR_OFF 0x124  // PDC Transfer Status Register.

#define PDC_RXTEN  0
#define PDC_RXTDIS 1
#define PDC_TXTEN  8
#define PDC_TXTDIS 9


#include "sam3_sysctl.h"
#include "sam3_pmc.h"
#include "sam3_ints.h"
#include "sam3_pio.h"
#include "sam3_nvic.h"
#include "sam3_uart.h"
#include "sam3_usart.h"
#include "sam3_spi.h"
#include "sam3_flash.h"
#include "sam3_wdt.h"

/**
 * UART I/O pins
 */
/*\{*/
#if CPU_CM3_SAM3U
	#define RXD0   11
	#define TXD0   12
#else
	#define RXD0    9
	#define TXD0   10
	#define RXD1    2
	#define TXD1    3
#endif
/*\}*/

/**
 * PIO I/O pins
 */
/*\{*/
#if CPU_CM3_SAM3U
	#define SPI0_SPCK   15
	#define SPI0_MOSI   14
	#define SPI0_MISO   13
#else
	#define SPI0_SPCK   14
	#define SPI0_MOSI   13
	#define SPI0_MISO   12
#endif
/*\}*/
#endif /* SAM3_H */

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
 * Copyright 2007 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \version $Id$
 *
 * \author Francesco Sacchi <batt@develer.com>
 *
 * AT91SAM7S256 register definitions.
 * This file is based on NUT/OS implementation. See license below.
 */

/*
 * Copyright (C) 2006-2007 by egnite Software GmbH. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holders nor the names of
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY EGNITE SOFTWARE GMBH AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL EGNITE
 * SOFTWARE GMBH OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * For additional information see http://www.ethernut.de/
 */

#ifndef AT91SAM7S256_H
#define AT91SAM7S256_H

#include <cfg/compiler.h>

#define FLASH_BASE      0x100000UL
#define RAM_BASE        0x200000UL

#define TC_BASE         0xFFFA0000      ///< Timer/counter base address.
#define UDP_BASE        0xFFFB0000      ///< USB device port base address.
#define TWI_BASE        0xFFFB8000      ///< Two-wire interface base address.
#define USART0_BASE     0xFFFC0000      ///< USART 0 base address.
#define USART1_BASE     0xFFFC4000      ///< USART 1 base address.
#define PWMC_BASE       0xFFFCC000      ///< PWM controller base address.
#define SSC_BASE        0xFFFD4000      ///< Serial synchronous controller base address.
#define ADC_BASE        0xFFFD8000      ///< ADC base address.
#define SPI_BASE        0xFFFE0000      ///< SPI0 base address.

#define AIC_BASE        0xFFFFF000      ///< AIC base address.
#define DBGU_BASE       0xFFFFF200      ///< DBGU base address.
#define PIOA_BASE       0xFFFFF400      ///< PIO A base address.
#define PMC_BASE        0xFFFFFC00      ///< PMC base address.
#define RSTC_BASE       0xFFFFFD00      ///< Resect controller register base address.
#define RTT_BASE        0xFFFFFD20      ///< Realtime timer base address.
#define PIT_BASE        0xFFFFFD30      ///< Periodic interval timer base address.
#define WDT_BASE        0xFFFFFD40      ///< Watch Dog register base address.
#define VREG_BASE       0xFFFFFD60      ///< Voltage regulator mode controller base address.
#define MC_BASE         0xFFFFFF00      ///< Memory controller base.

#define PIO_HAS_MULTIDRIVER        1
#define PIO_HAS_PULLUP             1
#define PIO_HAS_PERIPHERALSELECT   1
#define PIO_HAS_OUTPUTWRITEENABLE  1

#define DBGU_HAS_PDC               1
#define SPI_HAS_PDC                1
#define SSC_HAS_PDC                1
#define USART_HAS_PDC              1

#include "at91_aic.h"
#include "at91_pit.h"
#include "at91_pmc.h"
#include "at91_mc.h"
#include "at91_wdt.h"
#include "at91_rstc.h"
#include "at91_pio.h"
#include "at91_us.h"
#include "at91_dbgu.h"
//TODO: add other peripherals

/**
 * Peripheral Identifiers and Interrupts
 *\{
 */
#define FIQ_ID      0       ///< Fast interrupt ID.
#define SYSC_ID     1       ///< System controller interrupt.
#define PIOA_ID     2       ///< Parallel I/O controller ID.
/* ID 3 is reserved */
#define ADC_ID      4       ///< Analog to digital converter ID.
#define SPI_ID      5       ///< Serial peripheral interface ID.
#define US0_ID      6       ///< USART 0 ID.
#define US1_ID      7       ///< USART 1 ID.
#define SSC_ID      8       ///< Synchronous serial controller ID.
#define TWI_ID      9       ///< Two-wire interface ID.
#define PWMC_ID     10      ///< PWM controller ID.
#define UDP_ID      11      ///< USB device port ID.
#define TC0_ID      12      ///< Timer 0 ID.
#define TC1_ID      13      ///< Timer 1 ID.
#define TC2_ID      14      ///< Timer 2 ID.

#define IRQ0_ID     30      ///< External interrupt 0 ID.
#define IRQ1_ID     31      ///< External interrupt 1 ID.
/*\}*/

/**
 * USART pins name
 *\{
 */
#define RXD0        5
#define TXD0        6
#define RXD1       21
#define TXD1       22
/*\}*/

/**
 * SPI pins name
 *\{
 */
#define NPCS0      11
#define MISO       12
#define MOSI       13
#define SPCK       14
/*\}*/

#endif /* AT91SAM7S256_H */

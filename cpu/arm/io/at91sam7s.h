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
 * AT91SAM7S register definitions.
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

#ifndef AT91SAM7S_H
#define AT91SAM7S_H

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

#include "at91_aic.h"
#include "at91_pit.h"
#include "at91_pmc.h"
//TODO: add other peripherals

/** Peripheral Identifiers and Interrupts */
/*\{*/
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

#warning Revise me after this line!

#define PERIPH_RPR_OFF  0x00000100      ///< Receive pointer register offset.
#define PERIPH_RCR_OFF  0x00000104      ///< Receive counter register offset.
#define PERIPH_TPR_OFF  0x00000108      ///< Transmit pointer register offset.
#define PERIPH_TCR_OFF  0x0000010C      ///< Transmit counter register offset.
#define PERIPH_RNPR_OFF 0x00000110      ///< Receive next pointer register offset.
#define PERIPH_RNCR_OFF 0x00000114      ///< Receive next counter register offset.
#define PERIPH_TNPR_OFF 0x00000118      ///< Transmit next pointer register offset.
#define PERIPH_TNCR_OFF 0x0000011C      ///< Transmit next counter register offset.
#define PERIPH_PTCR_OFF 0x00000120      ///< PDC transfer control register offset.
#define PERIPH_PTSR_OFF 0x00000124      ///< PDC transfer status register offset.

#define PDC_RXTEN       0x00000001      ///< Receiver transfer enable.
#define PDC_RXTDIS      0x00000002      ///< Receiver transfer disable.
#define PDC_TXTEN       0x00000100      ///< Transmitter transfer enable.
#define PDC_TXTDIS      0x00000200      ///< Transmitter transfer disable.

#define DBGU_HAS_PDC
#define SPI_HAS_PDC
#define SSC_HAS_PDC
#define USART_HAS_PDC

#define PIO_HAS_MULTIDRIVER
#define PIO_HAS_PULLUP
#define PIO_HAS_PERIPHERALSELECT
#define PIO_HAS_OUTPUTWRITEENABLE


/** Historical SPI0 Peripheral Multiplexing Names */
/*\{*/
#define SPI0_NPCS0_PA12A	12	///< Port bit number on PIO-A Perpheral A.
#define SPI0_NPCS1_PA13A	13	///< Port bit number on PIO-A Perpheral A.
#define SPI0_NPCS1_PA07B	7	///< Port bit number on PIO-A Perpheral B.
#define SPI0_NPCS1_PB13B	13	///< Port bit number on PIO-B Perpheral B.
#define SPI0_NPCS2_PA14A	14	///< Port bit number on PIO-A Perpheral A.
#define SPI0_NPCS2_PA08B	8	///< Port bit number on PIO-A Perpheral B.
#define SPI0_NPCS2_PB14B	14	///< Port bit number on PIO-B Perpheral B.
#define SPI0_NPCS3_PA15A	15	///< Port bit number on PIO-A Perpheral A.
#define SPI0_NPCS3_PA09B	9	///< Port bit number on PIO-A Perpheral B.
#define SPI0_NPCS3_PB17B	17	///< Port bit number on PIO-B Perpheral B.
#define SPI0_MISO_PA16A		16	///< Port bit number on PIO-A Perpheral A.
#define SPI0_MOSI_PA17A		17	///< Port bit number on PIO-A Perpheral A.
#define SPI0_SPCK_PA18A		18	///< Port bit number on PIO-A Perpheral A.
/*\}*/

/** USART Peripheral Multiplexing */
/*\{*/
#define PA0_RXD0_A          0
#define PA1_TXD0_A          1
#define PA2_SCK0_A          2
#define PA3_RTS0_A          3
#define PA4_CTS0_A          4

#define PA5_RXD1_A          5
#define PA6_TXD1_A          6
#define PA7_SCK1_A          7
#define PA8_RTS1_A          8
#define PA9_CTS1_A          9
#define PB23_DCD1_B         23
#define PB24_DSR1_B         24
#define PB25_DTR1_B         25
#define PB26_RI1_B          26
/*\}*/

/** SPI Peripheral Multiplexing */
/*\{*/
#define PA16_SPI0_MISO_A    16
#define PA17_SPI0_MOSI_A    17
#define PA18_SPI0_SPCK_A    18
#define PA12_SPI0_NPCS0_A   12
#define PA13_SPI0_NPCS1_A   13
#define PA7_SPI0_NPCS1_B    7
#define PA14_SPI0_NPCS2_A   14
#define PB14_SPI0_NPCS2_B   14
#define PA8_SPI0_NPCS2_B    8
#define PA15_SPI0_NPCS3_A   15
#define PA9_SPI0_NPCS3_B    9

#define SPI0_PINS           _BV(PA16_SPI0_MISO_A) | _BV(PA17_SPI0_MOSI_A) | _BV(PA18_SPI0_SPCK_A)
#define SPI0_PIO_BASE       PIOA_BASE
#define SPI0_PSR_OFF        PIO_ASR_OFF

#define SPI0_CS0_PIN        _BV(PA12_SPI0_NPCS0_A)
#define SPI0_CS0_PIO_BASE   PIOA_BASE
#define SPI0_CS0_PSR_OFF    PIO_ASR_OFF

#ifndef SPI0_CS1_PIN
#define SPI0_CS1_PIN        _BV(PA13_SPI0_NPCS1_A)
#define SPI0_CS1_PIO_BASE   PIOA_BASE
#define SPI0_CS1_PSR_OFF    PIO_ASR_OFF
#endif

#ifndef SPI0_CS2_PIN
#define SPI0_CS2_PIN        _BV(PA14_SPI0_NPCS2_A)
#define SPI0_CS2_PIO_BASE   PIOA_BASE
#define SPI0_CS2_PSR_OFF    PIO_ASR_OFF
#endif

#ifndef SPI0_CS3_PIN
#define SPI0_CS3_PIN        _BV(PA15_SPI0_NPCS3_A)
#define SPI0_CS3_PIO_BASE   PIOA_BASE
#define SPI0_CS3_PSR_OFF    PIO_ASR_OFF
#endif

#define PA24_SPI1_MISO_B    24
#define PA23_SPI1_MOSI_B    23
#define PA22_SPI1_SPCK_B    22
#define PA21_SPI1_NPCS0_B   21
#define PA25_SPI1_NPCS1_B   25
#define PB13_SPI0_NPCS1_B   13
#define PA2_SPI1_NPCS1_B    2
#define PB10_SPI1_NPCS1_B   10
#define PA26_SPI1_NPCS2_B   26
#define PA3_SPI1_NPCS2_B    3
#define PB11_SPI1_NPCS2_B   11
#define PB17_SPI0_NPCS3_B   17
#define PA4_SPI1_NPCS3_B    4
#define PA29_SPI1_NPCS3_B   29
#define PB16_SPI1_NPCS3_B   16

#define SPI1_PINS           _BV(PA24_SPI1_MISO_B) | _BV(PA23_SPI1_MOSI_B) | _BV(PA22_SPI1_SPCK_B)
#define SPI1_PIO_BASE       PIOA_BASE
#define SPI1_PSR_OFF        PIO_BSR_OFF

#define SPI1_CS0_PIN        _BV(PA21_SPI1_NPCS0_B)
#define SPI1_CS0_PIO_BASE   PIOA_BASE
#define SPI1_CS0_PSR_OFF    PIO_BSR_OFF

#ifndef SPI1_CS1_PIN
#define SPI1_CS1_PIN        _BV(PA25_SPI1_NPCS1_B)
#define SPI1_CS1_PIO_BASE   PIOA_BASE
#define SPI1_CS1_PSR_OFF    PIO_BSR_OFF
#endif

#ifndef SPI1_CS2_PIN
#define SPI1_CS2_PIN        _BV(PA26_SPI1_NPCS2_B)
#define SPI1_CS2_PIO_BASE   PIOA_BASE
#define SPI1_CS2_PSR_OFF    PIO_BSR_OFF
#endif

#ifndef SPI1_CS3_PIN
#define SPI1_CS3_PIN        _BV(PA29_SPI1_NPCS3_B)
#define SPI1_CS3_PIO_BASE   PIOA_BASE
#define SPI1_CS3_PSR_OFF    PIO_BSR_OFF
#endif

/*\}*/

/** EMAC Interface Peripheral Multiplexing */
/*\{*/
#define PB0_ETXCK_EREFCK_A  0
#define PB1_ETXEN_A         1
#define PB2_ETX0_A          2
#define PB3_ETX1_A          3
#define PB4_ECRS_A          4
#define PB5_ERX0_A          5
#define PB6_ERX1_A          6
#define PB7_ERXER_A         7
#define PB8_EMDC_A          8
#define PB9_EMDIO_A         9
#define PB10_ETX2_A         10
#define PB11_ETX3_A         11
#define PB12_ETXER_A        12
#define PB13_ERX2_A         13
#define PB14_ERX3_A         14
#define PB15_ERXDV_ECRSDV_A 15
#define PB16_ECOL_A         16
#define PB17_ERXCK_A        17
#define PB18_EF100_A        18
/*\}*/

/** Debug Unit Peripheral Multiplexing */
/*\{*/
#define PA27_DRXD_A         27
#define PA28_DTXD_A         28
/*\}*/

/** Synchronous Serial Controller Peripheral Multiplexing */
/*\{*/
#define PA23_TD_A           23  ///< Transmit data pin.
#define PA24_RD_A           24  ///< Receive data pin.
#define PA22_TK_A           22  ///< Transmit clock pin.
#define PA25_RK_A           25  ///< Receive clock pin.
#define PA21_TF_A           21  ///< Transmit frame sync. pin.
#define PA26_RF_A           26  ///< Receive frame sync. pin.
/*\}*/

/** Two Wire Interface Peripheral Multiplexing */
/*\{*/
#define PA10_TWD_A          10  ///< Two wire serial data pin.
#define PA11_TWCK_A         11  ///< Two wire serial clock pin.
/*\}*/

/** Timer/Counter Peripheral Multiplexing */
/*\{*/
#define PB23_TIOA0_A        23
#define PB24_TIOB0_A        24
#define PB12_TCLK0_B        12

#define PB25_TIOA1_A        25
#define PB26_TIOB1_A        26
#define PB19_TCLK1_B        19

#define PB27_TIOA2_A        27
#define PB28_TIOB2_A        28
#define PA15_TCLK2_B        15
/*\}*/

/** Clocks, Oscillators and PLLs Peripheral Multiplexing */
/*\{*/
#define PB0_PCK0_B          0
#define PB20_PCK0_B         20
#define PA13_PCK1_B         13
#define PB29_PCK1_A         29
#define PB21_PCK1_B         21
#define PA30_PCK2_B         30
#define PB30_PCK2_A         30
#define PB22_PCK2_B         22
#define PA27_PCK3_B         27
/*\}*/

/** Advanced Interrupt Controller Peripheral Multiplexing */
/*\{*/
#define PA29_FIQ_A          29
#define PA30_IRQ0_A         30
#define PA14_IRQ1_B         14
/*\}*/

/** ADC Interface Peripheral Multiplexing */
/*\{*/
#define PB18_ADTRG_B        18  ///< ADC trigger pin.
/*\}*/

/** CAN Interface Peripheral Multiplexing */
/*\{*/
#define PA19_CANRX_A        19
#define PA20_CANTX_A        20
/*\}*/

/** PWM Peripheral Multiplexing */
/*\{*/
#define PB19_PWM0_A         19
#define PB27_PWM0_B         27
#define PB20_PWM1_A         20
#define PB28_PWM1_B         28
#define PB21_PWM2_A         21
#define PB29_PWM2_B         29
#define PB22_PWM3_A         22
#define PB30_PWM3_B         30
/*\}*/

#endif /* AT91SAM7S_H */

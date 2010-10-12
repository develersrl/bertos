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

#if CPU_CM3_AT91SAM3N
	/*
	 * Redefine CPU detect macros for compatibility with stock
	 * Atmel IO include file.
	 */
	#if CPU_CM3_AT91SAM3N4
		#define sam3n4
	#elif CPU_CM3_AT91SAM3N2
		#define sam3n2
	#elif CPU_CM3_AT91SAM3N1
		#define sam3n1
	#endif
	#include "sam3n.h"
#else
	#error Unimplemented CPU
#endif

#include "sam3_nvic.h"

/**
 * Total number of interrupts.
 */
/*\{*/
#define NUM_INTERRUPTS  32
/*\}*/

/**
 * UART PIO pins
 */
/*\{*/
#ifdef CPU_CM3_AT91SAM3U
	#define GPIO_UART0_RX_PIN   BV(11)
	#define GPIO_UART0_TX_PIN   BV(12)
#else
	#define GPIO_UART0_RX_PIN   BV(9)
	#define GPIO_UART0_TX_PIN   BV(10)
	#define GPIO_UART1_RX_PIN   BV(2)
	#define GPIO_UART1_TX_PIN   BV(3)
#endif
/*\}*/
#if 0
#include "sam3_sysctl.h"
#include "sam3_pmc.h"
#include "sam3_memmap.h"
#include "sam3_ints.h"
#include "sam3_gpio.h"
#include "sam3_uart.h"
#include "sam3_flash.h"
#endif

#endif /* SAM3_H */

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
 * Copyright 2012 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief STM32F2xx SYSCFG definition.
 */

#ifndef STM32_SYSCFG_H
#define STM32_SYSCFG_H

#include <cfg/compiler.h>

#if CPU_CM3_STM32F1
	#warning __FILTER_NEXT_WARNING__
	#warning Not supported
#elif CPU_CM3_STM32F2

struct stm32_syscfg
{
	reg32_t  MEMRMP;       /*!< SYSCFG memory remap register,                      Address offset: 0x00      */
	reg32_t  PMC;          /*!< SYSCFG peripheral mode configuration register,     Address offset: 0x04      */
	reg32_t  EXTICR[4];    /*!< SYSCFG external interrupt configuration registers, Address offset: 0x08-0x14 */
	uint32_t RESERVED[2];  /*!< Reserved, 0x18-0x1C                                                          */
	reg32_t  CMPCR;        /*!< SYSCFG Compensation cell control register,         Address offset: 0x20      */
};

#define SYSCFG ((struct stm32_syscfg *) SYSCFG_BASE)

#else
	#error Unknown CPU
#endif

#endif /* STM32_SYSCFG_H */

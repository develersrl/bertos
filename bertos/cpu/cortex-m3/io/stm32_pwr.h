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
 * Copyright 2011 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief STM32 Power Control.
 *
 * \author Andrea Righi <arighi@develer.com>
 */
#ifndef STM32_PWR_H
#define STM32_PWR_H

#include <io/stm32.h>

/* Power Control */
struct PWR
{
	reg32_t CR;
	reg32_t CSR;
};

#if CPU_CM3_STM32L1

/******************************************************************************/
/*                                                                            */
/*                          Power Control (PWR)                               */
/*                                                                            */
/******************************************************************************/

/********************  Bit definition for PWR_CR register  ********************/
#define  PWR_CR_LPSDSR                       ((uint16_t)0x0001)     /*!< Low-power deepsleep/sleep/low power run */
#define  PWR_CR_PDDS                         ((uint16_t)0x0002)     /*!< Power Down Deepsleep */
#define  PWR_CR_CWUF                         ((uint16_t)0x0004)     /*!< Clear Wakeup Flag */
#define  PWR_CR_CSBF                         ((uint16_t)0x0008)     /*!< Clear Standby Flag */
#define  PWR_CR_PVDE                         ((uint16_t)0x0010)     /*!< Power Voltage Detector Enable */

#define  PWR_CR_PLS                          ((uint16_t)0x00E0)     /*!< PLS[2:0] bits (PVD Level Selection) */
#define  PWR_CR_PLS_0                        ((uint16_t)0x0020)     /*!< Bit 0 */
#define  PWR_CR_PLS_1                        ((uint16_t)0x0040)     /*!< Bit 1 */
#define  PWR_CR_PLS_2                        ((uint16_t)0x0080)     /*!< Bit 2 */

/*!< PVD level configuration */
#define  PWR_CR_PLS_LEV0                     ((uint16_t)0x0000)     /*!< PVD level 0 */
#define  PWR_CR_PLS_LEV1                     ((uint16_t)0x0020)     /*!< PVD level 1 */
#define  PWR_CR_PLS_LEV2                     ((uint16_t)0x0040)     /*!< PVD level 2 */
#define  PWR_CR_PLS_LEV3                     ((uint16_t)0x0060)     /*!< PVD level 3 */
#define  PWR_CR_PLS_LEV4                     ((uint16_t)0x0080)     /*!< PVD level 4 */
#define  PWR_CR_PLS_LEV5                     ((uint16_t)0x00A0)     /*!< PVD level 5 */
#define  PWR_CR_PLS_LEV6                     ((uint16_t)0x00C0)     /*!< PVD level 6 */
#define  PWR_CR_PLS_LEV7                     ((uint16_t)0x00E0)     /*!< PVD level 7 */

#define  PWR_CR_DBP                          ((uint16_t)0x0100)     /*!< Disable Backup Domain write protection */
#define  PWR_CR_ULP                          ((uint16_t)0x0200)     /*!< Ultra Low Power mode */
#define  PWR_CR_FWU                          ((uint16_t)0x0400)     /*!< Fast wakeup */

#define  PWR_CR_VOS                          ((uint16_t)0x1800)     /*!< VOS[1:0] bits (Voltage scaling range selection) */
#define  PWR_CR_VOS_0                        ((uint16_t)0x0800)     /*!< Bit 0 */
#define  PWR_CR_VOS_1                        ((uint16_t)0x1000)     /*!< Bit 1 */
#define  PWR_CR_LPRUN                        ((uint16_t)0x4000)     /*!< Low power run mode */

/*******************  Bit definition for PWR_CSR register  ********************/
#define  PWR_CSR_WUF                         ((uint16_t)0x0001)     /*!< Wakeup Flag */
#define  PWR_CSR_SBF                         ((uint16_t)0x0002)     /*!< Standby Flag */
#define  PWR_CSR_PVDO                        ((uint16_t)0x0004)     /*!< PVD Output */
#define  PWR_CSR_VREFINTRDYF                 ((uint16_t)0x0008)     /*!< Internal voltage reference (VREFINT) ready flag */
#define  PWR_CSR_VOSF                        ((uint16_t)0x0010)     /*!< Voltage Scaling select flag */
#define  PWR_CSR_REGLPF                      ((uint16_t)0x0020)     /*!< Regulator LP flag */

#define  PWR_CSR_EWUP1                       ((uint16_t)0x0100)     /*!< Enable WKUP pin 1 */
#define  PWR_CSR_EWUP2                       ((uint16_t)0x0200)     /*!< Enable WKUP pin 2 */
#define  PWR_CSR_EWUP3                       ((uint16_t)0x0400)     /*!< Enable WKUP pin 3 */


#else
	#define PWR_CR_DBP	0x00000100
#endif

#endif /* STM32_PWR_H */

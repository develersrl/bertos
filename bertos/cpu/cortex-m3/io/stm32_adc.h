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
 * \brief STM32F103xx adc definition.
 */

#ifndef STM32_ADC_H
#define STM32_ADC_H

#include <cpu/types.h>

/* ADC dual mode */
#define ADC_MODE_INDEPENDENT                       ((uint32_t)0x00000000)
#define ADC_MODE_REGINJECSIMULT                    ((uint32_t)0x00010000)
#define ADC_MODE_REGSIMULT_ALTERTRIG               ((uint32_t)0x00020000)
#define ADC_MODE_INJECSIMULT_FASTINTERL            ((uint32_t)0x00030000)
#define ADC_MODE_INJECSIMULT_SLOWINTERL            ((uint32_t)0x00040000)
#define ADC_MODE_INJECSIMULT                       ((uint32_t)0x00050000)
#define ADC_MODE_REGSIMULT                         ((uint32_t)0x00060000)
#define ADC_MODE_FASTINTERL                        ((uint32_t)0x00070000)
#define ADC_MODE_SLOWINTERL                        ((uint32_t)0x00080000)
#define ADC_MODE_ALTERTRIG                         ((uint32_t)0x00090000)

/* ADC extrenal trigger sources for regular channels conversion */
#define ADC_EXTERNALTRIGCONV_T1_CC1                ((uint32_t)0x00000000)
#define ADC_EXTERNALTRIGCONV_T1_CC2                ((uint32_t)0x00020000)
#define ADC_EXTERNALTRIGCONV_T1_CC3                ((uint32_t)0x00040000)
#define ADC_EXTERNALTRIGCONV_T2_CC2                ((uint32_t)0x00060000)
#define ADC_EXTERNALTRIGCONV_T3_TRGO               ((uint32_t)0x00080000)
#define ADC_EXTERNALTRIGCONV_T4_CC4                ((uint32_t)0x000A0000)
#define ADC_EXTERNALTRIGCONV_EXT_IT11              ((uint32_t)0x000C0000)
#define ADC_EXTERNALTRIGCONV_NONE                  ((uint32_t)0x000E0000)

/* ADC data align */
#define ADC_DATAALIGN_RIGHT                        ((uint32_t)0x00000000)
#define ADC_DATAALIGN_LEFT                         ((uint32_t)0x00000800)

/* ADC channels */
#define ADC_CHANNEL_0                               ((uint8_t)0x00)
#define ADC_CHANNEL_1                               ((uint8_t)0x01)
#define ADC_CHANNEL_2                               ((uint8_t)0x02)
#define ADC_CHANNEL_3                               ((uint8_t)0x03)
#define ADC_CHANNEL_4                               ((uint8_t)0x04)
#define ADC_CHANNEL_5                               ((uint8_t)0x05)
#define ADC_CHANNEL_6                               ((uint8_t)0x06)
#define ADC_CHANNEL_7                               ((uint8_t)0x07)
#define ADC_CHANNEL_8                               ((uint8_t)0x08)
#define ADC_CHANNEL_9                               ((uint8_t)0x09)
#define ADC_CHANNEL_10                              ((uint8_t)0x0A)
#define ADC_CHANNEL_11                              ((uint8_t)0x0B)
#define ADC_CHANNEL_12                              ((uint8_t)0x0C)
#define ADC_CHANNEL_13                              ((uint8_t)0x0D)
#define ADC_CHANNEL_14                              ((uint8_t)0x0E)
#define ADC_CHANNEL_15                              ((uint8_t)0x0F)
#define ADC_CHANNEL_16                              ((uint8_t)0X10)
#define ADC_CHANNEL_17                              ((uint8_t)0X11)

/* ADC sampling times */
#define ADC_SAMPLETIME_1CYCLES5                    ((uint8_t)0x00)
#define ADC_SAMPLETIME_7CYCLES5                    ((uint8_t)0x01)
#define ADC_SAMPLETIME_13CYCLES5                   ((uint8_t)0x02)
#define ADC_SAMPLETIME_28CYCLES5                   ((uint8_t)0x03)
#define ADC_SAMPLETIME_41CYCLES5                   ((uint8_t)0x04)
#define ADC_SAMPLETIME_55CYCLES5                   ((uint8_t)0x05)
#define ADC_SAMPLETIME_71CYCLES5                   ((uint8_t)0x06)
#define ADC_SAMPLETIME_239CYCLES5                  ((uint8_t)0x07)

/* ADC extrenal trigger sources for injected channels conversion */
#define ADC_EXTERNALTRIGINJECCONV_T1_TRGO          ((uint32_t)0x00000000)
#define ADC_EXTERNALTRIGINJECCONV_T1_CC4           ((uint32_t)0x00001000)
#define ADC_EXTERNALTRIGINJECCONV_T2_TRGO          ((uint32_t)0x00002000)
#define ADC_EXTERNALTRIGINJECCONV_T2_CC1           ((uint32_t)0x00003000)
#define ADC_EXTERNALTRIGINJECCONV_T3_CC4           ((uint32_t)0x00004000)
#define ADC_EXTERNALTRIGINJECCONV_T4_TRGO          ((uint32_t)0x00005000)
#define ADC_EXTERNALTRIGINJECCONV_EXT_IT15         ((uint32_t)0x00006000)
#define ADC_EXTERNALTRIGINJECCONV_NONE             ((uint32_t)0x00007000)

/* ADC injected channel selection */
#define ADC_INJECTEDCHANNEL_1                       ((uint8_t)0x14)
#define ADC_INJECTEDCHANNEL_2                       ((uint8_t)0x18)
#define ADC_INJECTEDCHANNEL_3                       ((uint8_t)0x1C)
#define ADC_INJECTEDCHANNEL_4                       ((uint8_t)0x20)

/* ADC analog watchdog selection */
#define ADC_ANALOGWATCHDOG_SINGLEREGENABLE         ((uint32_t)0x00800200)
#define ADC_ANALOGWATCHDOG_SINGLEINJECENABLE       ((uint32_t)0x00400200)
#define ADC_ANALOGWATCHDOG_SINGLEREGORINJECENABLE  ((uint32_t)0x00C00200)
#define ADC_ANALOGWATCHDOG_ALLREGENABLE            ((uint32_t)0x00800000)
#define ADC_ANALOGWATCHDOG_ALLINJECENABLE          ((uint32_t)0x00400000)
#define ADC_ANALOGWATCHDOG_ALLREGALLINJECENABLE    ((uint32_t)0x00C00000)
#define ADC_ANALOGWATCHDOG_NONE                    ((uint32_t)0x00000000)

/* ADC interrupts definition */
#define ADC_IT_EOC                                 ((uint16_t)0x0220)
#define ADC_IT_AWD                                 ((uint16_t)0x0140)
#define ADC_IT_JEOC                                ((uint16_t)0x0480)

/* ADC flags definition */
#define ADC_FLAG_AWD                               ((uint8_t)0x01)
#define ADC_FLAG_EOC                               ((uint8_t)0x02)
#define ADC_FLAG_JEOC                              ((uint8_t)0x04)
#define ADC_FLAG_JSTRT                             ((uint8_t)0x08)
#define ADC_FLAG_STRT                              ((uint8_t)0X10)

#endif /* STM32_ADC_H */

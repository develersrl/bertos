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
 * \brief STM32F2xx timer definition.
 */

#ifndef STM32_TIMER_H
#define STM32_TIMER_H

#include <cpu/detect.h>

#if CPU_CM3_STM32F1
	#warning __FILTER_NEXT_WARNING__
	#warning Not supported
#elif CPU_CM3_STM32F2

typedef struct stm32_tim_half
{
	reg16_t VAL;
	uint16_t RESERVED;
} stm32_tim_half;

struct stm32_tim
{
	reg16_t CR1;          /*!< TIM control register 1,              Address offset: 0x00 */
	uint16_t RESERVED0;   /*!< Reserved, 0x02                                            */
	reg16_t CR2;          /*!< TIM control register 2,              Address offset: 0x04 */
	uint16_t RESERVED1;   /*!< Reserved, 0x06                                            */
	reg16_t SMCR;         /*!< TIM slave mode control register,     Address offset: 0x08 */
	uint16_t RESERVED2;   /*!< Reserved, 0x0A                                            */
	reg16_t DIER;         /*!< TIM DMA/interrupt enable register,   Address offset: 0x0C */
	uint16_t RESERVED3;   /*!< Reserved, 0x0E                                            */
	reg16_t SR;           /*!< TIM status register,                 Address offset: 0x10 */
	uint16_t RESERVED4;   /*!< Reserved, 0x12                                            */
	reg16_t EGR;          /*!< TIM event generation register,       Address offset: 0x14 */
	uint16_t RESERVED5;   /*!< Reserved, 0x16                                            */
	/*!<                       TIM capture/compare mode registers,  Address offset: 0x18 */
	stm32_tim_half CCMR[2];
	reg16_t CCER;         /*!< TIM capture/compare enable register, Address offset: 0x20 */
	uint16_t RESERVED8;   /*!< Reserved, 0x22                                            */
	reg32_t CNT;          /*!< TIM counter register,                Address offset: 0x24 */
	reg16_t PSC;          /*!< TIM prescaler,                       Address offset: 0x28 */
	uint16_t RESERVED9;   /*!< Reserved, 0x2A                                            */
	reg32_t ARR;          /*!< TIM auto-reload register,            Address offset: 0x2C */
	reg16_t RCR;          /*!< TIM repetition counter register,     Address offset: 0x30 */
	uint16_t RESERVED10;  /*!< Reserved, 0x32                                            */
	/*!<                       TIM capture/compare registers,       Address offset: 0x34 */
	reg32_t CCR[4];
	reg16_t BDTR;         /*!< TIM break and dead-time register,    Address offset: 0x44 */
	uint16_t RESERVED11;  /*!< Reserved, 0x46                                            */
	reg16_t DCR;          /*!< TIM DMA control register,            Address offset: 0x48 */
	uint16_t RESERVED12;  /*!< Reserved, 0x4A                                            */
	reg16_t DMAR;         /*!< TIM DMA address for full transfer,   Address offset: 0x4C */
	uint16_t RESERVED13;  /*!< Reserved, 0x4E                                            */
	reg16_t OR;           /*!< TIM option register,                 Address offset: 0x50 */
	uint16_t RESERVED14;  /*!< Reserved, 0x52                                            */
};

#define TIM1  ((struct stm32_tim *)TIM1_BASE)
#define TIM2  ((struct stm32_tim *)TIM2_BASE)
#define TIM3  ((struct stm32_tim *)TIM3_BASE)
#define TIM4  ((struct stm32_tim *)TIM4_BASE)
#define TIM5  ((struct stm32_tim *)TIM5_BASE)
#define TIM6  ((struct stm32_tim *)TIM6_BASE)
#define TIM7  ((struct stm32_tim *)TIM7_BASE)
#define TIM8  ((struct stm32_tim *)TIM8_BASE)
#define TIM9  ((struct stm32_tim *)TIM9_BASE)
#define TIM10 ((struct stm32_tim *)TIM10_BASE)
#define TIM11 ((struct stm32_tim *)TIM11_BASE)
#define TIM12 ((struct stm32_tim *)TIM12_BASE)
#define TIM13 ((struct stm32_tim *)TIM13_BASE)
#define TIM14 ((struct stm32_tim *)TIM14_BASE)

/* Bit definition for TIM_CR1 register */
#define TIM_CR1_CEN                         ((uint16_t)0x0001)            /*!<Counter enable */
#define TIM_CR1_UDIS                        ((uint16_t)0x0002)            /*!<Update disable */
#define TIM_CR1_URS                         ((uint16_t)0x0004)            /*!<Update request source */
#define TIM_CR1_OPM                         ((uint16_t)0x0008)            /*!<One pulse mode */
#define TIM_CR1_DIR                         ((uint16_t)0x0010)            /*!<Direction */

#define TIM_CR1_CMS                         ((uint16_t)0x0060)            /*!<CMS[1:0] bits (Center-aligned mode selection) */
#define TIM_CR1_CMS_0                       ((uint16_t)0x0020)            /*!<Bit 0 */
#define TIM_CR1_CMS_1                       ((uint16_t)0x0040)            /*!<Bit 1 */

#define TIM_CR1_ARPE                        ((uint16_t)0x0080)            /*!<Auto-reload preload enable */

#define TIM_CR1_CKD                         ((uint16_t)0x0300)            /*!<CKD[1:0] bits (clock division) */
#define TIM_CR1_CKD_0                       ((uint16_t)0x0100)            /*!<Bit 0 */
#define TIM_CR1_CKD_1                       ((uint16_t)0x0200)            /*!<Bit 1 */

/* Bit definition for TIM_CR2 register */
#define TIM_CR2_CCPC                        ((uint16_t)0x0001)            /*!<Capture/Compare Preloaded Control */
#define TIM_CR2_CCUS                        ((uint16_t)0x0004)            /*!<Capture/Compare Control Update Selection */
#define TIM_CR2_CCDS                        ((uint16_t)0x0008)            /*!<Capture/Compare DMA Selection */

#define TIM_CR2_MMS                         ((uint16_t)0x0070)            /*!<MMS[2:0] bits (Master Mode Selection) */
#define TIM_CR2_MMS_0                       ((uint16_t)0x0010)            /*!<Bit 0 */
#define TIM_CR2_MMS_1                       ((uint16_t)0x0020)            /*!<Bit 1 */
#define TIM_CR2_MMS_2                       ((uint16_t)0x0040)            /*!<Bit 2 */

#define TIM_CR2_TI1S                        ((uint16_t)0x0080)            /*!<TI1 Selection */
#define TIM_CR2_OIS1                        ((uint16_t)0x0100)            /*!<Output Idle state 1 (OC1 output) */
#define TIM_CR2_OIS1N                       ((uint16_t)0x0200)            /*!<Output Idle state 1 (OC1N output) */
#define TIM_CR2_OIS2                        ((uint16_t)0x0400)            /*!<Output Idle state 2 (OC2 output) */
#define TIM_CR2_OIS2N                       ((uint16_t)0x0800)            /*!<Output Idle state 2 (OC2N output) */
#define TIM_CR2_OIS3                        ((uint16_t)0x1000)            /*!<Output Idle state 3 (OC3 output) */
#define TIM_CR2_OIS3N                       ((uint16_t)0x2000)            /*!<Output Idle state 3 (OC3N output) */
#define TIM_CR2_OIS4                        ((uint16_t)0x4000)            /*!<Output Idle state 4 (OC4 output) */

/* Bit definition for TIM_SMCR register */
#define TIM_SMCR_SMS                        ((uint16_t)0x0007)            /*!<SMS[2:0] bits (Slave mode selection) */
#define TIM_SMCR_SMS_0                      ((uint16_t)0x0001)            /*!<Bit 0 */
#define TIM_SMCR_SMS_1                      ((uint16_t)0x0002)            /*!<Bit 1 */
#define TIM_SMCR_SMS_2                      ((uint16_t)0x0004)            /*!<Bit 2 */

#define TIM_SMCR_TS                         ((uint16_t)0x0070)            /*!<TS[2:0] bits (Trigger selection) */
#define TIM_SMCR_TS_0                       ((uint16_t)0x0010)            /*!<Bit 0 */
#define TIM_SMCR_TS_1                       ((uint16_t)0x0020)            /*!<Bit 1 */
#define TIM_SMCR_TS_2                       ((uint16_t)0x0040)            /*!<Bit 2 */

#define TIM_SMCR_MSM                        ((uint16_t)0x0080)            /*!<Master/slave mode */

#define TIM_SMCR_ETF                        ((uint16_t)0x0F00)            /*!<ETF[3:0] bits (External trigger filter) */
#define TIM_SMCR_ETF_0                      ((uint16_t)0x0100)            /*!<Bit 0 */
#define TIM_SMCR_ETF_1                      ((uint16_t)0x0200)            /*!<Bit 1 */
#define TIM_SMCR_ETF_2                      ((uint16_t)0x0400)            /*!<Bit 2 */
#define TIM_SMCR_ETF_3                      ((uint16_t)0x0800)            /*!<Bit 3 */

#define TIM_SMCR_ETPS                       ((uint16_t)0x3000)            /*!<ETPS[1:0] bits (External trigger prescaler) */
#define TIM_SMCR_ETPS_0                     ((uint16_t)0x1000)            /*!<Bit 0 */
#define TIM_SMCR_ETPS_1                     ((uint16_t)0x2000)            /*!<Bit 1 */

#define TIM_SMCR_ECE                        ((uint16_t)0x4000)            /*!<External clock enable */
#define TIM_SMCR_ETP                        ((uint16_t)0x8000)            /*!<External trigger polarity */

/* Bit definition for TIM_DIER register */
#define TIM_DIER_UIE                        ((uint16_t)0x0001)            /*!<Update interrupt enable */
#define TIM_DIER_CC1IEN                     ((uint16_t)0x0002)            /*!<Capture/Compare 1 interrupt enable */
#define TIM_DIER_CC2IEN                     ((uint16_t)0x0004)            /*!<Capture/Compare 2 interrupt enable */
#define TIM_DIER_CC3IEN                     ((uint16_t)0x0008)            /*!<Capture/Compare 3 interrupt enable */
#define TIM_DIER_CC4IEN                     ((uint16_t)0x0010)            /*!<Capture/Compare 4 interrupt enable */
#define TIM_DIER_COMIEN                     ((uint16_t)0x0020)            /*!<COM interrupt enable */
#define TIM_DIER_TIE                        ((uint16_t)0x0040)            /*!<Trigger interrupt enable */
#define TIM_DIER_BIE                        ((uint16_t)0x0080)            /*!<Break interrupt enable */
#define TIM_DIER_UDE                        ((uint16_t)0x0100)            /*!<Update DMA request enable */
#define TIM_DIER_CC1DEN                     ((uint16_t)0x0200)            /*!<Capture/Compare 1 DMA request enable */
#define TIM_DIER_CC2DEN                     ((uint16_t)0x0400)            /*!<Capture/Compare 2 DMA request enable */
#define TIM_DIER_CC3DEN                     ((uint16_t)0x0800)            /*!<Capture/Compare 3 DMA request enable */
#define TIM_DIER_CC4DEN                     ((uint16_t)0x1000)            /*!<Capture/Compare 4 DMA request enable */
#define TIM_DIER_COMDEN                     ((uint16_t)0x2000)            /*!<COM DMA request enable */
#define TIM_DIER_TDE                        ((uint16_t)0x4000)            /*!<Trigger DMA request enable */

/* Bit definition for TIM_SR register */
#define TIM_SR_UIF                          ((uint16_t)0x0001)            /*!<Update interrupt Flag */
#define TIM_SR_CC1IF                        ((uint16_t)0x0002)            /*!<Capture/Compare 1 interrupt Flag */
#define TIM_SR_CC2IF                        ((uint16_t)0x0004)            /*!<Capture/Compare 2 interrupt Flag */
#define TIM_SR_CC3IF                        ((uint16_t)0x0008)            /*!<Capture/Compare 3 interrupt Flag */
#define TIM_SR_CC4IF                        ((uint16_t)0x0010)            /*!<Capture/Compare 4 interrupt Flag */
#define TIM_SR_COMIF                        ((uint16_t)0x0020)            /*!<COM interrupt Flag */
#define TIM_SR_TIF                          ((uint16_t)0x0040)            /*!<Trigger interrupt Flag */
#define TIM_SR_BIF                          ((uint16_t)0x0080)            /*!<Break interrupt Flag */
#define TIM_SR_CC1OF                        ((uint16_t)0x0200)            /*!<Capture/Compare 1 Overcapture Flag */
#define TIM_SR_CC2OF                        ((uint16_t)0x0400)            /*!<Capture/Compare 2 Overcapture Flag */
#define TIM_SR_CC3OF                        ((uint16_t)0x0800)            /*!<Capture/Compare 3 Overcapture Flag */
#define TIM_SR_CC4OF                        ((uint16_t)0x1000)            /*!<Capture/Compare 4 Overcapture Flag */

/* Bit definition for TIM_EGR register */
#define TIM_EGR_UG                          ((uint8_t)0x01)               /*!<Update Generation */
#define TIM_EGR_CC1G                        ((uint8_t)0x02)               /*!<Capture/Compare 1 Generation */
#define TIM_EGR_CC2G                        ((uint8_t)0x04)               /*!<Capture/Compare 2 Generation */
#define TIM_EGR_CC3G                        ((uint8_t)0x08)               /*!<Capture/Compare 3 Generation */
#define TIM_EGR_CC4G                        ((uint8_t)0x10)               /*!<Capture/Compare 4 Generation */
#define TIM_EGR_COMG                        ((uint8_t)0x20)               /*!<Capture/Compare Control Update Generation */
#define TIM_EGR_TG                          ((uint8_t)0x40)               /*!<Trigger Generation */
#define TIM_EGR_BG                          ((uint8_t)0x80)               /*!<Break Generation */

/* Bit definition for TIM_CCMR1 register */
#define TIM_CCMR1_CC1S_OUT                  ((uint16_t)0 << 0)
#define TIM_CCMR1_CC1S_TI1                  ((uint16_t)1 << 0)
#define TIM_CCMR1_CC1S_TI2                  ((uint16_t)2 << 0)
#define TIM_CCMR1_CC1S_TRC                  ((uint16_t)3 << 0)

#define TIM_CCMR1_CC2S_OUT                  ((uint16_t)0 << 8)
#define TIM_CCMR1_CC2S_TI1                  ((uint16_t)1 << 8)
#define TIM_CCMR1_CC2S_TI2                  ((uint16_t)2 << 8)
#define TIM_CCMR1_CC2S_TRC                  ((uint16_t)3 << 8)

#define TIM_CCMR1_CC1S                      ((uint16_t)0x0003)            /*!<CC1S[1:0] bits (Capture/Compare 1 Selection) */
#define TIM_CCMR1_CC1S_0                    ((uint16_t)0x0001)            /*!<Bit 0 */
#define TIM_CCMR1_CC1S_1                    ((uint16_t)0x0002)            /*!<Bit 1 */

#define TIM_CCMR1_OC1FE                     ((uint16_t)0x0004)            /*!<Output Compare 1 Fast enable */
#define TIM_CCMR1_OC1PE                     ((uint16_t)0x0008)            /*!<Output Compare 1 Preload enable */

#define TIM_CCMR1_OC1M                      ((uint16_t)0x0070)            /*!<OC1M[2:0] bits (Output Compare 1 Mode) */
#define TIM_CCMR1_OC1M_0                    ((uint16_t)0x0010)            /*!<Bit 0 */
#define TIM_CCMR1_OC1M_1                    ((uint16_t)0x0020)            /*!<Bit 1 */
#define TIM_CCMR1_OC1M_2                    ((uint16_t)0x0040)            /*!<Bit 2 */

#define TIM_CCMR1_OC1CE                     ((uint16_t)0x0080)            /*!<Output Compare 1Clear Enable */

#define TIM_CCMR1_CC2S                      ((uint16_t)0x0300)            /*!<CC2S[1:0] bits (Capture/Compare 2 Selection) */
#define TIM_CCMR1_CC2S_0                    ((uint16_t)0x0100)            /*!<Bit 0 */
#define TIM_CCMR1_CC2S_1                    ((uint16_t)0x0200)            /*!<Bit 1 */

#define TIM_CCMR1_OC2FE                     ((uint16_t)0x0400)            /*!<Output Compare 2 Fast enable */
#define TIM_CCMR1_OC2PE                     ((uint16_t)0x0800)            /*!<Output Compare 2 Preload enable */

#define TIM_CCMR1_OC2M                      ((uint16_t)0x7000)            /*!<OC2M[2:0] bits (Output Compare 2 Mode) */
#define TIM_CCMR1_OC2M_0                    ((uint16_t)0x1000)            /*!<Bit 0 */
#define TIM_CCMR1_OC2M_1                    ((uint16_t)0x2000)            /*!<Bit 1 */
#define TIM_CCMR1_OC2M_2                    ((uint16_t)0x4000)            /*!<Bit 2 */

#define TIM_CCMR1_OC2CE                     ((uint16_t)0x8000)            /*!<Output Compare 2 Clear Enable */

#define TIM_CCMR1_IC1PSC                    ((uint16_t)0x000C)            /*!<IC1PSC[1:0] bits (Input Capture 1 Prescaler) */
#define TIM_CCMR1_IC1PSC_0                  ((uint16_t)0x0004)            /*!<Bit 0 */
#define TIM_CCMR1_IC1PSC_1                  ((uint16_t)0x0008)            /*!<Bit 1 */

#define TIM_CCMR1_IC1F                      ((uint16_t)0x00F0)            /*!<IC1F[3:0] bits (Input Capture 1 Filter) */
#define TIM_CCMR1_IC1F_0                    ((uint16_t)0x0010)            /*!<Bit 0 */
#define TIM_CCMR1_IC1F_1                    ((uint16_t)0x0020)            /*!<Bit 1 */
#define TIM_CCMR1_IC1F_2                    ((uint16_t)0x0040)            /*!<Bit 2 */
#define TIM_CCMR1_IC1F_3                    ((uint16_t)0x0080)            /*!<Bit 3 */

#define TIM_CCMR1_IC2PSC                    ((uint16_t)0x0C00)            /*!<IC2PSC[1:0] bits (Input Capture 2 Prescaler) */
#define TIM_CCMR1_IC2PSC_0                  ((uint16_t)0x0400)            /*!<Bit 0 */
#define TIM_CCMR1_IC2PSC_1                  ((uint16_t)0x0800)            /*!<Bit 1 */

#define TIM_CCMR1_IC2F                      ((uint16_t)0xF000)            /*!<IC2F[3:0] bits (Input Capture 2 Filter) */
#define TIM_CCMR1_IC2F_0                    ((uint16_t)0x1000)            /*!<Bit 0 */
#define TIM_CCMR1_IC2F_1                    ((uint16_t)0x2000)            /*!<Bit 1 */
#define TIM_CCMR1_IC2F_2                    ((uint16_t)0x4000)            /*!<Bit 2 */
#define TIM_CCMR1_IC2F_3                    ((uint16_t)0x8000)            /*!<Bit 3 */

/* Bit definition for TIM_CCMR2 register */
#define TIM_CCMR2_CC3S_OUT                  ((uint16_t)0 << 0)
#define TIM_CCMR2_CC3S_TI3                  ((uint16_t)1 << 0)
#define TIM_CCMR2_CC3S_TI4                  ((uint16_t)2 << 0)
#define TIM_CCMR2_CC3S_TRC                  ((uint16_t)3 << 0)

#define TIM_CCMR2_CC4S_OUT                  ((uint16_t)0 << 8)
#define TIM_CCMR2_CC4S_TI3                  ((uint16_t)1 << 8)
#define TIM_CCMR2_CC4S_TI4                  ((uint16_t)2 << 8)
#define TIM_CCMR2_CC4S_TRC                  ((uint16_t)3 << 8)

#define TIM_CCMR2_OC4M_PWM1                 ((uint16_t)6 << 12)

#define TIM_CCMR_CCS_OUT                    ((uint16_t)0)
#define TIM_CCMR_CCS_TI1                    ((uint16_t)1)
#define TIM_CCMR_CCS_TI2                    ((uint16_t)2)
#define TIM_CCMR_CCS_TRC                    ((uint16_t)3)

#define TIM_CCMR_OCM_FROZEN                 ((uint16_t)0 << 4)
#define TIM_CCMR_OCM_HIGH_MATCH             ((uint16_t)1 << 4)
#define TIM_CCMR_OCM_LOW_MATCH              ((uint16_t)2 << 4)
#define TIM_CCMR_OCM_TOGGLE                 ((uint16_t)3 << 4)
#define TIM_CCMR_OCM_LOW                    ((uint16_t)4 << 4)
#define TIM_CCMR_OCM_HIGH                   ((uint16_t)5 << 4)
#define TIM_CCMR_OCM_PWM1                   ((uint16_t)6 << 4)
#define TIM_CCMR_OCM_PWM2                   ((uint16_t)7 << 4)

#define TIM_CCMR_OCPE                       ((uint16_t)0x0008)            /*!<Output Compare Preload enable */

#define TIM_CCMR2_CC3S                      ((uint16_t)0x0003)            /*!<CC3S[1:0] bits (Capture/Compare 3 Selection) */
#define TIM_CCMR2_CC3S_0                    ((uint16_t)0x0001)            /*!<Bit 0 */
#define TIM_CCMR2_CC3S_1                    ((uint16_t)0x0002)            /*!<Bit 1 */

#define TIM_CCMR2_OC3FE                     ((uint16_t)0x0004)            /*!<Output Compare 3 Fast enable */
#define TIM_CCMR2_OC3PE                     ((uint16_t)0x0008)            /*!<Output Compare 3 Preload enable */

#define TIM_CCMR2_OC3M                      ((uint16_t)0x0070)            /*!<OC3M[2:0] bits (Output Compare 3 Mode) */
#define TIM_CCMR2_OC3M_0                    ((uint16_t)0x0010)            /*!<Bit 0 */
#define TIM_CCMR2_OC3M_1                    ((uint16_t)0x0020)            /*!<Bit 1 */
#define TIM_CCMR2_OC3M_2                    ((uint16_t)0x0040)            /*!<Bit 2 */

#define TIM_CCMR2_OC3CE                     ((uint16_t)0x0080)            /*!<Output Compare 3 Clear Enable */

#define TIM_CCMR2_CC4S                      ((uint16_t)0x0300)            /*!<CC4S[1:0] bits (Capture/Compare 4 Selection) */
#define TIM_CCMR2_CC4S_0                    ((uint16_t)0x0100)            /*!<Bit 0 */
#define TIM_CCMR2_CC4S_1                    ((uint16_t)0x0200)            /*!<Bit 1 */

#define TIM_CCMR2_OC4FE                     ((uint16_t)0x0400)            /*!<Output Compare 4 Fast enable */
#define TIM_CCMR2_OC4PE                     ((uint16_t)0x0800)            /*!<Output Compare 4 Preload enable */

#define TIM_CCMR2_OC4M                      ((uint16_t)0x7000)            /*!<OC4M[2:0] bits (Output Compare 4 Mode) */
#define TIM_CCMR2_OC4M_0                    ((uint16_t)0x1000)            /*!<Bit 0 */
#define TIM_CCMR2_OC4M_1                    ((uint16_t)0x2000)            /*!<Bit 1 */
#define TIM_CCMR2_OC4M_2                    ((uint16_t)0x4000)            /*!<Bit 2 */

#define TIM_CCMR2_OC4CE                     ((uint16_t)0x8000)            /*!<Output Compare 4 Clear Enable */

#define TIM_CCMR2_IC3PSC                    ((uint16_t)0x000C)            /*!<IC3PSC[1:0] bits (Input Capture 3 Prescaler) */
#define TIM_CCMR2_IC3PSC_0                  ((uint16_t)0x0004)            /*!<Bit 0 */
#define TIM_CCMR2_IC3PSC_1                  ((uint16_t)0x0008)            /*!<Bit 1 */

#define TIM_CCMR2_IC3F                      ((uint16_t)0x00F0)            /*!<IC3F[3:0] bits (Input Capture 3 Filter) */
#define TIM_CCMR2_IC3F_0                    ((uint16_t)0x0010)            /*!<Bit 0 */
#define TIM_CCMR2_IC3F_1                    ((uint16_t)0x0020)            /*!<Bit 1 */
#define TIM_CCMR2_IC3F_2                    ((uint16_t)0x0040)            /*!<Bit 2 */
#define TIM_CCMR2_IC3F_3                    ((uint16_t)0x0080)            /*!<Bit 3 */

#define TIM_CCMR2_IC4PSC                    ((uint16_t)0x0C00)            /*!<IC4PSC[1:0] bits (Input Capture 4 Prescaler) */
#define TIM_CCMR2_IC4PSC_0                  ((uint16_t)0x0400)            /*!<Bit 0 */
#define TIM_CCMR2_IC4PSC_1                  ((uint16_t)0x0800)            /*!<Bit 1 */

#define TIM_CCMR2_IC4F                      ((uint16_t)0xF000)            /*!<IC4F[3:0] bits (Input Capture 4 Filter) */
#define TIM_CCMR2_IC4F_0                    ((uint16_t)0x1000)            /*!<Bit 0 */
#define TIM_CCMR2_IC4F_1                    ((uint16_t)0x2000)            /*!<Bit 1 */
#define TIM_CCMR2_IC4F_2                    ((uint16_t)0x4000)            /*!<Bit 2 */
#define TIM_CCMR2_IC4F_3                    ((uint16_t)0x8000)            /*!<Bit 3 */

/* Bit definition for TIM_CCER register */
#define TIM_CCER_CC1_RISING                 ((uint16_t)0x0000)
#define TIM_CCER_CC1_FALLING                ((uint16_t)0x0002)
#define TIM_CCER_CC1_BOTH                   ((uint16_t)0x0006)

#define TIM_CCER_CCEN                       ((uint16_t)0x0001)            /*!<Capture/Compare output enable */

#define TIM_CCER_CC1EN                      ((uint16_t)0x0001)            /*!<Capture/Compare 1 output enable */
#define TIM_CCER_CC1P                       ((uint16_t)0x0002)            /*!<Capture/Compare 1 output Polarity */
#define TIM_CCER_CC1NE                      ((uint16_t)0x0004)            /*!<Capture/Compare 1 Complementary output enable */
#define TIM_CCER_CC1NP                      ((uint16_t)0x0008)            /*!<Capture/Compare 1 Complementary output Polarity */
#define TIM_CCER_CC2EN                      ((uint16_t)0x0010)            /*!<Capture/Compare 2 output enable */
#define TIM_CCER_CC2P                       ((uint16_t)0x0020)            /*!<Capture/Compare 2 output Polarity */
#define TIM_CCER_CC2NE                      ((uint16_t)0x0040)            /*!<Capture/Compare 2 Complementary output enable */
#define TIM_CCER_CC2NP                      ((uint16_t)0x0080)            /*!<Capture/Compare 2 Complementary output Polarity */
#define TIM_CCER_CC3EN                      ((uint16_t)0x0100)            /*!<Capture/Compare 3 output enable */
#define TIM_CCER_CC3P                       ((uint16_t)0x0200)            /*!<Capture/Compare 3 output Polarity */
#define TIM_CCER_CC3NE                      ((uint16_t)0x0400)            /*!<Capture/Compare 3 Complementary output enable */
#define TIM_CCER_CC3NP                      ((uint16_t)0x0800)            /*!<Capture/Compare 3 Complementary output Polarity */
#define TIM_CCER_CC4EN                      ((uint16_t)0x1000)            /*!<Capture/Compare 4 output enable */
#define TIM_CCER_CC4P                       ((uint16_t)0x2000)            /*!<Capture/Compare 4 output Polarity */
#define TIM_CCER_CC4NP                      ((uint16_t)0x8000)            /*!<Capture/Compare 4 Complementary output Polarity */

/* Bit definition for TIM_CNT register */
#define TIM_CNT_CNT                         ((uint16_t)0xFFFF)            /*!<Counter Value */

/* Bit definition for TIM_PSC register */
#define TIM_PSC_PSC                         ((uint16_t)0xFFFF)            /*!<Prescaler Value */

/* Bit definition for TIM_ARR register */
#define TIM_ARR_ARR                         ((uint16_t)0xFFFF)            /*!<actual auto-reload Value */

/* Bit definition for TIM_RCR register */
#define TIM_RCR_REP                         ((uint8_t)0xFF)               /*!<Repetition Counter Value */

/* Bit definition for TIM_CCR1 register */
#define TIM_CCR1_CCR1                       ((uint16_t)0xFFFF)            /*!<Capture/Compare 1 Value */

/* Bit definition for TIM_CCR2 register */
#define TIM_CCR2_CCR2                       ((uint16_t)0xFFFF)            /*!<Capture/Compare 2 Value */

/* Bit definition for TIM_CCR3 register */
#define TIM_CCR3_CCR3                       ((uint16_t)0xFFFF)            /*!<Capture/Compare 3 Value */

/* Bit definition for TIM_CCR4 register */
#define TIM_CCR4_CCR4                       ((uint16_t)0xFFFF)            /*!<Capture/Compare 4 Value */

/* Bit definition for TIM_BDTR register */
#define TIM_BDTR_DTG                        ((uint16_t)0x00FF)            /*!<DTG[0:7] bits (Dead-Time Generator set-up) */
#define TIM_BDTR_DTG_0                      ((uint16_t)0x0001)            /*!<Bit 0 */
#define TIM_BDTR_DTG_1                      ((uint16_t)0x0002)            /*!<Bit 1 */
#define TIM_BDTR_DTG_2                      ((uint16_t)0x0004)            /*!<Bit 2 */
#define TIM_BDTR_DTG_3                      ((uint16_t)0x0008)            /*!<Bit 3 */
#define TIM_BDTR_DTG_4                      ((uint16_t)0x0010)            /*!<Bit 4 */
#define TIM_BDTR_DTG_5                      ((uint16_t)0x0020)            /*!<Bit 5 */
#define TIM_BDTR_DTG_6                      ((uint16_t)0x0040)            /*!<Bit 6 */
#define TIM_BDTR_DTG_7                      ((uint16_t)0x0080)            /*!<Bit 7 */

#define TIM_BDTR_LOCK                       ((uint16_t)0x0300)            /*!<LOCK[1:0] bits (Lock Configuration) */
#define TIM_BDTR_LOCK_0                     ((uint16_t)0x0100)            /*!<Bit 0 */
#define TIM_BDTR_LOCK_1                     ((uint16_t)0x0200)            /*!<Bit 1 */

#define TIM_BDTR_OSSI                       ((uint16_t)0x0400)            /*!<Off-State Selection for Idle mode */
#define TIM_BDTR_OSSR                       ((uint16_t)0x0800)            /*!<Off-State Selection for Run mode */
#define TIM_BDTR_BKE                        ((uint16_t)0x1000)            /*!<Break enable */
#define TIM_BDTR_BKP                        ((uint16_t)0x2000)            /*!<Break Polarity */
#define TIM_BDTR_AOE                        ((uint16_t)0x4000)            /*!<Automatic Output enable */
#define TIM_BDTR_MOE                        ((uint16_t)0x8000)            /*!<Main Output enable */

/* Bit definition for TIM_DCR register */
#define TIM_DCR_DBA                         ((uint16_t)0x001F)            /*!<DBA[4:0] bits (DMA Base Address) */
#define TIM_DCR_DBA_0                       ((uint16_t)0x0001)            /*!<Bit 0 */
#define TIM_DCR_DBA_1                       ((uint16_t)0x0002)            /*!<Bit 1 */
#define TIM_DCR_DBA_2                       ((uint16_t)0x0004)            /*!<Bit 2 */
#define TIM_DCR_DBA_3                       ((uint16_t)0x0008)            /*!<Bit 3 */
#define TIM_DCR_DBA_4                       ((uint16_t)0x0010)            /*!<Bit 4 */

#define TIM_DCR_DBL                         ((uint16_t)0x1F00)            /*!<DBL[4:0] bits (DMA Burst Length) */
#define TIM_DCR_DBL_0                       ((uint16_t)0x0100)            /*!<Bit 0 */
#define TIM_DCR_DBL_1                       ((uint16_t)0x0200)            /*!<Bit 1 */
#define TIM_DCR_DBL_2                       ((uint16_t)0x0400)            /*!<Bit 2 */
#define TIM_DCR_DBL_3                       ((uint16_t)0x0800)            /*!<Bit 3 */
#define TIM_DCR_DBL_4                       ((uint16_t)0x1000)            /*!<Bit 4 */

/* Bit definition for TIM_DMAR register */
#define TIM_DMAR_DMAB                       ((uint16_t)0xFFFF)            /*!<DMA register for burst accesses */

/* Bit definition for TIM_OR register */
#define TIM_OR_TI4_RMP                       ((uint16_t)0x00C0)            /*!<TI4_RMP[1:0] bits (TIM5 Input 4 remap) */
#define TIM_OR_TI4_RMP_0                     ((uint16_t)0x0040)            /*!<Bit 0 */
#define TIM_OR_TI4_RMP_1                     ((uint16_t)0x0080)            /*!<Bit 1 */
#define TIM_OR_ITR1_RMP                      ((uint16_t)0x0C00)            /*!<ITR1_RMP[1:0] bits (TIM2 Internal trigger 1 remap) */
#define TIM_OR_ITR1_RMP_0                    ((uint16_t)0x0400)            /*!<Bit 0 */
#define TIM_OR_ITR1_RMP_1                    ((uint16_t)0x0800)            /*!<Bit 1 */

/* helpers to avoid hardcoding counter number */
#define TIM_CCER_VAL(counter, bits) ((bits) << ((counter) * 4))

#define TIM_CCMR_IDX(counter)       ((counter) > 1 ? 1 : 0)
#define TIM_CCMR_VAL(counter, bits) ((counter) & 1 ? (bits) << 8 : (bits) << 0)

#elif CPU_CM3_STM32F1
#warning Could probably use most of the code above (the only difference should be the 16 bit timer counter)
#else
#error Unknown CPU
#endif

#endif /* STM32_TIMER_H */

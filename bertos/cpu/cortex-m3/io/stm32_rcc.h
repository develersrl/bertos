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
 * \brief STM32 RCC definitions.
 */

#ifndef STM32_RCC_H
#define STM32_RCC_H

#if CPU_CM3_STM32F1

/* RCC registers bit address */
#define RCC_OFFSET                (RCC_BASE - PERIPH_BASE)

/**
 * CR Register
 */
/*\{*/
/* Alias word address of HSION bit */
#define CR_OFFSET                 (RCC_OFFSET + 0x00)
#define HSION_BITNUMBER           0x00
#define CR_HSION_BB               ((reg32_t *)(PERIPH_BB_BASE + (CR_OFFSET * 32) + (HSION_BITNUMBER * 4)))

/* Alias word address of PLLON bit */
#define PLLON_BITNUMBER           0x18
#define CR_PLLON_BB               ((reg32_t *)(PERIPH_BB_BASE + (CR_OFFSET * 32) + (PLLON_BITNUMBER * 4)))

/* Alias word address of CSSON bit */
#define CSSON_BITNUMBER           0x13
#define CR_CSSON_BB               ((reg32_t *)(PERIPH_BB_BASE + (CR_OFFSET * 32) + (CSSON_BITNUMBER * 4)))
/*\}*/

/**
 * CFGR Register
 */
/*\{*/
/* Alias word address of USBPRE bit */
#define CFGR_OFFSET               (RCC_OFFSET + 0x04)
#define USBPRE_BITNUMBER          0x16
#define CFGR_USBPRE_BB            ((reg32_t *)(PERIPH_BB_BASE + (CFGR_OFFSET * 32) + (USBPRE_BITNUMBER * 4)))
/*\}*/

/**
 * BDCR Register
 */
/*\{*/
/* Alias word address of RTCEN bit */
#define BDCR_OFFSET               (RCC_OFFSET + 0x20)
#define RTCEN_BITNUMBER           0x0F
#define BDCR_RTCEN_BB             ((reg32_t *)(PERIPH_BB_BASE + (BDCR_OFFSET * 32) + (RTCEN_BITNUMBER * 4)))

/* Alias word address of BDRST bit */
#define BDRST_BITNUMBER           0x10
#define BDCR_BDRST_BB             ((reg32_t *)(PERIPH_BB_BASE + (BDCR_OFFSET * 32) + (BDRST_BITNUMBER * 4)))
/*\}*/

/**
 * CSR Register
 */
/*\{*/
/* Alias word address of LSION bit */
#define CSR_OFFSET                (RCC_OFFSET + 0x24)
#define LSION_BITNUMBER           0x00
#define CSR_LSION_BB              ((reg32_t *)(PERIPH_BB_BASE + (CSR_OFFSET * 32) + (LSION_BITNUMBER * 4)))
/*\}*/

/**
 * RCC registers bit mask
 */
/*\{*/
/* CR register bit mask */
#define CR_HSEBYP_RESET           (0xFFFBFFFF)
#define CR_HSEBYP_SET             (0x00040000)
#define CR_HSEON_RESET            (0xFFFEFFFF)
#define CR_HSEON_SET              (0x00010000)
#define CR_HSITRIM_MASK           (0xFFFFFF07)

/* CFGR register bit mask */
#define CFGR_PLL_MASK             (0xFFC0FFFF)
#define CFGR_PLLMull_MASK         (0x003C0000)
#define CFGR_PLLSRC_MASK          (0x00010000)
#define CFGR_PLLXTPRE_MASK        (0x00020000)
#define CFGR_SWS_MASK             (0x0000000C)
#define CFGR_SW_MASK              (0xFFFFFFFC)
#define CFGR_HPRE_RESET_MASK      (0xFFFFFF0F)
#define CFGR_HPRE_SET_MASK        (0x000000F0)
#define CFGR_PPRE1_RESET_MASK     (0xFFFFF8FF)
#define CFGR_PPRE1_SET_MASK       (0x00000700)
#define CFGR_PPRE2_RESET_MASK     (0xFFFFC7FF)
#define CFGR_PPRE2_SET_MASK       (0x00003800)
#define CFGR_ADCPRE_RESET_MASK    (0xFFFF3FFF)
#define CFGR_ADCPRE_SET_MASK      (0x0000C000)

/* CSR register bit mask */
#define CSR_RVMF_SET              (0x01000000)

/* RCC Flag MASK */
#define FLAG_MASK                 (0x1F)

/* Typical VALUE of the HSI in Hz */
#define HSI_VALUE                 (8000000)

/* BDCR register base address */
#define BDCR_BASE                 (PERIPH_BASE + BDCR_OFFSET)

/* RCC Flag */
#define RCC_FLAG_HSIRDY                  (0x20)
#define RCC_FLAG_HSERDY                  (0x31)
#define RCC_FLAG_PLLRDY                  (0x39)
#define RCC_FLAG_LSERDY                  (0x41)
#define RCC_FLAG_LSIRDY                  (0x61)
#define RCC_FLAG_PINRST                  (0x7A)
#define RCC_FLAG_PORRST                  (0x7B)
#define RCC_FLAG_SFTRST                  (0x7C)
#define RCC_FLAG_IWDGRST                 (0x7D)
#define RCC_FLAG_WWDGRST                 (0x7E)
#define RCC_FLAG_LPWRRST                 (0x7F)

/* System clock source */
#define RCC_SYSCLK_HSI             (0x00000000)
#define RCC_SYSCLK_HSE             (0x00000001)
#define RCC_SYSCLK_PLLCLK          (0x00000002)

/* PLL entry clock source */
#define RCC_PLL_HSI_DIV2           (0x00000000)
#define RCC_PLL_HSE_DIV1           (0x00010000)
#define RCC_PLL_HSE_DIV2           (0x00030000)

/* PLL multiplication factor */
#define RCC_PLLMUL_2               (0x00000000)
#define RCC_PLLMUL_3               (0x00040000)
#define RCC_PLLMUL_4               (0x00080000)
#define RCC_PLLMUL_5               (0x000C0000)
#define RCC_PLLMUL_6               (0x00100000)
#define RCC_PLLMUL_7               (0x00140000)
#define RCC_PLLMUL_8               (0x00180000)
#define RCC_PLLMUL_9               (0x001C0000)
#define RCC_PLLMUL_10              (0x00200000)
#define RCC_PLLMUL_11              (0x00240000)
#define RCC_PLLMUL_12              (0x00280000)
#define RCC_PLLMUL_13              (0x002C0000)
#define RCC_PLLMUL_14              (0x00300000)
#define RCC_PLLMUL_15              (0x00340000)
#define RCC_PLLMUL_16              (0x00380000)

/* APB1/APB2 clock source */
#define RCC_HCLK_DIV1              (0x00000000)
#define RCC_HCLK_DIV2              (0x00000400)
#define RCC_HCLK_DIV4              (0x00000500)
#define RCC_HCLK_DIV8              (0x00000600)
#define RCC_HCLK_DIV16             (0x00000700)

/* USB clock source */
#define RCC_USBCLK_PLLCLK_1DIV5    (0x00)
#define RCC_USBCLK_PLLCLK_DIV1     (0x01)

/* ADC clock source */
#define RCC_PCLK2_DIV2             (0x00000000)
#define RCC_PCLK2_DIV4             (0x00004000)
#define RCC_PCLK2_DIV6             (0x00008000)
#define RCC_PCLK2_DIV8             (0x0000C000)

/* AHB clock source */
#define RCC_SYSCLK_DIV1            (0x00000000)
#define RCC_SYSCLK_DIV2            (0x00000080)
#define RCC_SYSCLK_DIV4            (0x00000090)
#define RCC_SYSCLK_DIV8            (0x000000A0)
#define RCC_SYSCLK_DIV16           (0x000000B0)
#define RCC_SYSCLK_DIV64           (0x000000C0)
#define RCC_SYSCLK_DIV128          (0x000000D0)
#define RCC_SYSCLK_DIV256          (0x000000E0)
#define RCC_SYSCLK_DIV512          (0x000000F0)
/*\}*/

/**
 * RCC register: APB1 peripheral
 */
/*\{*/
#define RCC_APB1_TIM2              (0x00000001)
#define RCC_APB1_TIM3              (0x00000002)
#define RCC_APB1_TIM4              (0x00000004)
#define RCC_APB1_WWDG              (0x00000800)
#define RCC_APB1_SPI2              (0x00004000)
#define RCC_APB1_USART2            (0x00020000)
#define RCC_APB1_USART3            (0x00040000)
#define RCC_APB1_I2C1              (0x00200000)
#define RCC_APB1_I2C2              (0x00400000)
#define RCC_APB1_USB               (0x00800000)
#define RCC_APB1_CAN               (0x02000000)
#define RCC_APB1_BKP               (0x08000000)
#define RCC_APB1_PWR               (0x10000000)
#define RCC_APB1_ALL               (0x1AE64807)
/*\}*/

/**
 * RCC register: APB2 peripheral
 */
/*\{*/
#define RCC_APB2_AFIO              (0x00000001)
#define RCC_APB2_GPIOA             (0x00000004)
#define RCC_APB2_GPIOB             (0x00000008)
#define RCC_APB2_GPIOC             (0x00000010)
#define RCC_APB2_GPIOD             (0x00000020)
#define RCC_APB2_GPIOE             (0x00000040)
#define RCC_APB2_ADC1              (0x00000200)
#define RCC_APB2_ADC2              (0x00000400)
#define RCC_APB2_TIM1              (0x00000800)
#define RCC_APB2_SPI1              (0x00001000)
#define RCC_APB2_USART1            (0x00004000)
#define RCC_APB2_ALL               (0x00005E7D)

/**
 * RCC register:  BCDR
 */
#define RCC_BDCR_LSEON             (0x00000001)
#define RCC_BDCR_LSERDY            (0x00000002)
#define RCC_BDCR_RTCSEL            (0x00000300)
#define RCC_BDCR_RTCEN             (0x00008000)
/*\}*/

/* Crystal frequency of the main oscillator (8MHz) */
#define PLL_VCO                    8000000

/* Reset and Clock Controller registers */
struct RCC
{
	reg32_t CR;
	reg32_t CFGR;
	reg32_t CIR;
	reg32_t APB2RSTR;
	reg32_t APB1RSTR;
	reg32_t AHBENR;
	reg32_t APB2ENR;
	reg32_t APB1ENR;
	reg32_t BDCR;
	reg32_t CSR;
};

/* RCC registers base */
extern struct RCC *RCC;

#elif CPU_CM3_STM32L1

struct RCC
{
	reg32_t CR;            /*!< RCC clock control register,                                   Address offset: 0x00 */
	reg32_t ICSCR;         /*!< RCC Internal clock sources calibration register,              Address offset: 0x04 */
	reg32_t CFGR;          /*!< RCC Clock configuration register,                             Address offset: 0x08 */
	reg32_t CIR;           /*!< RCC Clock interrupt register,                                 Address offset: 0x0C */
	reg32_t AHBRSTR;       /*!< RCC AHB peripheral reset register,                            Address offset: 0x10 */
	reg32_t APB2RSTR;      /*!< RCC APB2 peripheral reset register,                           Address offset: 0x14 */
	reg32_t APB1RSTR;      /*!< RCC APB1 peripheral reset register,                           Address offset: 0x18 */
	reg32_t AHBENR;        /*!< RCC AHB peripheral clock enable register,                     Address offset: 0x1C */
	reg32_t APB2ENR;       /*!< RCC APB2 peripheral clock enable register,                    Address offset: 0x20 */
	reg32_t APB1ENR;       /*!< RCC APB1 peripheral clock enable register,                    Address offset: 0x24 */
	reg32_t AHBLPENR;      /*!< RCC AHB peripheral clock enable in low power mode register,   Address offset: 0x28 */
	reg32_t APB2LPENR;     /*!< RCC APB2 peripheral clock enable in low power mode register,  Address offset: 0x2C */
	reg32_t APB1LPENR;     /*!< RCC APB1 peripheral clock enable in low power mode register,  Address offset: 0x30 */
	reg32_t CSR;           /*!< RCC Control/status register,                                  Address offset: 0x34 */
};

#define RCC_GPIO_ENABLE(base, gpio) ((base)->AHBENR |= BV(gpio))
#define RCC_GPIO_DISABLE(base, gpio) ((base)->AHBENR &= ~BV(gpio))

/******************************************************************************/
/*                                                                            */
/*                      Reset and Clock Control (RCC)                         */
/*                                                                            */
/******************************************************************************/
/********************  Bit definition for RCC_CR register  ********************/
#define  RCC_CR_HSION                        ((uint32_t)0x00000001)        /*!< Internal High Speed clock enable */
#define  RCC_CR_HSIRDY                       ((uint32_t)0x00000002)        /*!< Internal High Speed clock ready flag */

#define  RCC_CR_MSION                        ((uint32_t)0x00000100)        /*!< Internal Multi Speed clock enable */
#define  RCC_CR_MSIRDY                       ((uint32_t)0x00000200)        /*!< Internal Multi Speed clock ready flag */

#define  RCC_CR_HSEON                        ((uint32_t)0x00010000)        /*!< External High Speed clock enable */
#define  RCC_CR_HSERDY                       ((uint32_t)0x00020000)        /*!< External High Speed clock ready flag */
#define  RCC_CR_HSEBYP                       ((uint32_t)0x00040000)        /*!< External High Speed clock Bypass */

#define  RCC_CR_PLLON                        ((uint32_t)0x01000000)        /*!< PLL enable */
#define  RCC_CR_PLLRDY                       ((uint32_t)0x02000000)        /*!< PLL clock ready flag */
#define  RCC_CR_CSSON                        ((uint32_t)0x10000000)        /*!< Clock Security System enable */

#define  RCC_CR_RTCPRE                       ((uint32_t)0x60000000)        /*!< RTC/LCD Prescaler */
#define  RCC_CR_RTCPRE_0                     ((uint32_t)0x20000000)        /*!< Bit0 */
#define  RCC_CR_RTCPRE_1                     ((uint32_t)0x40000000)        /*!< Bit1 */

#define STM32_RTCPRE_MASK       (3 << 29)   /**< RTCPRE mask.               */
#define STM32_RTCPRE_DIV2       (0 << 29)   /**< HSE divided by 2.          */
#define STM32_RTCPRE_DIV4       (1 << 29)   /**< HSE divided by 4.          */
#define STM32_RTCPRE_DIV8       (2 << 29)   /**< HSE divided by 2.          */
#define STM32_RTCPRE_DIV16      (3 << 29)   /**< HSE divided by 16.         */

/********************  Bit definition for RCC_ICSCR register  *****************/
#define STM32_MSIRANGE_MASK     (7 << 13)   /**< MSIRANGE field mask.       */
#define STM32_MSIRANGE_64K      (0 << 13)   /**< 64kHz nominal.             */
#define STM32_MSIRANGE_128K     (1 << 13)   /**< 128kHz nominal.            */
#define STM32_MSIRANGE_256K     (2 << 13)   /**< 256kHz nominal.            */
#define STM32_MSIRANGE_512K     (3 << 13)   /**< 512kHz nominal.            */
#define STM32_MSIRANGE_1M       (4 << 13)   /**< 1MHz nominal.              */
#define STM32_MSIRANGE_2M       (5 << 13)   /**< 2MHz nominal.              */
#define STM32_MSIRANGE_4M       (6 << 13)   /**< 4MHz nominal               */

#define  RCC_ICSCR_HSICAL                    ((uint32_t)0x000000FF)        /*!< Internal High Speed clock Calibration */
#define  RCC_ICSCR_HSITRIM                   ((uint32_t)0x00001F00)        /*!< Internal High Speed clock trimming */

#define  RCC_ICSCR_MSIRANGE                  ((uint32_t)0x0000E000)        /*!< Internal Multi Speed clock Range */
#define  RCC_ICSCR_MSIRANGE_0                ((uint32_t)0x00000000)        /*!< Internal Multi Speed clock Range 65.536 KHz */
#define  RCC_ICSCR_MSIRANGE_1                ((uint32_t)0x00002000)        /*!< Internal Multi Speed clock Range 131.072 KHz */
#define  RCC_ICSCR_MSIRANGE_2                ((uint32_t)0x00004000)        /*!< Internal Multi Speed clock Range 262.144 KHz */
#define  RCC_ICSCR_MSIRANGE_3                ((uint32_t)0x00006000)        /*!< Internal Multi Speed clock Range 524.288 KHz */
#define  RCC_ICSCR_MSIRANGE_4                ((uint32_t)0x00008000)        /*!< Internal Multi Speed clock Range 1.048 MHz */
#define  RCC_ICSCR_MSIRANGE_5                ((uint32_t)0x0000A000)        /*!< Internal Multi Speed clock Range 2.097 MHz */
#define  RCC_ICSCR_MSIRANGE_6                ((uint32_t)0x0000C000)        /*!< Internal Multi Speed clock Range 4.194 MHz */
#define  RCC_ICSCR_MSICAL                    ((uint32_t)0x00FF0000)        /*!< Internal Multi Speed clock Calibration */
#define  RCC_ICSCR_MSITRIM                   ((uint32_t)0xFF000000)        /*!< Internal Multi Speed clock trimming */

/********************  Bit definition for RCC_CFGR register  ******************/
#define  RCC_CFGR_SW                         ((uint32_t)0x00000003)        /*!< SW[1:0] bits (System clock Switch) */
#define  RCC_CFGR_SW_0                       ((uint32_t)0x00000001)        /*!< Bit 0 */
#define  RCC_CFGR_SW_1                       ((uint32_t)0x00000002)        /*!< Bit 1 */

/*!< SW configuration */
#define  RCC_CFGR_SW_MSI                     ((uint32_t)0x00000000)        /*!< MSI selected as system clock */
#define  RCC_CFGR_SW_HSI                     ((uint32_t)0x00000001)        /*!< HSI selected as system clock */
#define  RCC_CFGR_SW_HSE                     ((uint32_t)0x00000002)        /*!< HSE selected as system clock */
#define  RCC_CFGR_SW_PLL                     ((uint32_t)0x00000003)        /*!< PLL selected as system clock */

#define  RCC_CFGR_SWS                        ((uint32_t)0x0000000C)        /*!< SWS[1:0] bits (System Clock Switch Status) */
#define  RCC_CFGR_SWS_0                      ((uint32_t)0x00000004)        /*!< Bit 0 */
#define  RCC_CFGR_SWS_1                      ((uint32_t)0x00000008)        /*!< Bit 1 */

/*!< SWS configuration */
#define  RCC_CFGR_SWS_MSI                    ((uint32_t)0x00000000)        /*!< MSI oscillator used as system clock */
#define  RCC_CFGR_SWS_HSI                    ((uint32_t)0x00000004)        /*!< HSI oscillator used as system clock */
#define  RCC_CFGR_SWS_HSE                    ((uint32_t)0x00000008)        /*!< HSE oscillator used as system clock */
#define  RCC_CFGR_SWS_PLL                    ((uint32_t)0x0000000C)        /*!< PLL used as system clock */

#define  RCC_CFGR_HPRE                       ((uint32_t)0x000000F0)        /*!< HPRE[3:0] bits (AHB prescaler) */
#define  RCC_CFGR_HPRE_0                     ((uint32_t)0x00000010)        /*!< Bit 0 */
#define  RCC_CFGR_HPRE_1                     ((uint32_t)0x00000020)        /*!< Bit 1 */
#define  RCC_CFGR_HPRE_2                     ((uint32_t)0x00000040)        /*!< Bit 2 */
#define  RCC_CFGR_HPRE_3                     ((uint32_t)0x00000080)        /*!< Bit 3 */

/*!< HPRE configuration */
#define  RCC_CFGR_HPRE_DIV1                  ((uint32_t)0x00000000)        /*!< SYSCLK not divided */
#define  RCC_CFGR_HPRE_DIV2                  ((uint32_t)0x00000080)        /*!< SYSCLK divided by 2 */
#define  RCC_CFGR_HPRE_DIV4                  ((uint32_t)0x00000090)        /*!< SYSCLK divided by 4 */
#define  RCC_CFGR_HPRE_DIV8                  ((uint32_t)0x000000A0)        /*!< SYSCLK divided by 8 */
#define  RCC_CFGR_HPRE_DIV16                 ((uint32_t)0x000000B0)        /*!< SYSCLK divided by 16 */
#define  RCC_CFGR_HPRE_DIV64                 ((uint32_t)0x000000C0)        /*!< SYSCLK divided by 64 */
#define  RCC_CFGR_HPRE_DIV128                ((uint32_t)0x000000D0)        /*!< SYSCLK divided by 128 */
#define  RCC_CFGR_HPRE_DIV256                ((uint32_t)0x000000E0)        /*!< SYSCLK divided by 256 */
#define  RCC_CFGR_HPRE_DIV512                ((uint32_t)0x000000F0)        /*!< SYSCLK divided by 512 */

#define  RCC_CFGR_PPRE1                      ((uint32_t)0x00000700)        /*!< PRE1[2:0] bits (APB1 prescaler) */
#define  RCC_CFGR_PPRE1_0                    ((uint32_t)0x00000100)        /*!< Bit 0 */
#define  RCC_CFGR_PPRE1_1                    ((uint32_t)0x00000200)        /*!< Bit 1 */
#define  RCC_CFGR_PPRE1_2                    ((uint32_t)0x00000400)        /*!< Bit 2 */

/*!< PPRE1 configuration */
#define  RCC_CFGR_PPRE1_DIV1                 ((uint32_t)0x00000000)        /*!< HCLK not divided */
#define  RCC_CFGR_PPRE1_DIV2                 ((uint32_t)0x00000400)        /*!< HCLK divided by 2 */
#define  RCC_CFGR_PPRE1_DIV4                 ((uint32_t)0x00000500)        /*!< HCLK divided by 4 */
#define  RCC_CFGR_PPRE1_DIV8                 ((uint32_t)0x00000600)        /*!< HCLK divided by 8 */
#define  RCC_CFGR_PPRE1_DIV16                ((uint32_t)0x00000700)        /*!< HCLK divided by 16 */

#define  RCC_CFGR_PPRE2                      ((uint32_t)0x00003800)        /*!< PRE2[2:0] bits (APB2 prescaler) */
#define  RCC_CFGR_PPRE2_0                    ((uint32_t)0x00000800)        /*!< Bit 0 */
#define  RCC_CFGR_PPRE2_1                    ((uint32_t)0x00001000)        /*!< Bit 1 */
#define  RCC_CFGR_PPRE2_2                    ((uint32_t)0x00002000)        /*!< Bit 2 */

/*!< PPRE2 configuration */
#define  RCC_CFGR_PPRE2_DIV1                 ((uint32_t)0x00000000)        /*!< HCLK not divided */
#define  RCC_CFGR_PPRE2_DIV2                 ((uint32_t)0x00002000)        /*!< HCLK divided by 2 */
#define  RCC_CFGR_PPRE2_DIV4                 ((uint32_t)0x00002800)        /*!< HCLK divided by 4 */
#define  RCC_CFGR_PPRE2_DIV8                 ((uint32_t)0x00003000)        /*!< HCLK divided by 8 */
#define  RCC_CFGR_PPRE2_DIV16                ((uint32_t)0x00003800)        /*!< HCLK divided by 16 */

/*!< PLL entry clock source*/
#define  RCC_CFGR_PLLSRC                     ((uint32_t)0x00010000)        /*!< PLL entry clock source */

#define  RCC_CFGR_PLLSRC_HSI                 ((uint32_t)0x00000000)        /*!< HSI as PLL entry clock source */
#define  RCC_CFGR_PLLSRC_HSE                 ((uint32_t)0x00010000)        /*!< HSE as PLL entry clock source */


#define  RCC_CFGR_PLLMUL                     ((uint32_t)0x003C0000)        /*!< PLLMUL[3:0] bits (PLL multiplication factor) */
#define  RCC_CFGR_PLLMUL_0                   ((uint32_t)0x00040000)        /*!< Bit 0 */
#define  RCC_CFGR_PLLMUL_1                   ((uint32_t)0x00080000)        /*!< Bit 1 */
#define  RCC_CFGR_PLLMUL_2                   ((uint32_t)0x00100000)        /*!< Bit 2 */
#define  RCC_CFGR_PLLMUL_3                   ((uint32_t)0x00200000)        /*!< Bit 3 */

/*!< PLLMUL configuration */
#define  RCC_CFGR_PLLMUL3                    ((uint32_t)0x00000000)        /*!< PLL input clock * 3 */
#define  RCC_CFGR_PLLMUL4                    ((uint32_t)0x00040000)        /*!< PLL input clock * 4 */
#define  RCC_CFGR_PLLMUL6                    ((uint32_t)0x00080000)        /*!< PLL input clock * 6 */
#define  RCC_CFGR_PLLMUL8                    ((uint32_t)0x000C0000)        /*!< PLL input clock * 8 */
#define  RCC_CFGR_PLLMUL12                   ((uint32_t)0x00100000)        /*!< PLL input clock * 12 */
#define  RCC_CFGR_PLLMUL16                   ((uint32_t)0x00140000)        /*!< PLL input clock * 16 */
#define  RCC_CFGR_PLLMUL24                   ((uint32_t)0x00180000)        /*!< PLL input clock * 24 */
#define  RCC_CFGR_PLLMUL32                   ((uint32_t)0x001C0000)        /*!< PLL input clock * 32 */
#define  RCC_CFGR_PLLMUL48                   ((uint32_t)0x00200000)        /*!< PLL input clock * 48 */

/*!< PLLDIV configuration */
#define  RCC_CFGR_PLLDIV                     ((uint32_t)0x00C00000)        /*!< PLLDIV[1:0] bits (PLL Output Division) */
#define  RCC_CFGR_PLLDIV_0                   ((uint32_t)0x00400000)        /*!< Bit0 */
#define  RCC_CFGR_PLLDIV_1                   ((uint32_t)0x00800000)        /*!< Bit1 */


/*!< PLLDIV configuration */
#define  RCC_CFGR_PLLDIV1                    ((uint32_t)0x00000000)        /*!< PLL clock output = CKVCO / 1 */
#define  RCC_CFGR_PLLDIV2                    ((uint32_t)0x00400000)        /*!< PLL clock output = CKVCO / 2 */
#define  RCC_CFGR_PLLDIV3                    ((uint32_t)0x00800000)        /*!< PLL clock output = CKVCO / 3 */
#define  RCC_CFGR_PLLDIV4                    ((uint32_t)0x00C00000)        /*!< PLL clock output = CKVCO / 4 */


#define  RCC_CFGR_MCOSEL                     ((uint32_t)0x07000000)        /*!< MCO[2:0] bits (Microcontroller Clock Output) */
#define  RCC_CFGR_MCOSEL_0                   ((uint32_t)0x01000000)        /*!< Bit 0 */
#define  RCC_CFGR_MCOSEL_1                   ((uint32_t)0x02000000)        /*!< Bit 1 */
#define  RCC_CFGR_MCOSEL_2                   ((uint32_t)0x04000000)        /*!< Bit 2 */

/*!< MCO configuration */
#define  RCC_CFGR_MCO_NOCLOCK                ((uint32_t)0x00000000)        /*!< No clock */
#define  RCC_CFGR_MCO_SYSCLK                 ((uint32_t)0x01000000)        /*!< System clock selected */
#define  RCC_CFGR_MCO_HSI                    ((uint32_t)0x02000000)        /*!< Internal 16 MHz RC oscillator clock selected */
#define  RCC_CFGR_MCO_MSI                    ((uint32_t)0x03000000)        /*!< Internal Medium Speed RC oscillator clock selected */
#define  RCC_CFGR_MCO_HSE                    ((uint32_t)0x04000000)        /*!< External 1-25 MHz oscillator clock selected */
#define  RCC_CFGR_MCO_PLL                    ((uint32_t)0x05000000)        /*!< PLL clock divided */
#define  RCC_CFGR_MCO_LSI                    ((uint32_t)0x06000000)        /*!< LSI selected */
#define  RCC_CFGR_MCO_LSE                    ((uint32_t)0x07000000)        /*!< LSE selected */

#define  RCC_CFGR_MCOPRE                     ((uint32_t)0x70000000)        /*!< MCOPRE[2:0] bits (Microcontroller Clock Output Prescaler) */
#define  RCC_CFGR_MCOPRE_0                   ((uint32_t)0x10000000)        /*!< Bit 0 */
#define  RCC_CFGR_MCOPRE_1                   ((uint32_t)0x20000000)        /*!< Bit 1 */
#define  RCC_CFGR_MCOPRE_2                   ((uint32_t)0x40000000)        /*!< Bit 2 */

/*!< MCO Prescaler configuration */
#define  RCC_CFGR_MCO_DIV1                   ((uint32_t)0x00000000)        /*!< MCO Clock divided by 1 */
#define  RCC_CFGR_MCO_DIV2                   ((uint32_t)0x10000000)        /*!< MCO Clock divided by 2 */
#define  RCC_CFGR_MCO_DIV4                   ((uint32_t)0x20000000)        /*!< MCO Clock divided by 4 */
#define  RCC_CFGR_MCO_DIV8                   ((uint32_t)0x30000000)        /*!< MCO Clock divided by 8 */
#define  RCC_CFGR_MCO_DIV16                  ((uint32_t)0x40000000)        /*!< MCO Clock divided by 16 */

/*!<******************  Bit definition for RCC_CIR register  ********************/
#define  RCC_CIR_LSIRDYF                     ((uint32_t)0x00000001)        /*!< LSI Ready Interrupt flag */
#define  RCC_CIR_LSERDYF                     ((uint32_t)0x00000002)        /*!< LSE Ready Interrupt flag */
#define  RCC_CIR_HSIRDYF                     ((uint32_t)0x00000004)        /*!< HSI Ready Interrupt flag */
#define  RCC_CIR_HSERDYF                     ((uint32_t)0x00000008)        /*!< HSE Ready Interrupt flag */
#define  RCC_CIR_PLLRDYF                     ((uint32_t)0x00000010)        /*!< PLL Ready Interrupt flag */
#define  RCC_CIR_MSIRDYF                     ((uint32_t)0x00000020)        /*!< MSI Ready Interrupt flag */
#define  RCC_CIR_LSECSS                      ((uint32_t)0x00000040)        /*!< LSE CSS Interrupt flag */
#define  RCC_CIR_CSSF                        ((uint32_t)0x00000080)        /*!< Clock Security System Interrupt flag */

#define  RCC_CIR_LSIRDYIE                    ((uint32_t)0x00000100)        /*!< LSI Ready Interrupt Enable */
#define  RCC_CIR_LSERDYIE                    ((uint32_t)0x00000200)        /*!< LSE Ready Interrupt Enable */
#define  RCC_CIR_HSIRDYIE                    ((uint32_t)0x00000400)        /*!< HSI Ready Interrupt Enable */
#define  RCC_CIR_HSERDYIE                    ((uint32_t)0x00000800)        /*!< HSE Ready Interrupt Enable */
#define  RCC_CIR_PLLRDYIE                    ((uint32_t)0x00001000)        /*!< PLL Ready Interrupt Enable */
#define  RCC_CIR_MSIRDYIE                    ((uint32_t)0x00002000)        /*!< MSI Ready Interrupt Enable */
#define  RCC_CIR_LSECSSIE                    ((uint32_t)0x00004000)        /*!< LSE CSS Interrupt Enable */

#define  RCC_CIR_LSIRDYC                     ((uint32_t)0x00010000)        /*!< LSI Ready Interrupt Clear */
#define  RCC_CIR_LSERDYC                     ((uint32_t)0x00020000)        /*!< LSE Ready Interrupt Clear */
#define  RCC_CIR_HSIRDYC                     ((uint32_t)0x00040000)        /*!< HSI Ready Interrupt Clear */
#define  RCC_CIR_HSERDYC                     ((uint32_t)0x00080000)        /*!< HSE Ready Interrupt Clear */
#define  RCC_CIR_PLLRDYC                     ((uint32_t)0x00100000)        /*!< PLL Ready Interrupt Clear */
#define  RCC_CIR_MSIRDYC                     ((uint32_t)0x00200000)        /*!< MSI Ready Interrupt Clear */
#define  RCC_CIR_LSECSSC                     ((uint32_t)0x00400000)        /*!< LSE CSS Interrupt Clear */
#define  RCC_CIR_CSSC                        ((uint32_t)0x00800000)        /*!< Clock Security System Interrupt Clear */

/**
 * @name    PWR_CR register bits definitions
 * @{
 */
#define STM32_VOS_MASK          (3 << 11)   /**< Core voltage mask.         */
#define STM32_VOS_1P8           (1 << 11)   /**< Core voltage 1.8 Volts.    */
#define STM32_VOS_1P5           (2 << 11)   /**< Core voltage 1.5 Volts.    */
#define STM32_VOS_1P2           (3 << 11)   /**< Core voltage 1.2 Volts.    */

#define STM32_PLS_MASK          (7 << 5)    /**< PLS bits mask.             */
#define STM32_PLS_LEV0          (0 << 5)    /**< PVD level 0.               */
#define STM32_PLS_LEV1          (1 << 5)    /**< PVD level 1.               */
#define STM32_PLS_LEV2          (2 << 5)    /**< PVD level 2.               */
#define STM32_PLS_LEV3          (3 << 5)    /**< PVD level 3.               */
#define STM32_PLS_LEV4          (4 << 5)    /**< PVD level 4.               */
#define STM32_PLS_LEV5          (5 << 5)    /**< PVD level 5.               */
#define STM32_PLS_LEV6          (6 << 5)    /**< PVD level 6.               */
#define STM32_PLS_LEV7          (7 << 5)    /**< PVD level 7.               */
/** @} */

#define STM32_SW_MSI            (0 << 0)    /**< SYSCLK source is MSI.      */
#define STM32_SW_HSI            (1 << 0)    /**< SYSCLK source is HSI.      */
#define STM32_SW_HSE            (2 << 0)    /**< SYSCLK source is HSE.      */
#define STM32_SW_PLL            (3 << 0)    /**< SYSCLK source is PLL.      */

#define STM32_HPRE_DIV1         (0 << 4)    /**< SYSCLK divided by 1.       */
#define STM32_HPRE_DIV2         (8 << 4)    /**< SYSCLK divided by 2.       */
#define STM32_HPRE_DIV4         (9 << 4)    /**< SYSCLK divided by 4.       */
#define STM32_HPRE_DIV8         (10 << 4)   /**< SYSCLK divided by 8.       */
#define STM32_HPRE_DIV16        (11 << 4)   /**< SYSCLK divided by 16.      */
#define STM32_HPRE_DIV64        (12 << 4)   /**< SYSCLK divided by 64.      */
#define STM32_HPRE_DIV128       (13 << 4)   /**< SYSCLK divided by 128.     */
#define STM32_HPRE_DIV256       (14 << 4)   /**< SYSCLK divided by 256.     */
#define STM32_HPRE_DIV512       (15 << 4)   /**< SYSCLK divided by 512.     */

#define STM32_PPRE1_DIV1        (0 << 8)    /**< HCLK divided by 1.         */
#define STM32_PPRE1_DIV2        (4 << 8)    /**< HCLK divided by 2.         */
#define STM32_PPRE1_DIV4        (5 << 8)    /**< HCLK divided by 4.         */
#define STM32_PPRE1_DIV8        (6 << 8)    /**< HCLK divided by 8.         */
#define STM32_PPRE1_DIV16       (7 << 8)    /**< HCLK divided by 16.        */

#define STM32_PPRE2_DIV1        (0 << 11)   /**< HCLK divided by 1.         */
#define STM32_PPRE2_DIV2        (4 << 11)   /**< HCLK divided by 2.         */
#define STM32_PPRE2_DIV4        (5 << 11)   /**< HCLK divided by 4.         */
#define STM32_PPRE2_DIV8        (6 << 11)   /**< HCLK divided by 8.         */
#define STM32_PPRE2_DIV16       (7 << 11)   /**< HCLK divided by 16.        */

#define STM32_PLLSRC_HSI        (0 << 16)   /**< PLL clock source is HSI.   */
#define STM32_PLLSRC_HSE        (1 << 16)   /**< PLL clock source is HSE.   */

#define STM32_MCOSEL_NOCLOCK    (0 << 24)   /**< No clock on MCO pin.       */
#define STM32_MCOSEL_SYSCLK     (1 << 24)   /**< SYSCLK on MCO pin.         */
#define STM32_MCOSEL_HSI        (2 << 24)   /**< HSI clock on MCO pin.      */
#define STM32_MCOSEL_MSI        (3 << 24)   /**< MSI clock on MCO pin.      */
#define STM32_MCOSEL_HSE        (4 << 24)   /**< HSE clock on MCO pin.      */
#define STM32_MCOSEL_PLL        (5 << 24)   /**< PLL clock on MCO pin.      */
#define STM32_MCOSEL_LSI        (6 << 24)   /**< LSI clock on MCO pin.      */
#define STM32_MCOSEL_LSE        (7 << 24)   /**< LSE clock on MCO pin.      */

#define STM32_MCOPRE_DIV1       (0 << 28)   /**< MCO divided by 1.          */
#define STM32_MCOPRE_DIV2       (1 << 28)   /**< MCO divided by 1.          */
#define STM32_MCOPRE_DIV4       (2 << 28)   /**< MCO divided by 1.          */
#define STM32_MCOPRE_DIV8       (3 << 28)   /**< MCO divided by 1.          */
#define STM32_MCOPRE_DIV16      (4 << 28)   /**< MCO divided by 1.          */

/*****************  Bit definition for RCC_AHBRSTR register  ******************/
#define  RCC_AHBRSTR_GPIOARST                ((uint32_t)0x00000001)        /*!< GPIO port A reset */
#define  RCC_AHBRSTR_GPIOBRST                ((uint32_t)0x00000002)        /*!< GPIO port B reset */
#define  RCC_AHBRSTR_GPIOCRST                ((uint32_t)0x00000004)        /*!< GPIO port C reset */
#define  RCC_AHBRSTR_GPIODRST                ((uint32_t)0x00000008)        /*!< GPIO port D reset */
#define  RCC_AHBRSTR_GPIOERST                ((uint32_t)0x00000010)        /*!< GPIO port E reset */
#define  RCC_AHBRSTR_GPIOHRST                ((uint32_t)0x00000020)        /*!< GPIO port H reset */
#define  RCC_AHBRSTR_GPIOFRST                ((uint32_t)0x00000040)        /*!< GPIO port F reset */
#define  RCC_AHBRSTR_GPIOGRST                ((uint32_t)0x00000080)        /*!< GPIO port G reset */
#define  RCC_AHBRSTR_CRCRST                  ((uint32_t)0x00001000)        /*!< CRC reset */
#define  RCC_AHBRSTR_FLITFRST                ((uint32_t)0x00008000)        /*!< FLITF reset */
#define  RCC_AHBRSTR_DMA1RST                 ((uint32_t)0x01000000)        /*!< DMA1 reset */
#define  RCC_AHBRSTR_DMA2RST                 ((uint32_t)0x02000000)        /*!< DMA2 reset */
#define  RCC_AHBRSTR_AESRST                  ((uint32_t)0x08000000)        /*!< AES reset */
#define  RCC_AHBRSTR_FSMCRST                 ((uint32_t)0x40000000)        /*!< FSMC reset */
 
/*****************  Bit definition for RCC_APB2RSTR register  *****************/
#define  RCC_APB2RSTR_SYSCFGRST              ((uint32_t)0x00000001)        /*!< System Configuration SYSCFG reset */
#define  RCC_APB2RSTR_TIM9RST                ((uint32_t)0x00000004)        /*!< TIM9 reset */
#define  RCC_APB2RSTR_TIM10RST               ((uint32_t)0x00000008)        /*!< TIM10 reset */
#define  RCC_APB2RSTR_TIM11RST               ((uint32_t)0x00000010)        /*!< TIM11 reset */
#define  RCC_APB2RSTR_ADC1RST                ((uint32_t)0x00000200)        /*!< ADC1 reset */
#define  RCC_APB2RSTR_SDIORST                ((uint32_t)0x00000800)        /*!< SDIO reset */
#define  RCC_APB2RSTR_SPI1RST                ((uint32_t)0x00001000)        /*!< SPI1 reset */
#define  RCC_APB2RSTR_USART1RST              ((uint32_t)0x00004000)        /*!< USART1 reset */

/*****************  Bit definition for RCC_APB1RSTR register  *****************/
#define  RCC_APB1RSTR_TIM2RST                ((uint32_t)0x00000001)        /*!< Timer 2 reset */
#define  RCC_APB1RSTR_TIM3RST                ((uint32_t)0x00000002)        /*!< Timer 3 reset */
#define  RCC_APB1RSTR_TIM4RST                ((uint32_t)0x00000004)        /*!< Timer 4 reset */
#define  RCC_APB1RSTR_TIM5RST                ((uint32_t)0x00000008)        /*!< Timer 5 reset */
#define  RCC_APB1RSTR_TIM6RST                ((uint32_t)0x00000010)        /*!< Timer 6 reset */
#define  RCC_APB1RSTR_TIM7RST                ((uint32_t)0x00000020)        /*!< Timer 7 reset */
#define  RCC_APB1RSTR_LCDRST                 ((uint32_t)0x00000200)        /*!< LCD reset */
#define  RCC_APB1RSTR_WWDGRST                ((uint32_t)0x00000800)        /*!< Window Watchdog reset */
#define  RCC_APB1RSTR_SPI2RST                ((uint32_t)0x00004000)        /*!< SPI 2 reset */
#define  RCC_APB1RSTR_SPI3RST                ((uint32_t)0x00008000)        /*!< SPI 3 reset */
#define  RCC_APB1RSTR_USART2RST              ((uint32_t)0x00020000)        /*!< USART 2 reset */
#define  RCC_APB1RSTR_USART3RST              ((uint32_t)0x00040000)        /*!< USART 3 reset */
#define  RCC_APB1RSTR_UART4RST               ((uint32_t)0x00080000)        /*!< UART 4 reset */
#define  RCC_APB1RSTR_UART5RST               ((uint32_t)0x00100000)        /*!< UART 5 reset */
#define  RCC_APB1RSTR_I2C1RST                ((uint32_t)0x00200000)        /*!< I2C 1 reset */
#define  RCC_APB1RSTR_I2C2RST                ((uint32_t)0x00400000)        /*!< I2C 2 reset */
#define  RCC_APB1RSTR_USBRST                 ((uint32_t)0x00800000)        /*!< USB reset */
#define  RCC_APB1RSTR_PWRRST                 ((uint32_t)0x10000000)        /*!< Power interface reset */
#define  RCC_APB1RSTR_DACRST                 ((uint32_t)0x20000000)        /*!< DAC interface reset */
#define  RCC_APB1RSTR_COMPRST                ((uint32_t)0x80000000)        /*!< Comparator interface reset */

/******************  Bit definition for RCC_AHBENR register  ******************/
#define  RCC_AHBENR_GPIOAEN                  ((uint32_t)0x00000001)        /*!< GPIO port A clock enable */
#define  RCC_AHBENR_GPIOBEN                  ((uint32_t)0x00000002)        /*!< GPIO port B clock enable */
#define  RCC_AHBENR_GPIOCEN                  ((uint32_t)0x00000004)        /*!< GPIO port C clock enable */
#define  RCC_AHBENR_GPIODEN                  ((uint32_t)0x00000008)        /*!< GPIO port D clock enable */
#define  RCC_AHBENR_GPIOEEN                  ((uint32_t)0x00000010)        /*!< GPIO port E clock enable */
#define  RCC_AHBENR_GPIOHEN                  ((uint32_t)0x00000020)        /*!< GPIO port H clock enable */
#define  RCC_AHBENR_GPIOFEN                  ((uint32_t)0x00000040)        /*!< GPIO port F clock enable */
#define  RCC_AHBENR_GPIOGEN                  ((uint32_t)0x00000080)        /*!< GPIO port G clock enable */
#define  RCC_AHBENR_CRCEN                    ((uint32_t)0x00001000)        /*!< CRC clock enable */
#define  RCC_AHBENR_FLITFEN                  ((uint32_t)0x00008000)        /*!< FLITF clock enable (has effect only when
                                                                                the Flash memory is in power down mode) */
#define  RCC_AHBENR_DMA1EN                   ((uint32_t)0x01000000)        /*!< DMA1 clock enable */
#define  RCC_AHBENR_DMA2EN                   ((uint32_t)0x02000000)        /*!< DMA2 clock enable */
#define  RCC_AHBENR_AESEN                    ((uint32_t)0x08000000)        /*!< AES clock enable */
#define  RCC_AHBENR_FSMCEN                   ((uint32_t)0x40000000)        /*!< FSMC clock enable */


/******************  Bit definition for RCC_APB2ENR register  *****************/
#define  RCC_APB2ENR_SYSCFGEN                ((uint32_t)0x00000001)         /*!< System Configuration SYSCFG clock enable */
#define  RCC_APB2ENR_TIM9EN                  ((uint32_t)0x00000004)         /*!< TIM9 interface clock enable */
#define  RCC_APB2ENR_TIM10EN                 ((uint32_t)0x00000008)         /*!< TIM10 interface clock enable */
#define  RCC_APB2ENR_TIM11EN                 ((uint32_t)0x00000010)         /*!< TIM11 Timer clock enable */
#define  RCC_APB2ENR_ADC1EN                  ((uint32_t)0x00000200)         /*!< ADC1 clock enable */
#define  RCC_APB2ENR_SDIOEN                  ((uint32_t)0x00000800)         /*!< SDIO clock enable */
#define  RCC_APB2ENR_SPI1EN                  ((uint32_t)0x00001000)         /*!< SPI1 clock enable */
#define  RCC_APB2ENR_USART1EN                ((uint32_t)0x00004000)         /*!< USART1 clock enable */


/*****************  Bit definition for RCC_APB1ENR register  ******************/
#define  RCC_APB1ENR_TIM2EN                  ((uint32_t)0x00000001)        /*!< Timer 2 clock enabled*/
#define  RCC_APB1ENR_TIM3EN                  ((uint32_t)0x00000002)        /*!< Timer 3 clock enable */
#define  RCC_APB1ENR_TIM4EN                  ((uint32_t)0x00000004)        /*!< Timer 4 clock enable */
#define  RCC_APB1ENR_TIM5EN                  ((uint32_t)0x00000008)        /*!< Timer 5 clock enable */
#define  RCC_APB1ENR_TIM6EN                  ((uint32_t)0x00000010)        /*!< Timer 6 clock enable */
#define  RCC_APB1ENR_TIM7EN                  ((uint32_t)0x00000020)        /*!< Timer 7 clock enable */
#define  RCC_APB1ENR_LCDEN                   ((uint32_t)0x00000200)        /*!< LCD clock enable */
#define  RCC_APB1ENR_WWDGEN                  ((uint32_t)0x00000800)        /*!< Window Watchdog clock enable */
#define  RCC_APB1ENR_SPI2EN                  ((uint32_t)0x00004000)        /*!< SPI 2 clock enable */
#define  RCC_APB1ENR_SPI3EN                  ((uint32_t)0x00008000)        /*!< SPI 3 clock enable */
#define  RCC_APB1ENR_USART2EN                ((uint32_t)0x00020000)        /*!< USART 2 clock enable */
#define  RCC_APB1ENR_USART3EN                ((uint32_t)0x00040000)        /*!< USART 3 clock enable */
#define  RCC_APB1ENR_UART4EN                 ((uint32_t)0x00080000)        /*!< UART 4 clock enable */
#define  RCC_APB1ENR_UART5EN                 ((uint32_t)0x00100000)        /*!< UART 5 clock enable */
#define  RCC_APB1ENR_I2C1EN                  ((uint32_t)0x00200000)        /*!< I2C 1 clock enable */
#define  RCC_APB1ENR_I2C2EN                  ((uint32_t)0x00400000)        /*!< I2C 2 clock enable */
#define  RCC_APB1ENR_USBEN                   ((uint32_t)0x00800000)        /*!< USB clock enable */
#define  RCC_APB1ENR_PWREN                   ((uint32_t)0x10000000)        /*!< Power interface clock enable */
#define  RCC_APB1ENR_DACEN                   ((uint32_t)0x20000000)        /*!< DAC interface clock enable */
#define  RCC_APB1ENR_COMPEN                  ((uint32_t)0x80000000)        /*!< Comparator interface clock enable */

/******************  Bit definition for RCC_AHBLPENR register  ****************/
#define  RCC_AHBLPENR_GPIOALPEN              ((uint32_t)0x00000001)        /*!< GPIO port A clock enabled in sleep mode */
#define  RCC_AHBLPENR_GPIOBLPEN              ((uint32_t)0x00000002)        /*!< GPIO port B clock enabled in sleep mode */
#define  RCC_AHBLPENR_GPIOCLPEN              ((uint32_t)0x00000004)        /*!< GPIO port C clock enabled in sleep mode */
#define  RCC_AHBLPENR_GPIODLPEN              ((uint32_t)0x00000008)        /*!< GPIO port D clock enabled in sleep mode */
#define  RCC_AHBLPENR_GPIOELPEN              ((uint32_t)0x00000010)        /*!< GPIO port E clock enabled in sleep mode */
#define  RCC_AHBLPENR_GPIOHLPEN              ((uint32_t)0x00000020)        /*!< GPIO port H clock enabled in sleep mode */
#define  RCC_AHBLPENR_GPIOFLPEN              ((uint32_t)0x00000040)        /*!< GPIO port F clock enabled in sleep mode */
#define  RCC_AHBLPENR_GPIOGLPEN              ((uint32_t)0x00000080)        /*!< GPIO port G clock enabled in sleep mode */
#define  RCC_AHBLPENR_CRCLPEN                ((uint32_t)0x00001000)        /*!< CRC clock enabled in sleep mode */
#define  RCC_AHBLPENR_FLITFLPEN              ((uint32_t)0x00008000)        /*!< Flash Interface clock enabled in sleep mode
                                                                                (has effect only when the Flash memory is
                                                                                 in power down mode) */
#define  RCC_AHBLPENR_SRAMLPEN               ((uint32_t)0x00010000)        /*!< SRAM clock enabled in sleep mode */
#define  RCC_AHBLPENR_DMA1LPEN               ((uint32_t)0x01000000)        /*!< DMA1 clock enabled in sleep mode */
#define  RCC_AHBLPENR_DMA2LPEN               ((uint32_t)0x02000000)        /*!< DMA2 clock enabled in sleep mode */
#define  RCC_AHBLPENR_AESLPEN                ((uint32_t)0x08000000)        /*!< AES clock enabled in sleep mode */
#define  RCC_AHBLPENR_FSMCLPEN               ((uint32_t)0x40000000)        /*!< FSMC clock enabled in sleep mode */

/******************  Bit definition for RCC_APB2LPENR register  ***************/
#define  RCC_APB2LPENR_SYSCFGLPEN            ((uint32_t)0x00000001)         /*!< System Configuration SYSCFG clock enabled in sleep mode */
#define  RCC_APB2LPENR_TIM9LPEN              ((uint32_t)0x00000004)         /*!< TIM9 interface clock enabled in sleep mode */
#define  RCC_APB2LPENR_TIM10LPEN             ((uint32_t)0x00000008)         /*!< TIM10 interface clock enabled in sleep mode */
#define  RCC_APB2LPENR_TIM11LPEN             ((uint32_t)0x00000010)         /*!< TIM11 Timer clock enabled in sleep mode */
#define  RCC_APB2LPENR_ADC1LPEN              ((uint32_t)0x00000200)         /*!< ADC1 clock enabled in sleep mode */
#define  RCC_APB2LPENR_SDIOLPEN              ((uint32_t)0x00000800)         /*!< SDIO clock enabled in sleep mode */
#define  RCC_APB2LPENR_SPI1LPEN              ((uint32_t)0x00001000)         /*!< SPI1 clock enabled in sleep mode */
#define  RCC_APB2LPENR_USART1LPEN            ((uint32_t)0x00004000)         /*!< USART1 clock enabled in sleep mode */

/*****************  Bit definition for RCC_APB1LPENR register  ****************/
#define  RCC_APB1LPENR_TIM2LPEN              ((uint32_t)0x00000001)        /*!< Timer 2 clock enabled in sleep mode */
#define  RCC_APB1LPENR_TIM3LPEN              ((uint32_t)0x00000002)        /*!< Timer 3 clock enabled in sleep mode */
#define  RCC_APB1LPENR_TIM4LPEN              ((uint32_t)0x00000004)        /*!< Timer 4 clock enabled in sleep mode */
#define  RCC_APB1LPENR_TIM5LPEN              ((uint32_t)0x00000008)        /*!< Timer 5 clock enabled in sleep mode */
#define  RCC_APB1LPENR_TIM6LPEN              ((uint32_t)0x00000010)        /*!< Timer 6 clock enabled in sleep mode */
#define  RCC_APB1LPENR_TIM7LPEN              ((uint32_t)0x00000020)        /*!< Timer 7 clock enabled in sleep mode */
#define  RCC_APB1LPENR_LCDLPEN               ((uint32_t)0x00000200)        /*!< LCD clock enabled in sleep mode */
#define  RCC_APB1LPENR_WWDGLPEN              ((uint32_t)0x00000800)        /*!< Window Watchdog clock enabled in sleep mode */
#define  RCC_APB1LPENR_SPI2LPEN              ((uint32_t)0x00004000)        /*!< SPI 2 clock enabled in sleep mode */
#define  RCC_APB1LPENR_SPI3LPEN              ((uint32_t)0x00008000)        /*!< SPI 3 clock enabled in sleep mode */
#define  RCC_APB1LPENR_USART2LPEN            ((uint32_t)0x00020000)        /*!< USART 2 clock enabled in sleep mode */
#define  RCC_APB1LPENR_USART3LPEN            ((uint32_t)0x00040000)        /*!< USART 3 clock enabled in sleep mode */
#define  RCC_APB1LPENR_UART4LPEN             ((uint32_t)0x00080000)        /*!< UART 4 clock enabled in sleep mode */
#define  RCC_APB1LPENR_UART5LPEN             ((uint32_t)0x00100000)        /*!< UART 5 clock enabled in sleep mode */
#define  RCC_APB1LPENR_I2C1LPEN              ((uint32_t)0x00200000)        /*!< I2C 1 clock enabled in sleep mode */
#define  RCC_APB1LPENR_I2C2LPEN              ((uint32_t)0x00400000)        /*!< I2C 2 clock enabled in sleep mode */
#define  RCC_APB1LPENR_USBLPEN               ((uint32_t)0x00800000)        /*!< USB clock enabled in sleep mode */
#define  RCC_APB1LPENR_PWRLPEN               ((uint32_t)0x10000000)        /*!< Power interface clock enabled in sleep mode */
#define  RCC_APB1LPENR_DACLPEN               ((uint32_t)0x20000000)        /*!< DAC interface clock enabled in sleep mode */
#define  RCC_APB1LPENR_COMPLPEN              ((uint32_t)0x80000000)        /*!< Comparator interface clock enabled in sleep mode*/

/*******************  Bit definition for RCC_CSR register  ********************/
#define  RCC_CSR_LSION                      ((uint32_t)0x00000001)        /*!< Internal Low Speed oscillator enable */
#define  RCC_CSR_LSIRDY                     ((uint32_t)0x00000002)        /*!< Internal Low Speed oscillator Ready */

#define  RCC_CSR_LSEON                      ((uint32_t)0x00000100)        /*!< External Low Speed oscillator enable */
#define  RCC_CSR_LSERDY                     ((uint32_t)0x00000200)        /*!< External Low Speed oscillator Ready */
#define  RCC_CSR_LSEBYP                     ((uint32_t)0x00000400)        /*!< External Low Speed oscillator Bypass */
#define  RCC_CSR_LSECSSON                   ((uint32_t)0x00000800)        /*!< External Low Speed oscillator CSS Enable */
#define  RCC_CSR_LSECSSD                    ((uint32_t)0x00001000)        /*!< External Low Speed oscillator CSS Detected */

#define  RCC_CSR_RTCSEL                     ((uint32_t)0x00030000)        /*!< RTCSEL[1:0] bits (RTC clock source selection) */
#define  RCC_CSR_RTCSEL_0                   ((uint32_t)0x00010000)        /*!< Bit 0 */
#define  RCC_CSR_RTCSEL_1                   ((uint32_t)0x00020000)        /*!< Bit 1 */

#define STM32_RTCSEL_MASK       (3 << 16)   /**< RTC source mask.           */
#define STM32_RTCSEL_NOCLOCK    (0 << 16)   /**< No RTC source.             */
#define STM32_RTCSEL_LSE        (1 << 16)   /**< RTC source is LSE.         */
#define STM32_RTCSEL_LSI        (2 << 16)   /**< RTC source is LSI.         */
#define STM32_RTCSEL_HSEDIV     (3 << 16)   /**< RTC source is HSE divided. */

#else /* CPU_CM3_STM32F2 */

/******************************************************************************/
/*                                                                            */
/*                         Reset and Clock Control                            */
/*                                                                            */
/******************************************************************************/
/********************  Bit definition for RCC_CR register  ********************/
#define  RCC_CR_HSION                        ((uint32_t)0x00000001)
#define  RCC_CR_HSIRDY                       ((uint32_t)0x00000002)

#define  RCC_CR_HSITRIM                      ((uint32_t)0x000000F8)
#define  RCC_CR_HSITRIM_0                    ((uint32_t)0x00000008)/*!<Bit 0 */
#define  RCC_CR_HSITRIM_1                    ((uint32_t)0x00000010)/*!<Bit 1 */
#define  RCC_CR_HSITRIM_2                    ((uint32_t)0x00000020)/*!<Bit 2 */
#define  RCC_CR_HSITRIM_3                    ((uint32_t)0x00000040)/*!<Bit 3 */
#define  RCC_CR_HSITRIM_4                    ((uint32_t)0x00000080)/*!<Bit 4 */

#define  RCC_CR_HSICAL                       ((uint32_t)0x0000FF00)
#define  RCC_CR_HSICAL_0                     ((uint32_t)0x00000100)/*!<Bit 0 */
#define  RCC_CR_HSICAL_1                     ((uint32_t)0x00000200)/*!<Bit 1 */
#define  RCC_CR_HSICAL_2                     ((uint32_t)0x00000400)/*!<Bit 2 */
#define  RCC_CR_HSICAL_3                     ((uint32_t)0x00000800)/*!<Bit 3 */
#define  RCC_CR_HSICAL_4                     ((uint32_t)0x00001000)/*!<Bit 4 */
#define  RCC_CR_HSICAL_5                     ((uint32_t)0x00002000)/*!<Bit 5 */
#define  RCC_CR_HSICAL_6                     ((uint32_t)0x00004000)/*!<Bit 6 */
#define  RCC_CR_HSICAL_7                     ((uint32_t)0x00008000)/*!<Bit 7 */

#define  RCC_CR_HSEON                        ((uint32_t)0x00010000)
#define  RCC_CR_HSERDY                       ((uint32_t)0x00020000)
#define  RCC_CR_HSEBYP                       ((uint32_t)0x00040000)
#define  RCC_CR_CSSON                        ((uint32_t)0x00080000)
#define  RCC_CR_PLLON                        ((uint32_t)0x01000000)
#define  RCC_CR_PLLRDY                       ((uint32_t)0x02000000)
#define  RCC_CR_PLLI2SON                     ((uint32_t)0x04000000)
#define  RCC_CR_PLLI2SRDY                    ((uint32_t)0x08000000)

/********************  Bit definition for RCC_PLLCFGR register  ***************/
#define  RCC_PLLCFGR_PLLM                    ((uint32_t)0x0000003F)
#define  RCC_PLLCFGR_PLLM_0                  ((uint32_t)0x00000001)
#define  RCC_PLLCFGR_PLLM_1                  ((uint32_t)0x00000002)
#define  RCC_PLLCFGR_PLLM_2                  ((uint32_t)0x00000004)
#define  RCC_PLLCFGR_PLLM_3                  ((uint32_t)0x00000008)
#define  RCC_PLLCFGR_PLLM_4                  ((uint32_t)0x00000010)
#define  RCC_PLLCFGR_PLLM_5                  ((uint32_t)0x00000020)

#define  RCC_PLLCFGR_PLLN                     ((uint32_t)0x00007FC0)
#define  RCC_PLLCFGR_PLLN_0                   ((uint32_t)0x00000040)
#define  RCC_PLLCFGR_PLLN_1                   ((uint32_t)0x00000080)
#define  RCC_PLLCFGR_PLLN_2                   ((uint32_t)0x00000100)
#define  RCC_PLLCFGR_PLLN_3                   ((uint32_t)0x00000200)
#define  RCC_PLLCFGR_PLLN_4                   ((uint32_t)0x00000400)
#define  RCC_PLLCFGR_PLLN_5                   ((uint32_t)0x00000800)
#define  RCC_PLLCFGR_PLLN_6                   ((uint32_t)0x00001000)
#define  RCC_PLLCFGR_PLLN_7                   ((uint32_t)0x00002000)
#define  RCC_PLLCFGR_PLLN_8                   ((uint32_t)0x00004000)

#define  RCC_PLLCFGR_PLLP                    ((uint32_t)0x00030000)
#define  RCC_PLLCFGR_PLLP_0                  ((uint32_t)0x00010000)
#define  RCC_PLLCFGR_PLLP_1                  ((uint32_t)0x00020000)

#define  RCC_PLLCFGR_PLLSRC                  ((uint32_t)0x00400000)
#define  RCC_PLLCFGR_PLLSRC_HSE              ((uint32_t)0x00400000)
#define  RCC_PLLCFGR_PLLSRC_HSI              ((uint32_t)0x00000000)

#define  RCC_PLLCFGR_PLLQ                    ((uint32_t)0x0F000000)
#define  RCC_PLLCFGR_PLLQ_0                  ((uint32_t)0x01000000)
#define  RCC_PLLCFGR_PLLQ_1                  ((uint32_t)0x02000000)
#define  RCC_PLLCFGR_PLLQ_2                  ((uint32_t)0x04000000)
#define  RCC_PLLCFGR_PLLQ_3                  ((uint32_t)0x08000000)

/********************  Bit definition for RCC_CFGR register  ******************/
/*!< SW configuration */
#define  RCC_CFGR_SW                         ((uint32_t)0x00000003)        /*!< SW[1:0] bits (System clock Switch) */
#define  RCC_CFGR_SW_0                       ((uint32_t)0x00000001)        /*!< Bit 0 */
#define  RCC_CFGR_SW_1                       ((uint32_t)0x00000002)        /*!< Bit 1 */

#define  RCC_CFGR_SW_HSI                     ((uint32_t)0x00000000)        /*!< HSI selected as system clock */
#define  RCC_CFGR_SW_HSE                     ((uint32_t)0x00000001)        /*!< HSE selected as system clock */
#define  RCC_CFGR_SW_PLL                     ((uint32_t)0x00000002)        /*!< PLL selected as system clock */

/*!< SWS configuration */
#define  RCC_CFGR_SWS                        ((uint32_t)0x0000000C)        /*!< SWS[1:0] bits (System Clock Switch Status) */
#define  RCC_CFGR_SWS_0                      ((uint32_t)0x00000004)        /*!< Bit 0 */
#define  RCC_CFGR_SWS_1                      ((uint32_t)0x00000008)        /*!< Bit 1 */

#define  RCC_CFGR_SWS_HSI                    ((uint32_t)0x00000000)        /*!< HSI oscillator used as system clock */
#define  RCC_CFGR_SWS_HSE                    ((uint32_t)0x00000004)        /*!< HSE oscillator used as system clock */
#define  RCC_CFGR_SWS_PLL                    ((uint32_t)0x00000008)        /*!< PLL used as system clock */

/*!< HPRE configuration */
#define  RCC_CFGR_HPRE                       ((uint32_t)0x000000F0)        /*!< HPRE[3:0] bits (AHB prescaler) */
#define  RCC_CFGR_HPRE_0                     ((uint32_t)0x00000010)        /*!< Bit 0 */
#define  RCC_CFGR_HPRE_1                     ((uint32_t)0x00000020)        /*!< Bit 1 */
#define  RCC_CFGR_HPRE_2                     ((uint32_t)0x00000040)        /*!< Bit 2 */
#define  RCC_CFGR_HPRE_3                     ((uint32_t)0x00000080)        /*!< Bit 3 */

#define  RCC_CFGR_HPRE_DIV1                  ((uint32_t)0x00000000)        /*!< SYSCLK not divided */
#define  RCC_CFGR_HPRE_DIV2                  ((uint32_t)0x00000080)        /*!< SYSCLK divided by 2 */
#define  RCC_CFGR_HPRE_DIV4                  ((uint32_t)0x00000090)        /*!< SYSCLK divided by 4 */
#define  RCC_CFGR_HPRE_DIV8                  ((uint32_t)0x000000A0)        /*!< SYSCLK divided by 8 */
#define  RCC_CFGR_HPRE_DIV16                 ((uint32_t)0x000000B0)        /*!< SYSCLK divided by 16 */
#define  RCC_CFGR_HPRE_DIV64                 ((uint32_t)0x000000C0)        /*!< SYSCLK divided by 64 */
#define  RCC_CFGR_HPRE_DIV128                ((uint32_t)0x000000D0)        /*!< SYSCLK divided by 128 */
#define  RCC_CFGR_HPRE_DIV256                ((uint32_t)0x000000E0)        /*!< SYSCLK divided by 256 */
#define  RCC_CFGR_HPRE_DIV512                ((uint32_t)0x000000F0)        /*!< SYSCLK divided by 512 */

/*!< PPRE1 configuration */
#define  RCC_CFGR_PPRE1                      ((uint32_t)0x00001C00)        /*!< PRE1[2:0] bits (APB1 prescaler) */
#define  RCC_CFGR_PPRE1_0                    ((uint32_t)0x00000400)        /*!< Bit 0 */
#define  RCC_CFGR_PPRE1_1                    ((uint32_t)0x00000800)        /*!< Bit 1 */
#define  RCC_CFGR_PPRE1_2                    ((uint32_t)0x00001000)        /*!< Bit 2 */

#define  RCC_CFGR_PPRE1_DIV1                 ((uint32_t)0x00000000)        /*!< HCLK not divided */
#define  RCC_CFGR_PPRE1_DIV2                 ((uint32_t)0x00001000)        /*!< HCLK divided by 2 */
#define  RCC_CFGR_PPRE1_DIV4                 ((uint32_t)0x00001400)        /*!< HCLK divided by 4 */
#define  RCC_CFGR_PPRE1_DIV8                 ((uint32_t)0x00001800)        /*!< HCLK divided by 8 */
#define  RCC_CFGR_PPRE1_DIV16                ((uint32_t)0x00001C00)        /*!< HCLK divided by 16 */

/*!< PPRE2 configuration */
#define  RCC_CFGR_PPRE2                      ((uint32_t)0x0000E000)        /*!< PRE2[2:0] bits (APB2 prescaler) */
#define  RCC_CFGR_PPRE2_0                    ((uint32_t)0x00002000)        /*!< Bit 0 */
#define  RCC_CFGR_PPRE2_1                    ((uint32_t)0x00004000)        /*!< Bit 1 */
#define  RCC_CFGR_PPRE2_2                    ((uint32_t)0x00008000)        /*!< Bit 2 */

#define  RCC_CFGR_PPRE2_DIV1                 ((uint32_t)0x00000000)        /*!< HCLK not divided */
#define  RCC_CFGR_PPRE2_DIV2                 ((uint32_t)0x00008000)        /*!< HCLK divided by 2 */
#define  RCC_CFGR_PPRE2_DIV4                 ((uint32_t)0x0000A000)        /*!< HCLK divided by 4 */
#define  RCC_CFGR_PPRE2_DIV8                 ((uint32_t)0x0000C000)        /*!< HCLK divided by 8 */
#define  RCC_CFGR_PPRE2_DIV16                ((uint32_t)0x0000E000)        /*!< HCLK divided by 16 */

/*!< RTCPRE configuration */
#define  RCC_CFGR_RTCPRE                     ((uint32_t)0x001F0000)
#define  RCC_CFGR_RTCPRE_0                   ((uint32_t)0x00010000)
#define  RCC_CFGR_RTCPRE_1                   ((uint32_t)0x00020000)
#define  RCC_CFGR_RTCPRE_2                   ((uint32_t)0x00040000)
#define  RCC_CFGR_RTCPRE_3                   ((uint32_t)0x00080000)
#define  RCC_CFGR_RTCPRE_4                   ((uint32_t)0x00100000)

/*!< MCO1 configuration */
#define  RCC_CFGR_MCO1                       ((uint32_t)0x00600000)
#define  RCC_CFGR_MCO1_0                     ((uint32_t)0x00200000)
#define  RCC_CFGR_MCO1_1                     ((uint32_t)0x00400000)

#define  RCC_CFGR_MCO1_SRC_HSE               (0x2 << 21)
#define  RCC_CFGR_MCO1_SRC_MASK              (0x3 << 21)

#define  RCC_CFGR_MCO_DIV_1                  (0x0 << 24)
#define  RCC_CFGR_MCO_DIV_MASK               (0x7 << 24)

#define  RCC_CFGR_I2SSRC                     ((uint32_t)0x00800000)

#define  RCC_CFGR_MCO1PRE                    ((uint32_t)0x07000000)
#define  RCC_CFGR_MCO1PRE_0                  ((uint32_t)0x01000000)
#define  RCC_CFGR_MCO1PRE_1                  ((uint32_t)0x02000000)
#define  RCC_CFGR_MCO1PRE_2                  ((uint32_t)0x04000000)

#define  RCC_CFGR_MCO2PRE                    ((uint32_t)0x38000000)
#define  RCC_CFGR_MCO2PRE_0                  ((uint32_t)0x08000000)
#define  RCC_CFGR_MCO2PRE_1                  ((uint32_t)0x10000000)
#define  RCC_CFGR_MCO2PRE_2                  ((uint32_t)0x20000000)

#define  RCC_CFGR_MCO2                       ((uint32_t)0xC0000000)
#define  RCC_CFGR_MCO2_0                     ((uint32_t)0x40000000)
#define  RCC_CFGR_MCO2_1                     ((uint32_t)0x80000000)

/********************  Bit definition for RCC_CIR register  *******************/
#define  RCC_CIR_LSIRDYF                     ((uint32_t)0x00000001)
#define  RCC_CIR_LSERDYF                     ((uint32_t)0x00000002)
#define  RCC_CIR_HSIRDYF                     ((uint32_t)0x00000004)
#define  RCC_CIR_HSERDYF                     ((uint32_t)0x00000008)
#define  RCC_CIR_PLLRDYF                     ((uint32_t)0x00000010)
#define  RCC_CIR_PLLI2SRDYF                  ((uint32_t)0x00000020)
#define  RCC_CIR_CSSF                        ((uint32_t)0x00000080)
#define  RCC_CIR_LSIRDYIE                    ((uint32_t)0x00000100)
#define  RCC_CIR_LSERDYIE                    ((uint32_t)0x00000200)
#define  RCC_CIR_HSIRDYIE                    ((uint32_t)0x00000400)
#define  RCC_CIR_HSERDYIE                    ((uint32_t)0x00000800)
#define  RCC_CIR_PLLRDYIE                    ((uint32_t)0x00001000)
#define  RCC_CIR_PLLI2SRDYIE                 ((uint32_t)0x00002000)
#define  RCC_CIR_LSIRDYC                     ((uint32_t)0x00010000)
#define  RCC_CIR_LSERDYC                     ((uint32_t)0x00020000)
#define  RCC_CIR_HSIRDYC                     ((uint32_t)0x00040000)
#define  RCC_CIR_HSERDYC                     ((uint32_t)0x00080000)
#define  RCC_CIR_PLLRDYC                     ((uint32_t)0x00100000)
#define  RCC_CIR_PLLI2SRDYC                  ((uint32_t)0x00200000)
#define  RCC_CIR_CSSC                        ((uint32_t)0x00800000)

/********************  Bit definition for RCC_AHB1RSTR register  **************/
#define  RCC_AHB1RSTR_GPIOARST               ((uint32_t)0x00000001)
#define  RCC_AHB1RSTR_GPIOBRST               ((uint32_t)0x00000002)
#define  RCC_AHB1RSTR_GPIOCRST               ((uint32_t)0x00000004)
#define  RCC_AHB1RSTR_GPIODRST               ((uint32_t)0x00000008)
#define  RCC_AHB1RSTR_GPIOERST               ((uint32_t)0x00000010)
#define  RCC_AHB1RSTR_GPIOFRST               ((uint32_t)0x00000020)
#define  RCC_AHB1RSTR_GPIOGRST               ((uint32_t)0x00000040)
#define  RCC_AHB1RSTR_GPIOHRST               ((uint32_t)0x00000080)
#define  RCC_AHB1RSTR_GPIOIRST               ((uint32_t)0x00000100)
#define  RCC_AHB1RSTR_CRCRST                 ((uint32_t)0x00001000)
#define  RCC_AHB1RSTR_DMA1RST                ((uint32_t)0x00200000)
#define  RCC_AHB1RSTR_DMA2RST                ((uint32_t)0x00400000)
#define  RCC_AHB1RSTR_ETHMACRST              ((uint32_t)0x02000000)
#define  RCC_AHB1RSTR_OTGHRST                ((uint32_t)0x10000000)

/********************  Bit definition for RCC_AHB2RSTR register  **************/
#define  RCC_AHB2RSTR_DCMIRST                ((uint32_t)0x00000001)
#define  RCC_AHB2RSTR_CRYPRST                ((uint32_t)0x00000010)
#define  RCC_AHB2RSTR_HASHRST                ((uint32_t)0x00000020)
 /* maintained for legacy purpose */
 #define  RCC_AHB2RSTR_HSAHRST                RCC_AHB2RSTR_HASHRST
#define  RCC_AHB2RSTR_RNGRST                 ((uint32_t)0x00000040)
#define  RCC_AHB2RSTR_OTGFSRST               ((uint32_t)0x00000080)

/********************  Bit definition for RCC_AHB3RSTR register  **************/
#define  RCC_AHB3RSTR_FSMCRST                ((uint32_t)0x00000001)

/********************  Bit definition for RCC_APB1RSTR register  **************/
#define  RCC_APB1RSTR_TIM2RST                ((uint32_t)0x00000001)
#define  RCC_APB1RSTR_TIM3RST                ((uint32_t)0x00000002)
#define  RCC_APB1RSTR_TIM4RST                ((uint32_t)0x00000004)
#define  RCC_APB1RSTR_TIM5RST                ((uint32_t)0x00000008)
#define  RCC_APB1RSTR_TIM6RST                ((uint32_t)0x00000010)
#define  RCC_APB1RSTR_TIM7RST                ((uint32_t)0x00000020)
#define  RCC_APB1RSTR_TIM12RST               ((uint32_t)0x00000040)
#define  RCC_APB1RSTR_TIM13RST               ((uint32_t)0x00000080)
#define  RCC_APB1RSTR_TIM14RST               ((uint32_t)0x00000100)
#define  RCC_APB1RSTR_WWDGEN                 ((uint32_t)0x00000800)
#define  RCC_APB1RSTR_SPI2RST                ((uint32_t)0x00008000)
#define  RCC_APB1RSTR_SPI3RST                ((uint32_t)0x00010000)
#define  RCC_APB1RSTR_USART2RST              ((uint32_t)0x00020000)
#define  RCC_APB1RSTR_USART3RST              ((uint32_t)0x00040000)
#define  RCC_APB1RSTR_UART4RST               ((uint32_t)0x00080000)
#define  RCC_APB1RSTR_UART5RST               ((uint32_t)0x00100000)
#define  RCC_APB1RSTR_I2C1RST                ((uint32_t)0x00200000)
#define  RCC_APB1RSTR_I2C2RST                ((uint32_t)0x00400000)
#define  RCC_APB1RSTR_I2C3RST                ((uint32_t)0x00800000)
#define  RCC_APB1RSTR_CAN1RST                ((uint32_t)0x02000000)
#define  RCC_APB1RSTR_CAN2RST                ((uint32_t)0x04000000)
#define  RCC_APB1RSTR_PWRRST                 ((uint32_t)0x10000000)
#define  RCC_APB1RSTR_DACRST                 ((uint32_t)0x20000000)

/********************  Bit definition for RCC_APB2RSTR register  **************/
#define  RCC_APB2RSTR_TIM1RST                ((uint32_t)0x00000001)
#define  RCC_APB2RSTR_TIM8RST                ((uint32_t)0x00000002)
#define  RCC_APB2RSTR_USART1RST              ((uint32_t)0x00000010)
#define  RCC_APB2RSTR_USART6RST              ((uint32_t)0x00000020)
#define  RCC_APB2RSTR_ADCRST                 ((uint32_t)0x00000100)
#define  RCC_APB2RSTR_SDIORST                ((uint32_t)0x00000800)
#define  RCC_APB2RSTR_SPI1RST                ((uint32_t)0x00001000)
#define  RCC_APB2RSTR_SYSCFGRST              ((uint32_t)0x00004000)
#define  RCC_APB2RSTR_TIM9RST                ((uint32_t)0x00010000)
#define  RCC_APB2RSTR_TIM10RST               ((uint32_t)0x00020000)
#define  RCC_APB2RSTR_TIM11RST               ((uint32_t)0x00040000)
/* Old SPI1RST bit definition, maintained for legacy purpose */
#define  RCC_APB2RSTR_SPI1                   RCC_APB2RSTR_SPI1RST

/********************  Bit definition for RCC_AHB1ENR register  ***************/
#define  RCC_AHB1ENR_GPIOAEN                 ((uint32_t)0x00000001)
#define  RCC_AHB1ENR_GPIOBEN                 ((uint32_t)0x00000002)
#define  RCC_AHB1ENR_GPIOCEN                 ((uint32_t)0x00000004)
#define  RCC_AHB1ENR_GPIODEN                 ((uint32_t)0x00000008)
#define  RCC_AHB1ENR_GPIOEEN                 ((uint32_t)0x00000010)
#define  RCC_AHB1ENR_GPIOFEN                 ((uint32_t)0x00000020)
#define  RCC_AHB1ENR_GPIOGEN                 ((uint32_t)0x00000040)
#define  RCC_AHB1ENR_GPIOHEN                 ((uint32_t)0x00000080)
#define  RCC_AHB1ENR_GPIOIEN                 ((uint32_t)0x00000100)
#define  RCC_AHB1ENR_CRCEN                   ((uint32_t)0x00001000)
#define  RCC_AHB1ENR_BKPSRAMEN               ((uint32_t)0x00040000)
#define  RCC_AHB1ENR_DMA1EN                  ((uint32_t)0x00200000)
#define  RCC_AHB1ENR_DMA2EN                  ((uint32_t)0x00400000)
#define  RCC_AHB1ENR_ETHMACEN                ((uint32_t)0x02000000)
#define  RCC_AHB1ENR_ETHMACTXEN              ((uint32_t)0x04000000)
#define  RCC_AHB1ENR_ETHMACRXEN              ((uint32_t)0x08000000)
#define  RCC_AHB1ENR_ETHMACPTPEN             ((uint32_t)0x10000000)
#define  RCC_AHB1ENR_OTGHSEN                 ((uint32_t)0x20000000)
#define  RCC_AHB1ENR_OTGHSULPIEN             ((uint32_t)0x40000000)

/********************  Bit definition for RCC_AHB2ENR register  ***************/
#define  RCC_AHB2ENR_DCMIEN                  ((uint32_t)0x00000001)
#define  RCC_AHB2ENR_CRYPEN                  ((uint32_t)0x00000010)
#define  RCC_AHB2ENR_HASHEN                  ((uint32_t)0x00000020)
#define  RCC_AHB2ENR_RNGEN                   ((uint32_t)0x00000040)
#define  RCC_AHB2ENR_OTGFSEN                 ((uint32_t)0x00000080)

/********************  Bit definition for RCC_AHB3ENR register  ***************/
#define  RCC_AHB3ENR_FSMCEN                  ((uint32_t)0x00000001)

/********************  Bit definition for RCC_APB1ENR register  ***************/
#define  RCC_APB1ENR_TIM2EN                  ((uint32_t)0x00000001)
#define  RCC_APB1ENR_TIM3EN                  ((uint32_t)0x00000002)
#define  RCC_APB1ENR_TIM4EN                  ((uint32_t)0x00000004)
#define  RCC_APB1ENR_TIM5EN                  ((uint32_t)0x00000008)
#define  RCC_APB1ENR_TIM6EN                  ((uint32_t)0x00000010)
#define  RCC_APB1ENR_TIM7EN                  ((uint32_t)0x00000020)
#define  RCC_APB1ENR_TIM12EN                 ((uint32_t)0x00000040)
#define  RCC_APB1ENR_TIM13EN                 ((uint32_t)0x00000080)
#define  RCC_APB1ENR_TIM14EN                 ((uint32_t)0x00000100)
#define  RCC_APB1ENR_WWDGEN                  ((uint32_t)0x00000800)
#define  RCC_APB1ENR_SPI2EN                  ((uint32_t)0x00004000)
#define  RCC_APB1ENR_SPI3EN                  ((uint32_t)0x00008000)
#define  RCC_APB1ENR_USART2EN                ((uint32_t)0x00020000)
#define  RCC_APB1ENR_USART3EN                ((uint32_t)0x00040000)
#define  RCC_APB1ENR_UART4EN                 ((uint32_t)0x00080000)
#define  RCC_APB1ENR_UART5EN                 ((uint32_t)0x00100000)
#define  RCC_APB1ENR_I2C1EN                  ((uint32_t)0x00200000)
#define  RCC_APB1ENR_I2C2EN                  ((uint32_t)0x00400000)
#define  RCC_APB1ENR_I2C3EN                  ((uint32_t)0x00800000)
#define  RCC_APB1ENR_CAN1EN                  ((uint32_t)0x02000000)
#define  RCC_APB1ENR_CAN2EN                  ((uint32_t)0x04000000)
#define  RCC_APB1ENR_PWREN                   ((uint32_t)0x10000000)
#define  RCC_APB1ENR_DACEN                   ((uint32_t)0x20000000)

/********************  Bit definition for RCC_APB2ENR register  ***************/
#define  RCC_APB2ENR_TIM1EN                  ((uint32_t)0x00000001)
#define  RCC_APB2ENR_TIM8EN                  ((uint32_t)0x00000002)
#define  RCC_APB2ENR_USART1EN                ((uint32_t)0x00000010)
#define  RCC_APB2ENR_USART6EN                ((uint32_t)0x00000020)
#define  RCC_APB2ENR_ADC1EN                  ((uint32_t)0x00000100)
#define  RCC_APB2ENR_ADC2EN                  ((uint32_t)0x00000200)
#define  RCC_APB2ENR_ADC3EN                  ((uint32_t)0x00000400)
#define  RCC_APB2ENR_SDIOEN                  ((uint32_t)0x00000800)
#define  RCC_APB2ENR_SPI1EN                  ((uint32_t)0x00001000)
#define  RCC_APB2ENR_SYSCFGEN                ((uint32_t)0x00004000)
#define  RCC_APB2ENR_TIM11EN                 ((uint32_t)0x00040000)
#define  RCC_APB2ENR_TIM10EN                 ((uint32_t)0x00020000)
#define  RCC_APB2ENR_TIM9EN                  ((uint32_t)0x00010000)

/********************  Bit definition for RCC_AHB1LPENR register  *************/
#define  RCC_AHB1LPENR_GPIOALPEN             ((uint32_t)0x00000001)
#define  RCC_AHB1LPENR_GPIOBLPEN             ((uint32_t)0x00000002)
#define  RCC_AHB1LPENR_GPIOCLPEN             ((uint32_t)0x00000004)
#define  RCC_AHB1LPENR_GPIODLPEN             ((uint32_t)0x00000008)
#define  RCC_AHB1LPENR_GPIOELPEN             ((uint32_t)0x00000010)
#define  RCC_AHB1LPENR_GPIOFLPEN             ((uint32_t)0x00000020)
#define  RCC_AHB1LPENR_GPIOGLPEN             ((uint32_t)0x00000040)
#define  RCC_AHB1LPENR_GPIOHLPEN             ((uint32_t)0x00000080)
#define  RCC_AHB1LPENR_GPIOILPEN             ((uint32_t)0x00000100)
#define  RCC_AHB1LPENR_CRCLPEN               ((uint32_t)0x00001000)
#define  RCC_AHB1LPENR_FLITFLPEN             ((uint32_t)0x00008000)
#define  RCC_AHB1LPENR_SRAM1LPEN             ((uint32_t)0x00010000)
#define  RCC_AHB1LPENR_SRAM2LPEN             ((uint32_t)0x00020000)
#define  RCC_AHB1LPENR_BKPSRAMLPEN           ((uint32_t)0x00040000)
#define  RCC_AHB1LPENR_DMA1LPEN              ((uint32_t)0x00200000)
#define  RCC_AHB1LPENR_DMA2LPEN              ((uint32_t)0x00400000)
#define  RCC_AHB1LPENR_ETHMACLPEN            ((uint32_t)0x02000000)
#define  RCC_AHB1LPENR_ETHMACTXLPEN          ((uint32_t)0x04000000)
#define  RCC_AHB1LPENR_ETHMACRXLPEN          ((uint32_t)0x08000000)
#define  RCC_AHB1LPENR_ETHMACPTPLPEN         ((uint32_t)0x10000000)
#define  RCC_AHB1LPENR_OTGHSLPEN             ((uint32_t)0x20000000)
#define  RCC_AHB1LPENR_OTGHSULPILPEN         ((uint32_t)0x40000000)

/********************  Bit definition for RCC_AHB2LPENR register  *************/
#define  RCC_AHB2LPENR_DCMILPEN              ((uint32_t)0x00000001)
#define  RCC_AHB2LPENR_CRYPLPEN              ((uint32_t)0x00000010)
#define  RCC_AHB2LPENR_HASHLPEN              ((uint32_t)0x00000020)
#define  RCC_AHB2LPENR_RNGLPEN               ((uint32_t)0x00000040)
#define  RCC_AHB2LPENR_OTGFSLPEN             ((uint32_t)0x00000080)

/********************  Bit definition for RCC_AHB3LPENR register  *************/
#define  RCC_AHB3LPENR_FSMCLPEN              ((uint32_t)0x00000001)

/********************  Bit definition for RCC_APB1LPENR register  *************/
#define  RCC_APB1LPENR_TIM2LPEN              ((uint32_t)0x00000001)
#define  RCC_APB1LPENR_TIM3LPEN              ((uint32_t)0x00000002)
#define  RCC_APB1LPENR_TIM4LPEN              ((uint32_t)0x00000004)
#define  RCC_APB1LPENR_TIM5LPEN              ((uint32_t)0x00000008)
#define  RCC_APB1LPENR_TIM6LPEN              ((uint32_t)0x00000010)
#define  RCC_APB1LPENR_TIM7LPEN              ((uint32_t)0x00000020)
#define  RCC_APB1LPENR_TIM12LPEN             ((uint32_t)0x00000040)
#define  RCC_APB1LPENR_TIM13LPEN             ((uint32_t)0x00000080)
#define  RCC_APB1LPENR_TIM14LPEN             ((uint32_t)0x00000100)
#define  RCC_APB1LPENR_WWDGLPEN              ((uint32_t)0x00000800)
#define  RCC_APB1LPENR_SPI2LPEN              ((uint32_t)0x00004000)
#define  RCC_APB1LPENR_SPI3LPEN              ((uint32_t)0x00008000)
#define  RCC_APB1LPENR_USART2LPEN            ((uint32_t)0x00020000)
#define  RCC_APB1LPENR_USART3LPEN            ((uint32_t)0x00040000)
#define  RCC_APB1LPENR_UART4LPEN             ((uint32_t)0x00080000)
#define  RCC_APB1LPENR_UART5LPEN             ((uint32_t)0x00100000)
#define  RCC_APB1LPENR_I2C1LPEN              ((uint32_t)0x00200000)
#define  RCC_APB1LPENR_I2C2LPEN              ((uint32_t)0x00400000)
#define  RCC_APB1LPENR_I2C3LPEN              ((uint32_t)0x00800000)
#define  RCC_APB1LPENR_CAN1LPEN              ((uint32_t)0x02000000)
#define  RCC_APB1LPENR_CAN2LPEN              ((uint32_t)0x04000000)
#define  RCC_APB1LPENR_PWRLPEN               ((uint32_t)0x10000000)
#define  RCC_APB1LPENR_DACLPEN               ((uint32_t)0x20000000)

/********************  Bit definition for RCC_APB2LPENR register  *************/
#define  RCC_APB2LPENR_TIM1LPEN              ((uint32_t)0x00000001)
#define  RCC_APB2LPENR_TIM8LPEN              ((uint32_t)0x00000002)
#define  RCC_APB2LPENR_USART1LPEN            ((uint32_t)0x00000010)
#define  RCC_APB2LPENR_USART6LPEN            ((uint32_t)0x00000020)
#define  RCC_APB2LPENR_ADC1LPEN              ((uint32_t)0x00000100)
#define  RCC_APB2LPENR_ADC2PEN               ((uint32_t)0x00000200)
#define  RCC_APB2LPENR_ADC3LPEN              ((uint32_t)0x00000400)
#define  RCC_APB2LPENR_SDIOLPEN              ((uint32_t)0x00000800)
#define  RCC_APB2LPENR_SPI1LPEN              ((uint32_t)0x00001000)
#define  RCC_APB2LPENR_SYSCFGLPEN            ((uint32_t)0x00004000)
#define  RCC_APB2LPENR_TIM9LPEN              ((uint32_t)0x00010000)
#define  RCC_APB2LPENR_TIM10LPEN             ((uint32_t)0x00020000)
#define  RCC_APB2LPENR_TIM11LPEN             ((uint32_t)0x00040000)

/********************  Bit definition for RCC_BDCR register  ******************/
#define  RCC_BDCR_LSEON                      ((uint32_t)0x00000001)
#define  RCC_BDCR_LSERDY                     ((uint32_t)0x00000002)
#define  RCC_BDCR_LSEBYP                     ((uint32_t)0x00000004)

#define  RCC_BDCR_RTCSEL                    ((uint32_t)0x00000300)
#define  RCC_BDCR_RTCSEL_0                  ((uint32_t)0x00000100)
#define  RCC_BDCR_RTCSEL_1                  ((uint32_t)0x00000200)

#define  RCC_BDCR_RTCEN                      ((uint32_t)0x00008000)
#define  RCC_BDCR_BDRST                      ((uint32_t)0x00010000)

/********************  Bit definition for RCC_CSR register  *******************/
#define  RCC_CSR_LSION                       ((uint32_t)0x00000001)
#define  RCC_CSR_LSIRDY                      ((uint32_t)0x00000002)
#define  RCC_CSR_RMVF                        ((uint32_t)0x01000000)
#define  RCC_CSR_BORRSTF                     ((uint32_t)0x02000000)
#define  RCC_CSR_PADRSTF                     ((uint32_t)0x04000000)
#define  RCC_CSR_PORRSTF                     ((uint32_t)0x08000000)
#define  RCC_CSR_SFTRSTF                     ((uint32_t)0x10000000)
#define  RCC_CSR_WDGRSTF                     ((uint32_t)0x20000000)
#define  RCC_CSR_WWDGRSTF                    ((uint32_t)0x40000000)
#define  RCC_CSR_LPWRRSTF                    ((uint32_t)0x80000000)

/********************  Bit definition for RCC_SSCGR register  *****************/
#define  RCC_SSCGR_MODPER                    ((uint32_t)0x00001FFF)
#define  RCC_SSCGR_INCSTEP                   ((uint32_t)0x0FFFE000)
#define  RCC_SSCGR_SPREADSEL                 ((uint32_t)0x40000000)
#define  RCC_SSCGR_SSCGEN                    ((uint32_t)0x80000000)

/********************  Bit definition for RCC_PLLI2SCFGR register  ************/
#define  RCC_PLLI2SCFGR_PLLI2SN              ((uint32_t)0x00007FC0)
#define  RCC_PLLI2SCFGR_PLLI2SR              ((uint32_t)0x70000000)



typedef struct RCC_TypeDef
{
  reg32_t CR;            /*!< RCC clock control register,                                  Address offset: 0x00 */
  reg32_t PLLCFGR;       /*!< RCC PLL configuration register,                              Address offset: 0x04 */
  reg32_t CFGR;          /*!< RCC clock configuration register,                            Address offset: 0x08 */
  reg32_t CIR;           /*!< RCC clock interrupt register,                                Address offset: 0x0C */
  reg32_t AHB1RSTR;      /*!< RCC AHB1 peripheral reset register,                          Address offset: 0x10 */
  reg32_t AHB2RSTR;      /*!< RCC AHB2 peripheral reset register,                          Address offset: 0x14 */
  reg32_t AHB3RSTR;      /*!< RCC AHB3 peripheral reset register,                          Address offset: 0x18 */
  reg32_t RESERVED0;     /*!< Reserved, 0x1C                                                                    */
  reg32_t APB1RSTR;      /*!< RCC APB1 peripheral reset register,                          Address offset: 0x20 */
  reg32_t APB2RSTR;      /*!< RCC APB2 peripheral reset register,                          Address offset: 0x24 */
  reg32_t RESERVED1[2];  /*!< Reserved, 0x28-0x2C                                                               */
  reg32_t AHB1ENR;       /*!< RCC AHB1 peripheral clock register,                          Address offset: 0x30 */
  reg32_t AHB2ENR;       /*!< RCC AHB2 peripheral clock register,                          Address offset: 0x34 */
  reg32_t AHB3ENR;       /*!< RCC AHB3 peripheral clock register,                          Address offset: 0x38 */
  reg32_t RESERVED2;     /*!< Reserved, 0x3C                                                                    */
  reg32_t APB1ENR;       /*!< RCC APB1 peripheral clock enable register,                   Address offset: 0x40 */
  reg32_t APB2ENR;       /*!< RCC APB2 peripheral clock enable register,                   Address offset: 0x44 */
  reg32_t RESERVED3[2];  /*!< Reserved, 0x48-0x4C                                                               */
  reg32_t AHB1LPENR;     /*!< RCC AHB1 peripheral clock enable in low power mode register, Address offset: 0x50 */
  reg32_t AHB2LPENR;     /*!< RCC AHB2 peripheral clock enable in low power mode register, Address offset: 0x54 */
  reg32_t AHB3LPENR;     /*!< RCC AHB3 peripheral clock enable in low power mode register, Address offset: 0x58 */
  reg32_t RESERVED4;     /*!< Reserved, 0x5C                                                                    */
  reg32_t APB1LPENR;     /*!< RCC APB1 peripheral clock enable in low power mode register, Address offset: 0x60 */
  reg32_t APB2LPENR;     /*!< RCC APB2 peripheral clock enable in low power mode register, Address offset: 0x64 */
  reg32_t RESERVED5[2];  /*!< Reserved, 0x68-0x6C                                                               */
  reg32_t BDCR;          /*!< RCC Backup domain control register,                          Address offset: 0x70 */
  reg32_t CSR;           /*!< RCC clock control & status register,                         Address offset: 0x74 */
  reg32_t RESERVED6[2];  /*!< Reserved, 0x78-0x7C                                                               */
  reg32_t SSCGR;         /*!< RCC spread spectrum clock generation register,               Address offset: 0x80 */
  reg32_t PLLI2SCFGR;    /*!< RCC PLLI2S configuration register,                           Address offset: 0x84 */
} RCC_TypeDef;

#define RCC  ((struct RCC_TypeDef *) RCC_BASE)

#define APB1_FREQ       (CPU_FREQ / 4)
#define APB2_FREQ       (CPU_FREQ / 2)

#define APB1_TIMER_FREQ ((APB1_FREQ == CPU_FREQ) ? APB1_FREQ : APB1_FREQ * 2)
#define APB2_TIMER_FREQ ((APB2_FREQ == CPU_FREQ) ? APB2_FREQ : APB2_FREQ * 2)

#define RCC_GPIO_ENABLE(gpio) (RCC->AHB1ENR |= BV(gpio))
#define RCC_GPIO_DISABLE(gpio) (RCC->AHB1ENR &= ~BV(gpio))

#endif

#endif /* STM32_RCC_H */

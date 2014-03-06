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
 * \brief STM32F2xx DMA definition.
 */

#ifndef STM32_DMA_H
#define STM32_DMA_H

#include <cpu/detect.h>

#if CPU_CM3_STM32F1
	#warning __FILTER_NEXT_WARNING__
	#warning Not supported
#elif CPU_CM3_STM32F2

struct stm32_dmastream
{
	reg32_t CR;     /*!< DMA stream x configuration register      */
	reg32_t NDTR;   /*!< DMA stream x number of data register     */
	reg32_t PAR;    /*!< DMA stream x peripheral address register */
	reg32_t M0AR;   /*!< DMA stream x memory 0 address register   */
	reg32_t M1AR;   /*!< DMA stream x memory 1 address register   */
	reg32_t FCR;    /*!< DMA stream x FIFO control register       */
};

struct stm32_dma
{
	reg32_t LISR;   /*!< DMA low interrupt status register,      Address offset: 0x00 */
	reg32_t HISR;   /*!< DMA high interrupt status register,     Address offset: 0x04 */
	reg32_t LIFCR;  /*!< DMA low interrupt flag clear register,  Address offset: 0x08 */
	reg32_t HIFCR;  /*!< DMA high interrupt flag clear register, Address offset: 0x0C */
	struct stm32_dmastream STR[8];
};

#define DMA1 ((struct stm32_dma *) DMA1_BASE)
#define DMA2 ((struct stm32_dma *) DMA2_BASE)

/* Bits definition for DMA_CR register **/
#define DMA_CR_CHSEL                         ((uint32_t)0x0E000000)
#define DMA_CR_CHSEL_0                       ((uint32_t)0x02000000)
#define DMA_CR_CHSEL_1                       ((uint32_t)0x04000000)
#define DMA_CR_CHSEL_2                       ((uint32_t)0x08000000)
#define DMA_CR_MBURST                        ((uint32_t)0x01800000)
#define DMA_CR_MBURST_0                      ((uint32_t)0x00800000)
#define DMA_CR_MBURST_1                      ((uint32_t)0x01000000)
#define DMA_CR_PBURST                        ((uint32_t)0x00600000)
#define DMA_CR_PBURST_0                      ((uint32_t)0x00200000)
#define DMA_CR_PBURST_1                      ((uint32_t)0x00400000)
#define DMA_CR_ACK                           ((uint32_t)0x00100000)
#define DMA_CR_TARGET_1                      ((uint32_t)0x00080000)
#define DMA_CR_DBUF                          ((uint32_t)0x00040000)
#define DMA_CR_PL                            ((uint32_t)0x00030000)
#define DMA_CR_PL_0                          ((uint32_t)0x00010000)
#define DMA_CR_PL_1                          ((uint32_t)0x00020000)
#define DMA_CR_PINCOS                        ((uint32_t)0x00008000)
#define DMA_CR_MSIZE                         ((uint32_t)0x00006000)
#define DMA_CR_MSIZE_0                       ((uint32_t)0x00002000)
#define DMA_CR_MSIZE_1                       ((uint32_t)0x00004000)
#define DMA_CR_PSIZE                         ((uint32_t)0x00001800)
#define DMA_CR_PSIZE_0                       ((uint32_t)0x00000800)
#define DMA_CR_PSIZE_1                       ((uint32_t)0x00001000)
#define DMA_CR_MINC                          ((uint32_t)0x00000400)
#define DMA_CR_PINC                          ((uint32_t)0x00000200)
#define DMA_CR_CIRC                          ((uint32_t)0x00000100)
#define DMA_CR_DIR                           ((uint32_t)0x000000C0)
#define DMA_CR_DIR_0                         ((uint32_t)0x00000040)
#define DMA_CR_DIR_1                         ((uint32_t)0x00000080)
#define DMA_CR_PFCTRL                        ((uint32_t)0x00000020)
#define DMA_CR_TCIE                          ((uint32_t)0x00000010)  /*<! Transfer complete interrupt */
#define DMA_CR_HTIE                          ((uint32_t)0x00000008)
#define DMA_CR_TEIE                          ((uint32_t)0x00000004)
#define DMA_CR_DMEIE                         ((uint32_t)0x00000002)
#define DMA_CR_EN                            ((uint32_t)0x00000001)

#define DMA_CR_CHAN(n)                       ((uint32_t)(n) << 25)

#define DMA_CR_PRI_LOW                       ((uint32_t)0 << 16)
#define DMA_CR_PRI_MED                       ((uint32_t)1 << 16)
#define DMA_CR_PRI_HIGH                      ((uint32_t)2 << 16)
#define DMA_CR_PRI_VHIGH                     ((uint32_t)3 << 16)

#define DMA_CR_PSIZE_8                       ((uint32_t)0 << 11)
#define DMA_CR_PSIZE_16                      ((uint32_t)1 << 11)
#define DMA_CR_PSIZE_32                      ((uint32_t)2 << 11)

#define DMA_CR_MSIZE_8                       ((uint32_t)0 << 13)
#define DMA_CR_MSIZE_16                      ((uint32_t)1 << 13)
#define DMA_CR_MSIZE_32                      ((uint32_t)2 << 13)

#define DMA_CR_DIR_P2M                       ((uint32_t)0 << 6)
#define DMA_CR_DIR_M2P                       ((uint32_t)1 << 6)
#define DMA_CR_DIR_M2M                       ((uint32_t)2 << 6)

/* Bits definition for DMA_CNDTR register */
#define DMA_NDT                              ((uint32_t)0x0000FFFF)
#define DMA_NDT_0                            ((uint32_t)0x00000001)
#define DMA_NDT_1                            ((uint32_t)0x00000002)
#define DMA_NDT_2                            ((uint32_t)0x00000004)
#define DMA_NDT_3                            ((uint32_t)0x00000008)
#define DMA_NDT_4                            ((uint32_t)0x00000010)
#define DMA_NDT_5                            ((uint32_t)0x00000020)
#define DMA_NDT_6                            ((uint32_t)0x00000040)
#define DMA_NDT_7                            ((uint32_t)0x00000080)
#define DMA_NDT_8                            ((uint32_t)0x00000100)
#define DMA_NDT_9                            ((uint32_t)0x00000200)
#define DMA_NDT_10                           ((uint32_t)0x00000400)
#define DMA_NDT_11                           ((uint32_t)0x00000800)
#define DMA_NDT_12                           ((uint32_t)0x00001000)
#define DMA_NDT_13                           ((uint32_t)0x00002000)
#define DMA_NDT_14                           ((uint32_t)0x00004000)
#define DMA_NDT_15                           ((uint32_t)0x00008000)

/* Bits definition for DMA_FCR register */
#define DMA_FCR_FEIE                         ((uint32_t)0x00000080)
#define DMA_FCR_FS                           ((uint32_t)0x00000038)
#define DMA_FCR_FS_0                         ((uint32_t)0x00000008)
#define DMA_FCR_FS_1                         ((uint32_t)0x00000010)
#define DMA_FCR_FS_2                         ((uint32_t)0x00000020)
#define DMA_FCR_DMDIS                        ((uint32_t)0x00000004)
#define DMA_FCR_FTH                          ((uint32_t)0x00000003)
#define DMA_FCR_FTH_0                        ((uint32_t)0x00000001)
#define DMA_FCR_FTH_1                        ((uint32_t)0x00000002)

/* Bits definition for DMA_LISR register **/
#define DMA_LISR_TCIF3                       ((uint32_t)0x08000000)
#define DMA_LISR_HTIF3                       ((uint32_t)0x04000000)
#define DMA_LISR_TEIF3                       ((uint32_t)0x02000000)
#define DMA_LISR_DMEIF3                      ((uint32_t)0x01000000)
#define DMA_LISR_FEIF3                       ((uint32_t)0x00400000)
#define DMA_LISR_TCIF2                       ((uint32_t)0x00200000)
#define DMA_LISR_HTIF2                       ((uint32_t)0x00100000)
#define DMA_LISR_TEIF2                       ((uint32_t)0x00080000)
#define DMA_LISR_DMEIF2                      ((uint32_t)0x00040000)
#define DMA_LISR_FEIF2                       ((uint32_t)0x00010000)
#define DMA_LISR_TCIF1                       ((uint32_t)0x00000800)
#define DMA_LISR_HTIF1                       ((uint32_t)0x00000400)
#define DMA_LISR_TEIF1                       ((uint32_t)0x00000200)
#define DMA_LISR_DMEIF1                      ((uint32_t)0x00000100)
#define DMA_LISR_FEIF1                       ((uint32_t)0x00000040)
#define DMA_LISR_TCIF0                       ((uint32_t)0x00000020)
#define DMA_LISR_HTIF0                       ((uint32_t)0x00000010)
#define DMA_LISR_TEIF0                       ((uint32_t)0x00000008)
#define DMA_LISR_DMEIF0                      ((uint32_t)0x00000004)
#define DMA_LISR_FEIF0                       ((uint32_t)0x00000001)

/* Bits definition for DMA_HISR register **/
#define DMA_HISR_TCIF7                       ((uint32_t)0x08000000)
#define DMA_HISR_HTIF7                       ((uint32_t)0x04000000)
#define DMA_HISR_TEIF7                       ((uint32_t)0x02000000)
#define DMA_HISR_DMEIF7                      ((uint32_t)0x01000000)
#define DMA_HISR_FEIF7                       ((uint32_t)0x00400000)
#define DMA_HISR_TCIF6                       ((uint32_t)0x00200000)
#define DMA_HISR_HTIF6                       ((uint32_t)0x00100000)
#define DMA_HISR_TEIF6                       ((uint32_t)0x00080000)
#define DMA_HISR_DMEIF6                      ((uint32_t)0x00040000)
#define DMA_HISR_FEIF6                       ((uint32_t)0x00010000)
#define DMA_HISR_TCIF5                       ((uint32_t)0x00000800)
#define DMA_HISR_HTIF5                       ((uint32_t)0x00000400)
#define DMA_HISR_TEIF5                       ((uint32_t)0x00000200)
#define DMA_HISR_DMEIF5                      ((uint32_t)0x00000100)
#define DMA_HISR_FEIF5                       ((uint32_t)0x00000040)
#define DMA_HISR_TCIF4                       ((uint32_t)0x00000020)
#define DMA_HISR_HTIF4                       ((uint32_t)0x00000010)
#define DMA_HISR_TEIF4                       ((uint32_t)0x00000008)
#define DMA_HISR_DMEIF4                      ((uint32_t)0x00000004)
#define DMA_HISR_FEIF4                       ((uint32_t)0x00000001)

/* Bits definition for DMA_LIFCR register */
#define DMA_LIFCR_CTCIF3                     ((uint32_t)0x08000000)
#define DMA_LIFCR_CHTIF3                     ((uint32_t)0x04000000)
#define DMA_LIFCR_CTEIF3                     ((uint32_t)0x02000000)
#define DMA_LIFCR_CDMEIF3                    ((uint32_t)0x01000000)
#define DMA_LIFCR_CFEIF3                     ((uint32_t)0x00400000)
#define DMA_LIFCR_CTCIF2                     ((uint32_t)0x00200000)
#define DMA_LIFCR_CHTIF2                     ((uint32_t)0x00100000)
#define DMA_LIFCR_CTEIF2                     ((uint32_t)0x00080000)
#define DMA_LIFCR_CDMEIF2                    ((uint32_t)0x00040000)
#define DMA_LIFCR_CFEIF2                     ((uint32_t)0x00010000)
#define DMA_LIFCR_CTCIF1                     ((uint32_t)0x00000800)
#define DMA_LIFCR_CHTIF1                     ((uint32_t)0x00000400)
#define DMA_LIFCR_CTEIF1                     ((uint32_t)0x00000200)
#define DMA_LIFCR_CDMEIF1                    ((uint32_t)0x00000100)
#define DMA_LIFCR_CFEIF1                     ((uint32_t)0x00000040)
#define DMA_LIFCR_CTCIF0                     ((uint32_t)0x00000020)
#define DMA_LIFCR_CHTIF0                     ((uint32_t)0x00000010)
#define DMA_LIFCR_CTEIF0                     ((uint32_t)0x00000008)
#define DMA_LIFCR_CDMEIF0                    ((uint32_t)0x00000004)
#define DMA_LIFCR_CFEIF0                     ((uint32_t)0x00000001)

/* Bits definition for DMA_HIFCR  register */
#define DMA_HIFCR_CTCIF7                     ((uint32_t)0x08000000)
#define DMA_HIFCR_CHTIF7                     ((uint32_t)0x04000000)
#define DMA_HIFCR_CTEIF7                     ((uint32_t)0x02000000)
#define DMA_HIFCR_CDMEIF7                    ((uint32_t)0x01000000)
#define DMA_HIFCR_CFEIF7                     ((uint32_t)0x00400000)
#define DMA_HIFCR_CTCIF6                     ((uint32_t)0x00200000)
#define DMA_HIFCR_CHTIF6                     ((uint32_t)0x00100000)
#define DMA_HIFCR_CTEIF6                     ((uint32_t)0x00080000)
#define DMA_HIFCR_CDMEIF6                    ((uint32_t)0x00040000)
#define DMA_HIFCR_CFEIF6                     ((uint32_t)0x00010000)
#define DMA_HIFCR_CTCIF5                     ((uint32_t)0x00000800)
#define DMA_HIFCR_CHTIF5                     ((uint32_t)0x00000400)
#define DMA_HIFCR_CTEIF5                     ((uint32_t)0x00000200)
#define DMA_HIFCR_CDMEIF5                    ((uint32_t)0x00000100)
#define DMA_HIFCR_CFEIF5                     ((uint32_t)0x00000040)
#define DMA_HIFCR_CTCIF4                     ((uint32_t)0x00000020)
#define DMA_HIFCR_CHTIF4                     ((uint32_t)0x00000010)
#define DMA_HIFCR_CTEIF4                     ((uint32_t)0x00000008)
#define DMA_HIFCR_CDMEIF4                    ((uint32_t)0x00000004)
#define DMA_HIFCR_CFEIF4                     ((uint32_t)0x00000001)

#define DMA_TCI_BIT(stream) \
	((stream & 0x3) == 0 ? BV(5) :     \
	 (stream & 0x3) == 1 ? BV(11) :    \
	 (stream & 0x3) == 2 ? BV(21) :    \
			       BV(27))     \

/* Clear transfer complete interrupt flag */
#define DMA_CLEAR_TCI(dma, stream)                                 \
	do                                                         \
	{                                                          \
		if (stream < 4)                                    \
			(dma)->LIFCR |= DMA_TCI_BIT(stream);       \
		else                                               \
			(dma)->HIFCR |= DMA_TCI_BIT(stream);       \
	} while(0)

#else
	#error Unknown CPU
#endif

#endif /* STM32_DMA_H */

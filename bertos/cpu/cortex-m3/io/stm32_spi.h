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
 * \brief STM32F2xx SPI definition.
 */

#ifndef STM32_SPI_H
#define STM32_SPI_H

#include <cpu/detect.h>

#if CPU_CM3_STM32F1
	#warning __FILTER_NEXT_WARNING__
	#warning Not supported
#elif CPU_CM3_STM32F2

struct stm32_spi
{
	reg16_t CR1;        /*!< SPI control register 1 (not used in I2S mode),      Address offset: 0x00 */
	uint16_t RESERVED0; /*!< Reserved, 0x02                                                           */
	reg16_t CR2;        /*!< SPI control register 2,                             Address offset: 0x04 */
	uint16_t RESERVED1; /*!< Reserved, 0x06                                                           */
	reg16_t SR;         /*!< SPI status register,                                Address offset: 0x08 */
	uint16_t RESERVED2; /*!< Reserved, 0x0A                                                           */
	reg16_t DR;         /*!< SPI data register,                                  Address offset: 0x0C */
	uint16_t RESERVED3; /*!< Reserved, 0x0E                                                           */
	reg16_t CRCPR;      /*!< SPI CRC polynomial register (not used in I2S mode), Address offset: 0x10 */
	uint16_t RESERVED4; /*!< Reserved, 0x12                                                           */
	reg16_t RXCRCR;     /*!< SPI RX CRC register (not used in I2S mode),         Address offset: 0x14 */
	uint16_t RESERVED5; /*!< Reserved, 0x16                                                           */
	reg16_t TXCRCR;     /*!< SPI TX CRC register (not used in I2S mode),         Address offset: 0x18 */
	uint16_t RESERVED6; /*!< Reserved, 0x1A                                                           */
	reg16_t I2SCFGR;    /*!< SPI_I2S configuration register,                     Address offset: 0x1C */
	uint16_t RESERVED7; /*!< Reserved, 0x1E                                                           */
	reg16_t I2SPR;      /*!< SPI_I2S prescaler register,                         Address offset: 0x20 */
	uint16_t RESERVED8; /*!< Reserved, 0x22                                                           */
};

#define SPI1 ((struct stm32_spi *) SPI1_BASE)
#define SPI2 ((struct stm32_spi *) SPI2_BASE)
#define SPI3 ((struct stm32_spi *) SPI3_BASE)

/* Bit definition for SPI_CR1 register */
#define SPI_CR1_CPHA                        ((uint16_t)0x0001)            /*!<Clock Phase */
#define SPI_CR1_CPOL                        ((uint16_t)0x0002)            /*!<Clock Polarity */
#define SPI_CR1_MSTR                        ((uint16_t)0x0004)            /*!<Master Selection */

#define SPI_CR1_CLK_DIV256                  ((uint16_t)7 << 3)
#define SPI_CR1_CLK_DIV128                  ((uint16_t)6 << 3)
#define SPI_CR1_CLK_DIV64                   ((uint16_t)5 << 3)
#define SPI_CR1_CLK_DIV32                   ((uint16_t)4 << 3)
#define SPI_CR1_CLK_DIV16                   ((uint16_t)3 << 3)
#define SPI_CR1_CLK_DIV8                    ((uint16_t)2 << 3)
#define SPI_CR1_CLK_DIV4                    ((uint16_t)1 << 3)
#define SPI_CR1_CLK_DIV2                    ((uint16_t)0 << 3)

#define SPI_CR1_BR                          ((uint16_t)0x0038)            /*!<BR[2:0] bits (Baud Rate Control) */
#define SPI_CR1_BR_0                        ((uint16_t)0x0008)            /*!<Bit 0 */
#define SPI_CR1_BR_1                        ((uint16_t)0x0010)            /*!<Bit 1 */
#define SPI_CR1_BR_2                        ((uint16_t)0x0020)            /*!<Bit 2 */

#define SPI_CR1_SPIEN                       ((uint16_t)0x0040)            /*!<SPI Enable */
#define SPI_CR1_LSBFIRST                    ((uint16_t)0x0080)            /*!<Frame Format */
#define SPI_CR1_SSI                         ((uint16_t)0x0100)            /*!<Internal slave select */
#define SPI_CR1_SSM                         ((uint16_t)0x0200)            /*!<Software slave management */
#define SPI_CR1_RXONLY                      ((uint16_t)0x0400)            /*!<Receive only */
#define SPI_CR1_DFF_16BIT                   ((uint16_t)0x0800)            /*!<Data Frame Format */
#define SPI_CR1_CRCNEXT                     ((uint16_t)0x1000)            /*!<Transmit CRC next */
#define SPI_CR1_CRCEN                       ((uint16_t)0x2000)            /*!<Hardware CRC calculation enable */
#define SPI_CR1_BIDIOE                      ((uint16_t)0x4000)            /*!<Output enable in bidirectional mode */
#define SPI_CR1_BIDIMODE                    ((uint16_t)0x8000)            /*!<Bidirectional data mode enable */

/* Bit definition for SPI_CR2 register */
#define SPI_CR2_RXDMAEN                     ((uint8_t)0x01)               /*!<Rx Buffer DMA Enable */
#define SPI_CR2_TXDMAEN                     ((uint8_t)0x02)               /*!<Tx Buffer DMA Enable */
#define SPI_CR2_SSOE                        ((uint8_t)0x04)               /*!<SS Output Enable */
#define SPI_CR2_TIFRAME                     ((uint8_t)0x10)               /*!<Enable SPI TI frames */
#define SPI_CR2_ERRIE                       ((uint8_t)0x20)               /*!<Error Interrupt Enable */
#define SPI_CR2_RXNEIE                      ((uint8_t)0x40)               /*!<RX buffer Not Empty Interrupt Enable */
#define SPI_CR2_TXEIE                       ((uint8_t)0x80)               /*!<Tx buffer Empty Interrupt Enable */

/* Bit definition for SPI_SR register */
#define SPI_SR_RXNE                         ((uint8_t)0x01)               /*!<Receive buffer Not Empty */
#define SPI_SR_TXE                          ((uint8_t)0x02)               /*!<Transmit buffer Empty */
#define SPI_SR_CHSIDE                       ((uint8_t)0x04)               /*!<Channel side */
#define SPI_SR_UDR                          ((uint8_t)0x08)               /*!<Underrun flag */
#define SPI_SR_CRCERR                       ((uint8_t)0x10)               /*!<CRC Error flag */
#define SPI_SR_MODF                         ((uint8_t)0x20)               /*!<Mode fault */
#define SPI_SR_OVR                          ((uint8_t)0x40)               /*!<Overrun flag */
#define SPI_SR_BSY                          ((uint8_t)0x80)               /*!<Busy flag */

/* Bit definition for SPI_DR register */
#define SPI_DR_DR                           ((uint16_t)0xFFFF)            /*!<Data Register */

/* Bit definition for SPI_CRCPR register */
#define SPI_CRCPR_CRCPOLY                   ((uint16_t)0xFFFF)            /*!<CRC polynomial register */

/* Bit definition for SPI_RXCRCR register */
#define SPI_RXCRCR_RXCRC                    ((uint16_t)0xFFFF)            /*!<Rx CRC Register */

/* Bit definition for SPI_TXCRCR register */
#define SPI_TXCRCR_TXCRC                    ((uint16_t)0xFFFF)            /*!<Tx CRC Register */

/* Bit definition for SPI_I2SCFGR register */
#define SPI_I2SCFGR_CHLEN_32BIT             ((uint16_t)0x0001)            /*!<Channel length (number of bits per audio channel) */

#define SPI_I2SCFGR_DATLEN_16BIT            ((uint16_t)0 << 1)
#define SPI_I2SCFGR_DATLEN_24BIT            ((uint16_t)1 << 1)
#define SPI_I2SCFGR_DATLEN_32BIT            ((uint16_t)2 << 1)

#define SPI_I2SCFGR_DATLEN                  ((uint16_t)0x0006)            /*!<DATLEN[1:0] bits (Data length to be transferred) */
#define SPI_I2SCFGR_DATLEN_0                ((uint16_t)0x0002)            /*!<Bit 0 */
#define SPI_I2SCFGR_DATLEN_1                ((uint16_t)0x0004)            /*!<Bit 1 */

#define SPI_I2SCFGR_CKPOL                   ((uint16_t)0x0008)            /*!<steady state clock polarity */

#define SPI_I2SCFGR_I2SSTD_PCM              ((uint16_t)3 << 4)
#define SPI_I2SCFGR_I2SSTD                  ((uint16_t)0x0030)            /*!<I2SSTD[1:0] bits (I2S standard selection) */
#define SPI_I2SCFGR_I2SSTD_0                ((uint16_t)0x0010)            /*!<Bit 0 */
#define SPI_I2SCFGR_I2SSTD_1                ((uint16_t)0x0020)            /*!<Bit 1 */

#define SPI_I2SCFGR_PCMSYNC_LONG            ((uint16_t)0x0080)            /*!<PCM frame synchronization */

#define SPI_I2SCFGR_I2SCFG_RXMASTER         ((uint16_t)3 << 8)
#define SPI_I2SCFGR_I2SCFG                  ((uint16_t)0x0300)            /*!<I2SCFG[1:0] bits (I2S configuration mode) */
#define SPI_I2SCFGR_I2SCFG_0                ((uint16_t)0x0100)            /*!<Bit 0 */
#define SPI_I2SCFGR_I2SCFG_1                ((uint16_t)0x0200)            /*!<Bit 1 */

#define SPI_I2SCFGR_I2SE                    ((uint16_t)0x0400)            /*!<I2S Enable */
#define SPI_I2SCFGR_I2SMODE                 ((uint16_t)0x0800)            /*!<I2S mode selection */

/* Bit definition for SPI_I2SPR register */
#define SPI_I2SPR_I2SDIV                    ((uint16_t)0x00FF)            /*!<I2S Linear prescaler */
#define SPI_I2SPR_ODD                       ((uint16_t)0x0100)            /*!<Odd factor for the prescaler */
#define SPI_I2SPR_MCKOE                     ((uint16_t)0x0200)            /*!<Master Clock Output Enable */

#define SPI_I2S_CLOCKMASK                   ((uint16_t)0x1ff)
#define SPI_I2S_CLOCKDIV(div)               ((((div) & 1) << 8) | ((div) >> 1))

#else
	#error Unknown CPU
#endif

#endif /* STM32_SPI_H */

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
 * \version $Id: at91_us.h 20354 2008-01-23 11:51:13Z asterix $
 *
 * \author Daniele Basile <asterix@develer.com>
 *
 * AT91 UART User interface.
 * This file is based on NUT/OS implementation. See license below.
 */
/*
 * Copyright (C) 2005-2006 by egnite Software GmbH. All rights reserved.
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

#ifndef AT91_US_H
#define AT91_US_H

/**
 * USART Control Register
 */
/*\{*/
#define US_CR_OFF               0x00000000      ///< USART control register offset.
#define US0_CR  (*((reg32_t *)(USART0_BASE + US_CR_OFF)))       ///< Channel 0 control register address.
#define US1_CR  (*((reg32_t *)(USART1_BASE + US_CR_OFF)))       ///< Channel 1 control register address.
#define US_RSTRX                         2      ///< Reset receiver.
#define US_RSTTX                         3      ///< Reset transmitter.
#define US_RXEN                          4      ///< Receiver enable.
#define US_RXDIS                         5      ///< Receiver disable.
#define US_TXEN                          6      ///< Transmitter enable.
#define US_TXDIS                         7      ///< Transmitter disable.
#define US_RSTSTA                        8      ///< Reset status bits.
#define US_STTBRK                        9      ///< Start break.
#define US_STPBRK                        10     ///< Stop break.
#define US_STTTO                         11     ///< Start timeout.
#define US_SENDA                         12     ///< Send next byte with address bit set.
#define US_RSTIT                         13     ///< Reset interations.
#define US_RSTNAK                        14     ///< Reset non acknowledge.
#define US_RETTO                         15     ///< Rearm time out.
#define US_DTREN                         16     ///< Data terminal ready enable.
#define US_DTRDIS                        17     ///< Data terminal ready disable.
#define US_RTSEN                         18     ///< Request to send enable.
#define US_RTSDIS                        19     ///< Request to send disable.
/*\}*/

/**
 * Mode Register
 */
/*\{*/
#define US_MR_OFF               0x00000004      ///< USART mode register offset.
#define US0_MR  (*((reg32_t *)(USART0_BASE + US_MR_OFF)))       ///< Channel 0 mode register address.
#define US1_MR  (*((reg32_t *)(USART1_BASE + US_MR_OFF)))       ///< Channel 1 mode register address.

#define US_USART_MODE_MASK      0x0000000F      ///< USART mode mask.
#define US_USART_MODE_NORMA     0x00000000      ///< Normal.
#define US_USART_MODE_RS485     0x00000001      ///< RS485.
#define US_USART_MODE_HW_HDSH   0x00000002      ///< Hardware handshaking.
#define US_USART_MODE_MODEM     0x00000003      ///< Modem.
#define US_USART_MODE_ISO7816T0 0x00000004      ///< ISO7816 protocol: T=0.
#define US_USART_MODE_ISO7816T1 0x00000006      ///< ISO7816 protocol: T=1.
#define US_USART_MODE_IRDA      0x00000008      ///< IrDA.

#define US_CLKS_MASK            0x00000030      ///< Clock selection mask.
#define US_CLKS_MCK             0x00000000      ///< Master clock.
#define US_CLKS_MCK8            0x00000010      ///< Master clock divided by 8.
#define US_CLKS_SCK             0x00000020      ///< External clock.
#define US_CLKS_SLCK            0x00000030      ///< Slow clock.

#define US_CHRL_MASK            0x000000C0      ///< Masks data length.
#define US_CHRL_5               0x00000000      ///< 5 data bits.
#define US_CHRL_6               0x00000040      ///< 6 data bits.
#define US_CHRL_7               0x00000080      ///< 7 data bits.
#define US_CHRL_8               0x000000C0      ///< 8 data bits.

#define US_SYNC                          8      ///< Synchronous mode enable.

#define US_PAR_MASK             0x00000E00      ///< Parity mode mask.
#define US_PAR_EVEN             0x00000000      ///< Even parity.
#define US_PAR_ODD              0x00000200      ///< Odd parity.
#define US_PAR_SPACE            0x00000400      ///< Space parity.
#define US_PAR_MARK             0x00000600      ///< Marked parity.
#define US_PAR_NO               0x00000800      ///< No parity.
#define US_PAR_MULTIDROP        0x00000C00      ///< Multi-drop mode.

#define US_NBSTOP_MASK          0x00003000      ///< Masks stop bit length.
#define US_NBSTOP_1             0x00000000      ///< 1 stop bit.
#define US_NBSTOP_1_5           0x00001000      ///< 1.5 stop bits.
#define US_NBSTOP_2             0x00002000      ///< 2 stop bits.

#define US_CHMODE_MASK              0x0000C000  ///< Channel mode mask.
#define US_CHMODE_NORMAL            0x00000000  ///< Normal mode.
#define US_CHMODE_AUTOMATIC_ECHO    0x00004000  ///< Automatic echo.
#define US_CHMODE_LOCAL_LOOPBACK    0x00008000  ///< Local loopback.
#define US_CHMODE_REMOTE_LOOPBACK   0x0000C000  ///< Remote loopback.

#define US_MSBF                         16      ///< Bit order.
#define US_MODE9                        17      ///< 9 bit mode.
#define US_CLKO                         18      ///< Clock output select.
#define US_OVER                         19      ///< Oversampling mode.
#define US_INACK                        20      ///< Inhibit non acknowledge.
#define US_DSNACK                       21      ///< Disable successive nack.

#define US_MAX_INTERATION_MASK      0x07000000  ///< Max numer of interation in mode ISO7816 T=0.

#define US_FILTER                       28      ///< Infrared receive line filter.

/*\}*/

/**
 * Status and Interrupt Register
 */
/*\{*/
#define US_IER_OFF              0x00000008      ///< USART interrupt enable register offset.
#define US0_IER (*((reg32_t *)(USART0_BASE + US_IER_OFF)))      ///< Channel 0 interrupt enable register address.
#define US1_IER (*((reg32_t *)(USART1_BASE + US_IER_OFF)))      ///< Channel 1 interrupt enable register address.

#define US_IDR_OFF              0x0000000C      ///< USART interrupt disable register offset.
#define US0_IDR (*((reg32_t *)(USART0_BASE + US_IDR_OFF)))      ///< Channel 0 interrupt disable register address.
#define US1_IDR (*((reg32_t *)(USART1_BASE + US_IDR_OFF)))      ///< Channel 1 interrupt disable register address.

#define US_IMR_OFF              0x00000010      ///< USART interrupt mask register offset.
#define US0_IMR (*((reg32_t *)(USART0_BASE + US_IMR_OFF)))      ///< Channel 0 interrupt mask register address.
#define US1_IMR (*((reg32_t *)(USART1_BASE + US_IMR_OFF)))      ///< Channel 1 interrupt mask register address.

#define US_CSR_OFF              0x00000014      ///< USART status register offset.
#define US0_CSR (*((reg32_t *)(USART0_BASE + US_CSR_OFF)))      ///< Channel 0 status register address.
#define US1_CSR (*((reg32_t *)(USART1_BASE + US_CSR_OFF)))      ///< Channel 1 status register address.
#define US_CSR_RI                       20      ///< Image of RI input.
#define US_CSR_DSR                      21      ///< Image of DSR input.
#define US_CSR_DCD                      22      ///< Image of DCD input.
#define US_CSR_CTS                      23      ///< Image of CTS input.

#define US_RXRDY                         0      ///< Receiver ready.
#define US_TXRDY                         1      ///< Transmitter ready.
#define US_RXBRK                         2      ///< Receiver break.
#define US_ENDRX                         3      ///< End of receiver PDC transfer.
#define US_ENDTX                         4      ///< End of transmitter PDC transfer.
#define US_OVRE                          5      ///< Overrun error.
#define US_FRAME                         6      ///< Framing error.
#define US_PARE                          7      ///< Parity error.
#define US_TIMEOUT                       8      ///< Receiver timeout.
#define US_TXEMPTY                       9      ///< Transmitter empty.
#define US_ITERATION                    10      ///< Iteration interrupt enable.
#define US_TXBUFE                       11      ///< Buffer empty interrupt enable.
#define US_RXBUFF                       12      ///< Buffer full interrupt enable.
#define US_NACK                         13      ///< Non acknowledge interrupt enable.
#define US_RIIC                         16      ///< Ring indicator input change enable.
#define US_DSRIC                        17      ///< Data set ready input change enable.
#define US_DCDIC                        18      ///< Data carrier detect input change interrupt enable.
#define US_CTSIC                        19      ///< Clear to send input change interrupt enable.

/**
 * Receiver Holding Register
 */
/*\{*/
#define US_RHR_OFF              0x00000018      ///< USART receiver holding register offset.
#define US0_RHR (*((reg32_t *)(USART0_BASE + US_RHR_OFF)))      ///< Channel 0 receiver holding register address.
#define US1_RHR (*((reg32_t *)(USART1_BASE + US_RHR_OFF)))      ///< Channel 1 receiver holding register address.
#define US_RHR_RXCHR_MASK       0x000001FF      ///< Last char received if US_RXRDY is set.
#define US_RHR_RXSYNH                   15      ///< Received sync.
/*\}*/

/**
 * Transmitter Holding Register
 */
/*\{*/
#define US_THR_OFF              0x0000001C      ///< USART transmitter holding register offset.
#define US0_THR (*((reg32_t *)(USART0_BASE + US_THR_OFF)))      ///< Channel 0 transmitter holding register address.
#define US1_THR (*((reg32_t *)(USART1_BASE + US_THR_OFF)))      ///< Channel 1 transmitter holding register address.
#define US_THR_TXCHR_MASK       0x000001FF      ///< Next char to be trasmitted.
#define US_THR_TXSYNH                   15      ///< Sync field to be trasmitted.
/*\}*/

/**
 * Baud Rate Generator Register
 */
/*\{*/
#define US_BRGR_OFF             0x00000020      ///< USART baud rate register offset.
#define US0_BRGR (*((reg32_t *)(USART0_BASE + US_BRGR_OFF)))    ///< Channel 0 baud rate register address.
#define US1_BRGR (*((reg32_t *)(USART1_BASE + US_BRGR_OFF)))    ///< Channel 1 baud rate register address.
/*\}*/

/**
 * Receiver Timeout Register
 */
/*\{*/
#define US_RTOR_OFF             0x00000024      ///< USART receiver timeout register offset.
#define US0_RTOR (*((reg32_t *)(USART0_BASE + US_RTOR_OFF)))    ///< Channel 0 receiver timeout register address.
#define US1_RTOR (*((reg32_t *)(USART1_BASE + US_RTOR_OFF)))    ///< Channel 1 receiver timeout register address.
/*\}*/

/**
 * Transmitter Time Guard Register
 */
/*\{*/
#define US_TTGR_OFF             0x00000028      ///< USART transmitter time guard register offset.
#define US0_TTGR (*((reg32_t *)(USART0_BASE + US_TTGR_OFF)))    ///< Channel 0 transmitter time guard register address.
#define US1_TTGR (*((reg32_t *)(USART1_BASE + US_TTGR_OFF)))    ///< Channel 1 transmitter time guard register address.
/*\}*/

/**
 * FI DI Ratio Register
*/
/*\{*/
#define US_FIDI_OFF             0x00000040      ///< USART FI DI ratio register offset.
#define US0_FIDI (*((reg32_t *)(USART0_BASE + US_FIDI_OFF)))    ///< Channel 0 FI DI ratio register address.
#define US1_FIDI (*((reg32_t *)(USART1_BASE + US_FIDI_OFF)))    ///< Channel 1 FI DI ratio register address.
/*\}*/

/**
 * Error Counter Register
 */
/*\{*/
#define US_NER_OFF              0x00000044      ///< USART error counter register offset.
#define US0_NER  (*((reg32_t *)(USART0_BASE + US_NER_OFF)))     ///< Channel 0 error counter register address.
#define US1_NER  (*((reg32_t *)(USART1_BASE + US_NER_OFF)))     ///< Channel 1 error counter register address.
/*\}*/

/**
 * IrDA Filter Register
 */
/*\{*/
#define US_IF_OFF               0x0000004C      ///< USART IrDA filter register offset.
#define US0_IF (*((reg32_t *)(USART0_BASE + US_IF_OFF)))        ///< Channel 0 IrDA filter register address.
#define US1_IF (*((reg32_t *)(USART1_BASE + US_IF_OFF)))        ///< Channel 1 IrDA filter register address.
/*\}*/

#if USART_HAS_PDC

	/**
	 * Receive Pointer Register
	 */
	/*\{*/
	#define US0_RPR (*((reg32_t *)(USART0_BASE + PERIPH_RPR_OFF)))      ///< Channel 0 receive pointer register address.
	#define US1_RPR (*((reg32_t *)(USART1_BASE + PERIPH_RPR_OFF)))      ///< Channel 1 receive pointer register address.
	/*\}*/

	/**
	 * Receive Counter Register
	 */
	/*\{*/
	#define US0_RCR (*((reg32_t *)(USART0_BASE + PERIPH_RCR_OFF)))      ///< Channel 0 receive counter register address.
	#define US1_RCR (*((reg32_t *)(USART1_BASE + PERIPH_RCR_OFF)))      ///< Channel 1 receive counter register address.
	/*\}*/

	/**
	* Transmit Pointer Register
	*/
	/*\{*/
	#define US0_TPR (*((reg32_t *)(USART0_BASE + PERIPH_TPR_OFF)))      ///< Channel 0 transmit pointer register address.
	#define US1_TPR (*((reg32_t *)(USART1_BASE + PERIPH_TPR_OFF)))      ///< Channel 1 transmit pointer register address.
	/*\}*/

	/**
	* Transmit Counter Register
	*/
	/*\{*/
	#define US0_TCR (*((reg32_t *)(USART0_BASE + PERIPH_TCR_OFF)))      ///< Channel 0 transmit counter register address.
	#define US1_TCR (*((reg32_t *)(USART1_BASE + PERIPH_TCR_OFF)))      ///< Channel 1 transmit counter register address.
	/*\}*/

	#if defined(PERIPH_RNPR_OFF) && defined(PERIPH_RNCR_OFF)
	#define US0_RNPR   (*((reg32_t *)(USART0_BASE + PERIPH_RNPR_OFF)))  ///< PDC channel 0 receive next pointer register.
	#define US1_RNPR   (*((reg32_t *)(USART1_BASE + PERIPH_RNPR_OFF)))  ///< PDC channel 1 receive next pointer register.
	#define US0_RNCR   (*((reg32_t *)(USART0_BASE + PERIPH_RNCR_OFF)))  ///< PDC channel 0 receive next counter register.
	#define US1_RNCR   (*((reg32_t *)(USART1_BASE + PERIPH_RNCR_OFF)))  ///< PDC channel 1 receive next counter register.
	#endif

	#if defined(PERIPH_TNPR_OFF) && defined(PERIPH_TNCR_OFF)
	#define US0_TNPR   (*((reg32_t *)(USART0_BASE + PERIPH_TNPR_OFF)))  ///< PDC channel 0 transmit next pointer register.
	#define US1_TNPR   (*((reg32_t *)(USART1_BASE + PERIPH_TNPR_OFF)))  ///< PDC channel 1 transmit next pointer register.
	#define US0_TNCR   (*((reg32_t *)(USART0_BASE + PERIPH_TNCR_OFF)))  ///< PDC channel 0 transmit next counter register.
	#define US1_TNCR   (*((reg32_t *)(USART1_BASE + PERIPH_TNCR_OFF)))  ///< PDC channel 1 transmit next counter register.
	#endif

	#if defined(PERIPH_PTCR_OFF)
	#define US0_PTCR   (*((reg32_t *)(USART0_BASE + PERIPH_PTCR_OFF)))  ///< PDC channel 0 transfer control register.
	#define US1_PTCR   (*((reg32_t *)(USART1_BASE + PERIPH_PTCR_OFF)))  ///< PDC channel 1 transfer control register.
	#endif

	#if defined(PERIPH_PTSR_OFF)
	#define US0_PTSR   (*((reg32_t *)(USART0_BASE + PERIPH_PTSR_OFF)))  ///< PDC channel 0 transfer status register.
	#define US1_PTSR   (*((reg32_t *)(USART1_BASE + PERIPH_PTSR_OFF)))  ///< PDC channel 1 transfer status register.
	#endif

#endif  /* USART_HAS_PDC */


/**
 * SPI Control Register
 * \{
 */
#define	SPI_CR_OFF	    0x00000000  ///< Control register offset.
#define SPI_CR          (*((reg32_t *)(SPI_BASE + SPI_CR_OFF))) ///< SPI control register.
#define SPI_SPIEN                0 ///< SPI enable.
#define SPI_SPIDIS               1 ///< SPI disable.
#define SPI_SWRST                7 ///< Software reset.
#define SPI_LASTXFER            24 ///< Last transfer.
/*\}*/

/**
 * SPI Mode Register
 * \{
 */
#define SPI_MR_OFF      0x00000004  ///< Mode register offset.
#define SPI_MR          (*((reg32_t *)(SPI_BASE + SPI_MR_OFF))) ///< SPI mode register.
#define SPI_MSTR                 0  ///< Master mode.
#define SPI_PS                   1  ///< Peripheral select.
#define SPI_PCSDEC               2  ///< Chip select decode.
#define SPI_MODFDIS              4  ///< Mode fault detection.
#define SPI_LLB                  7  ///< Local loopback enable.
#define SPI_PCS         0x000F0000  ///< Peripheral chip select mask.
#define SPI_PCS_0       0x000E0000  ///< Peripheral chip select 0.
#define SPI_PCS_1       0x000D0000  ///< Peripheral chip select 1.
#define SPI_PCS_2       0x000B0000  ///< Peripheral chip select 2.
#define SPI_PCS_3       0x00070000  ///< Peripheral chip select 3.
#define SPI_PCS_LSB             16  ///< Least significant bit of peripheral chip select.
#define SPI_DLYBCS      0xFF000000  ///< Mask for delay between chip selects.
#define SPI_DLYBCS_LSB          24  ///< Least significant bit of delay between chip selects.
/*\}*/

/**
 * SPI Receive Data Register
 * \{
 */
#define SPI_RDR_OFF     0x00000008  ///< Receive data register offset.
#define SPI_RDR         (*((reg32_t *)(SPI_BASE + SPI_RDR_OFF))) ///< SPI receive data register.
#define SPI_RD          0x0000FFFF  ///< Receive data mask.
#define SPI_RD_LSB              0   ///< Least significant bit of receive data.
/*\}*/

/**
 * SPI Transmit Data Register
 * \{
 */
#define SPI_TDR_OFF     0x0000000C  ///< Transmit data register offset.
#define SPI_TDR         (*((reg32_t *)(SPI_BASE + SPI_TDR_OFF))) ///< SPI transmit data register.
#define SPI_TD          0x0000FFFF  ///< Transmit data mask.
#define SPI_TD_LSB              0   ///< Least significant bit of transmit data.
/*\}*/

/**
 * SPI Status and Interrupt Register
 * \{
 */
#define SPI_SR_OFF      0x00000010  ///< Status register offset.
#define SPI_SR          (*((reg32_t *)(SPI_BASE + SPI_SR_OFF))) ///< Status register.
#define SPI_IER_OFF     0x00000014  ///< Interrupt enable register offset.
#define SPI_IER         (*((reg32_t *)(SPI_BASE + SPI_IER_OFF))) ///< Interrupt enable register.
#define SPI_IDR_OFF     0x00000018  ///< Interrupt disable register offset.
#define SPI_IDR         (*((reg32_t *)(SPI_BASE + SPI_IDR_OFF))) ///< Interrupt disable register.
#define SPI_IMR_OFF     0x0000001C  ///< Interrupt mask register offset.
#define SPI_IMR         (*((reg32_t *)(SPI_BASE + SPI_IMR_OFF))) ///< Interrupt mask register.

#define SPI_RDRF                 0  ///< Receive data register full.
#define SPI_TDRE                 1  ///< Transmit data register empty.
#define SPI_MODF                 2  ///< Mode fault error.
#define SPI_OVRES                3  ///< Overrun error status.
#define SPI_ENDRX                4  ///< End of RX buffer.
#define SPI_ENDTX                5  ///< End of TX buffer.
#define SPI_RXBUFF               6  ///< RX buffer full.
#define SPI_TXBUFE               7  ///< TX buffer empty.
#define SPI_NSSR                 8  ///< NSS rising.
#define SPI_TXEMPTY              9  ///< Transmission register empty.
#define SPI_SPIENS               16  ///< SPI enable status.
/*\}*/

/**
 * SPI Chip Select Registers
 * \{
 */
#define SPI_CSR0_OFF    0x00000030  ///< Chip select register 0 offset.
#define SPI_CS0         (*((reg32_t *)(SPI_BASE + SPI_CSR0_OFF))) ///<  Chip select register 0.
#define SPI_CSR1_OFF    0x00000034  ///< Chip select register 1 offset.
#define SPI_CS1         (*((reg32_t *)(SPI_BASE + SPI_CSR1_OFF))) ///<  Chip select register 1.
#define SPI_CSR2_OFF    0x00000038  ///< Chip select register 2 offset.
#define SPI_CS2         (*((reg32_t *)(SPI_BASE + SPI_CSR2_OFF))) ///<  Chip select register 2.
#define SPI_CSR3_OFF    0x0000003C  ///< Chip select register 3 offset.
#define SPI_CS3         (*((reg32_t *)(SPI_BASE + SPI_CSR3_OFF))) ///<  Chip select register 3.

#define SPI_CPOL                 0  ///< Clock polarity.
#define SPI_NCPHA                1  ///< Clock phase.
#define SPI_CSAAT                3  ///< Chip select active after transfer.
#define SPI_BITS        0x000000F0  ///< Bits per transfer mask.
#define SPI_BITS_8      0x00000000  ///< 8 bits per transfer.
#define SPI_BITS_9      0x00000010  ///< 9 bits per transfer.
#define SPI_BITS_10     0x00000020  ///< 10 bits per transfer.
#define SPI_BITS_11     0x00000030  ///< 11 bits per transfer.
#define SPI_BITS_12     0x00000040  ///< 12 bits per transfer.
#define SPI_BITS_13     0x00000050  ///< 13 bits per transfer.
#define SPI_BITS_14     0x00000060  ///< 14 bits per transfer.
#define SPI_BITS_15     0x00000070  ///< 15 bits per transfer.
#define SPI_BITS_16     0x00000080  ///< 16 bits per transfer.
#define SPI_BITS_LSB            4   ///< Least significant bit of bits per transfer.
#define SPI_SCBR        0x0000FF00  ///< Serial clock baud rate mask.
#define SPI_SCBR_LSB            8   ///< Least significant bit of serial clock baud rate.
#define SPI_DLYBS       0x00FF0000  ///< Delay before SPCK mask.
#define SPI_DLYBS_LSB           16  ///< Least significant bit of delay before SPCK.
#define SPI_DLYBCT      0xFF000000  ///< Delay between consecutive transfers mask.
#define SPI_DLYBCT_LSB          24  ///< Least significant bit of delay between consecutive transfers.
/*\}*/

#endif /* AT91_US_H */

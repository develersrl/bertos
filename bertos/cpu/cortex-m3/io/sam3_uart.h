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
 * \brief AT91SAM3 UART hardware.
 */

#ifndef SAM3_UART_H
#define SAM3_UART_H

/**
 * UART registers base addresses.
 */
/*\{*/
#define UART0_BASE  0x400E0600
#ifndef CPU_CM3_AT91SAM3U
	#define UART1_BASE  0x400E0800
#endif
/*\}*/

/**
 * UART register offsets.
 */
/*\{*/
#define UART_CR       0x000  //< Control Register
#define UART_MR       0x004  //< Mode Register
#define UART_IER      0x008  //< Interrupt Enable Register
#define UART_IDR      0x00C  //< Interrupt Disable Register
#define UART_IMR      0x010  //< Interrupt Mask Register
#define UART_SR       0x014  //< Status Register
#define UART_RHR      0x018  //< Receive Holding Register
#define UART_THR      0x01C  //< Transmit Holding Register
#define UART_BRGR     0x020  //< Baud Rate Generator Register

#define UART_RPR      0x100  //< Receive Pointer Register
#define UART_RCR      0x104  //< Receive Counter Register
#define UART_TPR      0x108  //< Transmit Pointer Register
#define UART_TCR      0x10C  //< Transmit Counter Register
#define UART_RNPR     0x110  //< Receive Next Pointer Register
#define UART_RNCR     0x114  //< Receive Next Counter Register
#define UART_TNPR     0x118  //< Transmit Next Pointer Register
#define UART_TNCR     0x11C  //< Transmit Next Counter Register
#define UART_PTCR     0x120  //< Transfer Control Register
#define UART_PTSR     0x124  //< Transfer Status Register
/*\}*/


/**
 * Bit fields in the UART_CR register.
 */
/*\{*/
#define UART_CR_RSTRX     BV(2)  //< Reset Receiver
#define UART_CR_RSTTX     BV(3)  //< Reset Transmitter
#define UART_CR_RXEN      BV(4)  //< Receiver Enable
#define UART_CR_RXDIS     BV(5)  //< Receiver Disable
#define UART_CR_TXEN      BV(6)  //< Transmitter Enable
#define UART_CR_TXDIS     BV(7)  //< Transmitter Disable
#define UART_CR_RSTSTA    BV(8)  //< Reset Status Bits
/*\}*/

/**
 * Bit fields in the UART_MR register.
 */
/*\{*/
#define UART_MR_PAR_S                    9                         //< Parity Type shift
#define UART_MR_PAR_M                    (0x7 << UART_MR_PAR_S)    //< Parity Type mask
#define   UART_MR_PAR_EVEN               (0x0 << UART_MR_PAR_S)    //< Even parity
#define   UART_MR_PAR_ODD                (0x1 << UART_MR_PAR_S)    //< Odd parity
#define   UART_MR_PAR_SPACE              (0x2 << UART_MR_PAR_S)    //< Space: parity forced to 0
#define   UART_MR_PAR_MARK               (0x3 << UART_MR_PAR_S)    //< Mark: parity forced to 1
#define   UART_MR_PAR_NO                 (0x4 << UART_MR_PAR_S)    //< No parity
#define UART_MR_CHMODE_S                 14                        //< Channel Mode shift
#define UART_MR_CHMODE_M                 (0x3 << UART_MR_CHMODE_S) //< Channel Mode mask
#define   UART_MR_CHMODE_NORMAL          (0x0 << UART_MR_CHMODE_S) //< Normal Mode
#define   UART_MR_CHMODE_AUTOMATIC       (0x1 << UART_MR_CHMODE_S) //< Automatic Echo
#define   UART_MR_CHMODE_LOCAL_LOOPBACK  (0x2 << UART_MR_CHMODE_S) //< Local Loopback
#define   UART_MR_CHMODE_REMOTE_LOOPBACK (0x3 << UART_MR_CHMODE_S) //< Remote Loopback
/*\}*/

/**
 * Bit fields in the UART_IER register.
 */
/*\{*/
#define UART_IER_RXRDY    BV(0)   //< Enable RXRDY Interrupt
#define UART_IER_TXRDY    BV(1)   //< Enable TXRDY Interrupt
#define UART_IER_ENDRX    BV(3)   //< Enable End of Receive Transfer Interrupt
#define UART_IER_ENDTX    BV(4)   //< Enable End of Transmit Interrupt
#define UART_IER_OVRE     BV(5)   //< Enable Overrun Error Interrupt
#define UART_IER_FRAME    BV(6)   //< Enable Framing Error Interrupt
#define UART_IER_PARE     BV(7)   //< Enable Parity Error Interrupt
#define UART_IER_TXEMPTY  BV(9)   //< Enable TXEMPTY Interrupt
#define UART_IER_TXBUFE   BV(11)  //< Enable Buffer Empty Interrupt
#define UART_IER_RXBUFF   BV(12)  //< Enable Buffer Full Interrupt
/*\}*/

/**
 * Bit fields in the UART_IDR register.
 */
/*\{*/
#define UART_IDR_RXRDY    BV(0)   //< Disable RXRDY Interrupt
#define UART_IDR_TXRDY    BV(1)   //< Disable TXRDY Interrupt
#define UART_IDR_ENDRX    BV(3)   //< Disable End of Receive Transfer Interrupt
#define UART_IDR_ENDTX    BV(4)   //< Disable End of Transmit Interrupt
#define UART_IDR_OVRE     BV(5)   //< Disable Overrun Error Interrupt
#define UART_IDR_FRAME    BV(6)   //< Disable Framing Error Interrupt
#define UART_IDR_PARE     BV(7)   //< Disable Parity Error Interrupt
#define UART_IDR_TXEMPTY  BV(9)   //< Disable TXEMPTY Interrupt
#define UART_IDR_TXBUFE   BV(11)  //< Disable Buffer Empty Interrupt
#define UART_IDR_RXBUFF   BV(12)  //< Disable Buffer Full Interrupt
/*\}*/

/**
 * Bit fields in the UART_IMR register.
 */
/*\{*/
#define UART_IMR_RXRDY    BV(0)   //< Mask RXRDY Interrupt
#define UART_IMR_TXRDY    BV(1)   //< Disable TXRDY Interrupt
#define UART_IMR_ENDRX    BV(3)   //< Mask End of Receive Transfer Interrupt
#define UART_IMR_ENDTX    BV(4)   //< Mask End of Transmit Interrupt
#define UART_IMR_OVRE     BV(5)   //< Mask Overrun Error Interrupt
#define UART_IMR_FRAME    BV(6)   //< Mask Framing Error Interrupt
#define UART_IMR_PARE     BV(7)   //< Mask Parity Error Interrupt
#define UART_IMR_TXEMPTY  BV(9)   //< Mask TXEMPTY Interrupt
#define UART_IMR_TXBUFE   BV(11)  //< Mask TXBUFE Interrupt
#define UART_IMR_RXBUFF   BV(12)  //< Mask RXBUFF Interrupt
/*\}*/

/**
 * Bit fields in the UART_SR register.
 */
/*\{*/
#define UART_SR_RXRDY     BV(0)   //< Receiver Ready
#define UART_SR_TXRDY     BV(1)   //< Transmitter Ready
#define UART_SR_ENDRX     BV(3)   //< End of Receiver Transfer
#define UART_SR_ENDTX     BV(4)   //< End of Transmitter Transfer
#define UART_SR_OVRE      BV(5)   //< Overrun Error
#define UART_SR_FRAME     BV(6)   //< Framing Error
#define UART_SR_PARE      BV(7)   //< Parity Error
#define UART_SR_TXEMPTY   BV(9)   //< Transmitter Empty
#define UART_SR_TXBUFE    BV(11)  //< Transmission Buffer Empty
#define UART_SR_RXBUFF    BV(12)  //< Receive Buffer Full
/*\}*/

/**
 * Bit fields in the UART_RHR register.
 */
/*\{*/
#define UART_RHR_RXCHR_M  0xFF  //< Received Character mask
#define UART_RHR_RXCHR_S  0     //< Received Character shift
/*\}*/

/**
 * Bit fields in the UART_THR register.
 */
/*\{*/
#define UART_THR_TXCHR_M  0xFF  //< Character to be Transmitted mask
#define UART_THR_TXCHR_S  0     //< Character to be Transmitted shift
/*\}*/

/**
 * Bit fields in the UART_BRGR register.
 */
/*\{*/
#define UART_BRGR_CD_M  0xFFFF  //< Clock Divisor mask
#define UART_BRGR_CD_S  0       //< Clock Divisor shift
/*\}*/

/**
 * Bit fields in the UART_RPR register.
 */
/*\{*/
#define UART_RPR_RXPTR_M 0xFFFFFFFF  //< Receive Pointer Register mask
#define UART_RPR_RXPTR_S 0           //< Receive Pointer Register shift
/*\}*/

/**
 * Bit fields in the UART_RCR register.
 */
/*\{*/
#define UART_RCR_RXCTR_M 0xFFFF  //< Receive Counter Register mask
#define UART_RCR_RXCTR_S 0       //< Receive Counter Register shift
/*\}*/

/**
 * Bit fields in the UART_TPR register.
 */
/*\{*/
#define UART_TPR_TXPTR_M 0xFFFFFFFF  //< Transmit Counter Register mask
#define UART_TPR_TXPTR_S 0           //< Transmit Counter Register shift
/*\}*/

/**
 * Bit fields in the UART_TCR register.
 */
/*\{*/
#define UART_TCR_TXCTR_M 0xFFFF  //< Transmit Counter Register mask
#define UART_TCR_TXCTR_S 0       //< Transmit Counter Register shift
/*\}*/

/**
 * Bit fields in the UART_RNPR register.
 */
/*\{*/
#define UART_RNPR_RXNPTR_M 0xFFFFFFFF  //< Receive Next Pointer mask
#define UART_RNPR_RXNPTR_S 0           //< Receive Next Pointer shift
/*\}*/

/**
 * Bit fields in the UART_RNCR register.
 */
/*\{*/
#define UART_RNCR_RXNCTR_M 0xFFFF  //< Receive Next Counter mask
#define UART_RNCR_RXNCTR_S 0       //< Receive Next Counter shift
/*\}*/

/**
 * Bit fields in the UART_TNPR register.
 */
/*\{*/
#define UART_TNPR_TXNPTR_M 0xFFFFFFFF  //< Transmit Next Pointer mask
#define UART_TNPR_TXNPTR_S 0           //< Transmit Next Pointer shift
/*\}*/

/**
 * Bit fields in the UART_TNCR register.
 */
/*\{*/
#define UART_TNCR_TXNCTR_M 0xFFFF  //< Transmit Counter Next mask
#define UART_TNCR_TXNCTR_S 0       //< Transmit Counter Next shift
/*\}*/

/**
 * Bit fields in the UART_PTCR register.
 */
/*\{*/
#define UART_PTCR_RXTEN   BV(0)  //< Receiver Transfer Enable
#define UART_PTCR_RXTDIS  BV(1)  //< Receiver Transfer Disable
#define UART_PTCR_TXTEN   BV(8)  //< Transmitter Transfer Enable
#define UART_PTCR_TXTDIS  BV(9)  //< Transmitter Transfer Disable
/*\}*/

/**
 * Bit fields in the UART_PTSR register.
 */
/*\{*/
#define UART_PTSR_RXTEN   BV(0)  //< Receiver Transfer Enable
#define UART_PTSR_TXTEN   BV(8)  //< Transmitter Transfer Enable
/*\}*/


#endif /* SAM3_UART_H */

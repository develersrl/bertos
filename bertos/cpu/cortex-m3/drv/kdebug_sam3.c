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
 * \brief AT91SAM3 debug support (implementation).
 *
 * \author Stefano Fedrigo <aleph@develer.com>
 */

#include <cfg/cfg_debug.h>
#include <cfg/macros.h> /* for BV() */

#include <io/sam3.h>


#if CONFIG_KDEBUG_PORT == 0
	#define UART_BASE       UART0
	#define UART_INT        UART0_IRQn
	#define UART_PIO_BASE   PIOA
	#define UART_PINS       (GPIO_UART0_RX_PIN | GPIO_UART0_TX_PIN)
#elif (CONFIG_KDEBUG_PORT == 1) && !defined(CPU_CM3_AT91SAM3U)
	#define UART_BASE       UART1
	#define UART_INT        UART1_IRQn
	#define UART_PIO_BASE   PIOB
	#define UART_PINS       (GPIO_UART1_RX_PIN | GPIO_UART1_TX_PIN)
#else
	#error "UART port not supported in this board"
#endif

// TODO: refactor serial simple functions and use them, see lm3s kdebug
#define KDBG_WAIT_READY()     while (!(UART_BASE->UART_SR & UART_SR_TXRDY)) {}
#define KDBG_WAIT_TXDONE()    while (!(UART_BASE->UART_SR & UART_SR_TXEMPTY)) {}

#define KDBG_WRITE_CHAR(c)    do { UART_BASE->UART_THR = (c); } while(0)

/* Debug unit is used only for debug purposes so does not generate interrupts. */
#define KDBG_MASK_IRQ(old)    do { (void)old; } while(0)

/* Debug unit is used only for debug purposes so does not generate interrupts. */
#define KDBG_RESTORE_IRQ(old) do { (void)old; } while(0)

typedef uint32_t kdbg_irqsave_t;


INLINE void kdbg_hw_init(void)
{
	/* Disable PIO mode and set appropriate UART pins peripheral mode */
	UART_PIO_BASE->PIO_PDR = UART_PINS;
	UART_PIO_BASE->PIO_ABCDSR[0] &= ~UART_PINS;
	UART_PIO_BASE->PIO_ABCDSR[1] &= ~UART_PINS;

	/* Enable the peripheral clock */
	PMC_PCER |= BV(UART_INT);

	/* Reset and disable receiver & transmitter */
	UART_BASE->UART_CR = UART_CR_RSTRX | UART_CR_RSTTX | UART_CR_RXDIS | UART_CR_TXDIS;

	/* Set mode: normal, no parity */
	UART_BASE->UART_MR = UART_MR_PAR_NO;

	/* Set baud rate */
	UART_BASE->UART_BRGR = CPU_FREQ / CONFIG_KDEBUG_BAUDRATE / 16;

	/* Enable receiver & transmitter */
	UART_BASE->UART_CR = UART_CR_RXEN | UART_CR_TXEN;
}

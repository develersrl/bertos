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
 * \brief LM3S1968 startup interrupt vector table
 *
 * \author Andrea Righi <arighi@develer.com>
 */

.syntax unified
.thumb

.text
.thumb_func
default_isr:
	wfi
	b default_isr

.section .vectors,"ax",%progbits

irq_vectors:
	.word __msp_end		/* Initial stack pointer */
	.word __init		/* The reset handler */
	.word default_isr	/* The NMI handler */
	.word 	default_isr	/* The hard fault handler */
	.word default_isr	/* The MPU fault handler */
	.word default_isr	/* The bus fault handler */
	.word default_isr	/* The usage fault handler */
	.word 0			/* Reserved */
	.word 0			/* Reserved */
	.word 0			/* Reserved */
	.word 0			/* Reserved */
	.word default_isr	/* SVCall handler */
	.word default_isr	/* Debug monitor handler */
	.word 0			/* Reserved */
	.word default_isr	/* The PendSV handler */
	.word default_isr	/* The SysTick handler */
	.word default_isr	/* GPIO Port A */
	.word default_isr	/* GPIO Port B */
	.word default_isr	/* GPIO Port C */
	.word default_isr	/* GPIO Port D */
	.word default_isr	/* GPIO Port E */
	.word default_isr	/* UART0 Rx and Tx */
	.word default_isr	/* UART1 Rx and Tx */
	.word default_isr	/* SSI0 Rx and Tx */
	.word default_isr	/* I2C0 Master and Slave */
	.word default_isr	/* PWM Fault */
	.word default_isr	/* PWM Generator 0 */
	.word default_isr	/* PWM Generator 1 */
	.word default_isr	/* PWM Generator 2 */
	.word default_isr	/* Quadrature Encoder 0 */
	.word default_isr	/* ADC Sequence 0 */
	.word default_isr	/* ADC Sequence 1 */
	.word default_isr	/* ADC Sequence 2 */
	.word default_isr	/* ADC Sequence 3 */
	.word default_isr	/* Watchdog timer */
	.word default_isr	/* Timer 0 subtimer A */
	.word default_isr	/* Timer 0 subtimer B */
	.word default_isr	/* Timer 1 subtimer A */
	.word default_isr	/* Timer 1 subtimer B */
	.word default_isr	/* Timer 2 subtimer A */
	.word default_isr	/* Timer 2 subtimer B */
	.word default_isr	/* Analog Comparator 0 */
	.word default_isr	/* Analog Comparator 1 */
	.word default_isr	/* Analog Comparator 2 */
	.word default_isr	/* System Control (PLL, OSC, BO) */
	.word default_isr	/* FLASH Control */
	.word default_isr	/* GPIO Port F */
	.word default_isr	/* GPIO Port G */
	.word default_isr	/* GPIO Port H */
	.word default_isr	/* UART2 Rx and Tx */
	.word default_isr	/* SSI1 Rx and Tx */
	.word default_isr	/* Timer 3 subtimer A */
	.word default_isr	/* Timer 3 subtimer B */
	.word default_isr	/* I2C1 Master and Slave */
	.word default_isr	/* Quadrature Encoder 1 */
	.word default_isr	/* CAN0 */
	.word default_isr	/* CAN1 */
	.word default_isr	/* CAN2 */
	.word default_isr	/* Ethernet */
	.word default_isr	/* Hibernate */

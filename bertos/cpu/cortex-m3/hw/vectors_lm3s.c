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

#include <cfg/compiler.h>
#include <cpu/attr.h> /* PAUSE, UNREACHABLE */

extern size_t __stack_end;
extern void __init(void);

static void NORETURN NAKED default_isr(void)
{
	PAUSE;
	UNREACHABLE();
}

static void (* const irq_vectors[])(void) __attribute__ ((section(".vectors"))) =
{
	(void (*)(void))&__stack_end,	/* Initial stack pointer */
	__init,		/* The reset handler */
	default_isr,	/* The NMI handler */
	default_isr,	/* The hard fault handler */
	default_isr,	/* The MPU fault handler */
	default_isr,	/* The bus fault handler */
	default_isr,	/* The usage fault handler */
	0,		/* Reserved */
	0,		/* Reserved */
	0,		/* Reserved */
	0,		/* Reserved */
	default_isr,	/* SVCall handler */
	default_isr,	/* Debug monitor handler */
	0,		/* Reserved */
	default_isr,	/* The PendSV handler */
	default_isr,	/* The SysTick handler */
	default_isr,	/* GPIO Port A */
	default_isr,	/* GPIO Port B */
	default_isr,	/* GPIO Port C */
	default_isr,	/* GPIO Port D */
	default_isr,	/* GPIO Port E */
	default_isr,	/* UART0 Rx and Tx */
	default_isr,	/* UART1 Rx and Tx */
	default_isr,	/* SSI0 Rx and Tx */
	default_isr,	/* I2C0 Master and Slave */
	default_isr,	/* PWM Fault */
	default_isr,	/* PWM Generator 0 */
	default_isr,	/* PWM Generator 1 */
	default_isr,	/* PWM Generator 2 */
	default_isr,	/* Quadrature Encoder 0 */
	default_isr,	/* ADC Sequence 0 */
	default_isr,	/* ADC Sequence 1 */
	default_isr,	/* ADC Sequence 2 */
	default_isr,	/* ADC Sequence 3 */
	default_isr,	/* Watchdog timer */
	default_isr,	/* Timer 0 subtimer A */
	default_isr,	/* Timer 0 subtimer B */
	default_isr,	/* Timer 1 subtimer A */
	default_isr,	/* Timer 1 subtimer B */
	default_isr,	/* Timer 2 subtimer A */
	default_isr,	/* Timer 2 subtimer B */
	default_isr,	/* Analog Comparator 0 */
	default_isr,	/* Analog Comparator 1 */
	default_isr,	/* Analog Comparator 2 */
	default_isr,	/* System Control (PLL, OSC, BO) */
	default_isr,	/* FLASH Control */
	default_isr,	/* GPIO Port F */
	default_isr,	/* GPIO Port G */
	default_isr,	/* GPIO Port H */
	default_isr,	/* UART2 Rx and Tx */
	default_isr,	/* SSI1 Rx and Tx */
	default_isr,	/* Timer 3 subtimer A */
	default_isr,	/* Timer 3 subtimer B */
	default_isr,	/* I2C1 Master and Slave */
	default_isr,	/* Quadrature Encoder 1 */
	default_isr,	/* CAN0 */
	default_isr,	/* CAN1 */
	default_isr,	/* CAN2 */
	default_isr,	/* Ethernet */
	default_isr	/* Hibernate */
};

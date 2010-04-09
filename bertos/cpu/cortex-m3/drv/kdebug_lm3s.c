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
 * \brief LM3S debug support (implementation).
 *
 * \author Andrea Righi <arighi@develer.com>
 */

#include <cfg/cfg_debug.h>
#include <cfg/macros.h> /* for BV() */
#include <drv/clock_lm3s.h> /* __delay() */
#include <drv/gpio_lm3s.h>
#include <io/lm3s.h>
#include "kdebug_lm3s.h"

INLINE void uart_disable(size_t base)
{
	/* Disable the FIFO */
	HWREG(base + UART_O_LCRH) &= ~UART_LCRH_FEN;
	/* Disable the UART */
	HWREG(base + UART_O_CTL) &=
		~(UART_CTL_UARTEN | UART_CTL_TXE | UART_CTL_RXE);
}

INLINE void uart_enable(size_t base)
{
	/* Enable the FIFO */
	HWREG(base + UART_O_LCRH) |= UART_LCRH_FEN;
	/* Enable RX, TX, and the UART */
	HWREG(base + UART_O_CTL) |=
			UART_CTL_UARTEN | UART_CTL_TXE | UART_CTL_RXE;
}

INLINE void uart_config(size_t base, uint32_t baud, reg32_t config)
{
	unsigned long div;
	bool hi_speed;

	if (baud * 16 > CPU_FREQ)
	{
		hi_speed = true;
		baud /= 2;
	}
	div = (CPU_FREQ * 8 / baud + 1) / 2;

	uart_disable(base);

	if (hi_speed)
		HWREG(base + UART_O_CTL) |= UART_CTL_HSE;
	else
		HWREG(base + UART_O_CTL) &= ~UART_CTL_HSE;

	/* Set the baud rate */
	HWREG(base + UART_O_IBRD) = div / 64;
	HWREG(base + UART_O_FBRD) = div % 64;

	/* Set parity, data length, and number of stop bits. */
	HWREG(base + UART_O_LCRH) = config;

	/* Clear the flags register */
	HWREG(base + UART_O_FR) = 0;

	uart_enable(base);
}

INLINE bool uart_putchar(size_t base, unsigned char ch)
{
	if (!(HWREG(base + UART_O_FR) & UART_FR_TXFF))
	{
		HWREG(base + UART_O_DR) = ch;
		return true;
	}
	return false;
}

#if CONFIG_KDEBUG_PORT == KDEBUG_PORT_DBGU
#define KDBG_WAIT_READY()     while (HWREG(UART0_BASE + UART_O_FR) & UART_FR_BUSY) {}
#define KDBG_WAIT_TXDONE()    while (!(HWREG(UART0_BASE + UART_O_FR) & UART_FR_TXFE)) {}

#define KDBG_WRITE_CHAR(c)    do { HWREG(UART0_BASE + UART_O_DR) = c; } while(0)

/* Debug unit is used only for debug purposes so does not generate interrupts. */
#define KDBG_MASK_IRQ(old)    do { (void)old; } while(0)

/* Debug unit is used only for debug purposes so does not generate interrupts. */
#define KDBG_RESTORE_IRQ(old) do { (void)old; } while(0)

typedef uint32_t kdbg_irqsave_t;

#else
#error CONFIG_KDEBUG_PORT should be KDEBUG_PORT_DBGU
#endif

INLINE void kdbg_hw_init(void)
{
	/* Enable the peripheral clock */
	SYSCTL_RCGC1_R |= SYSCTL_RCGC1_UART0;
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOA;
	__delay(512);
	/* Set GPIO A0 and A1 as UART pins */
	lm3s_gpio_pin_config(GPIO_PORTA_BASE, BV(0) | BV(1),
			GPIO_DIR_MODE_HW, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);
	/* 115.200, 8-bit word, no parity, one stop bit */
	uart_config(UART0_BASE, CONFIG_KDEBUG_BAUDRATE, UART_LCRH_WLEN_8);
}

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
 * \brief STM32 debug support (implementation).
 *
 * \author Andrea Righi <arighi@develer.com>
 */

#include <cfg/macros.h> /* for BV() */
#include <cfg/cfg_debug.h>
#include <drv/gpio_stm32.h>
#include <drv/clock_stm32.h> /* RCC */
#include <io/stm32.h>
#include "kdebug_stm32.h"

#define CR1_RUN_SET               ((uint16_t)0x2000)  /* USART Enable MASK */
#define CR1_RUN_RESET             ((uint16_t)0xDFFF)  /* USART Disable MASK */
#define CR1_RWU_SET               ((uint16_t)0x0002)  /* USART mute mode Enable MASK */
#define CR1_RWU_RESET             ((uint16_t)0xFFFD)  /* USART mute mode Enable MASK */
#define CR1_SBK_SET               ((uint16_t)0x0001)  /* USART Break Character send MASK */
#define CR1_CLEAR_MASK            ((uint16_t)0xE9F3)  /* USART CR1 MASK */

#define CR2_MASK                  ((uint16_t)0xFFF0)  /* USART address MASK */
#define CR2_LINE_SET              ((uint16_t)0x4000)  /* USART LIN Enable MASK */
#define CR2_LINE_RESET            ((uint16_t)0xBFFF)  /* USART LIN Disable MASK */
#define CR2_CLEAR_MASK            ((uint16_t)0xC0FF)  /* USART CR2 MASK */

#define CR3_SCEN_SET              ((uint16_t)0x0020)  /* USART SC Enable MASK */
#define CR3_SCEN_RESET            ((uint16_t)0xFFDF)  /* USART SC Disable MASK */
#define CR3_NACK_SET              ((uint16_t)0x0010)  /* USART SC NACK Enable MASK */
#define CR3_NACK_RESET            ((uint16_t)0xFFEF)  /* USART SC NACK Disable MASK */
#define CR3_HDSEL_SET             ((uint16_t)0x0008)  /* USART Half-Duplex Enable MASK */
#define CR3_HDSEL_RESET           ((uint16_t)0xFFF7)  /* USART Half-Duplex Disable MASK */
#define CR3_IRLP_MASK             ((uint16_t)0xFFFB)  /* USART IrDA LowPower mode MASK */
#define CR3_LBDL_MASK             ((uint16_t)0xFFDF)  /* USART LIN Break detection MASK */
#define CR3_WAKE_MASK             ((uint16_t)0xF7FF)  /* USART WakeUp Method MASK */
#define CR3_IREN_SET              ((uint16_t)0x0002)  /* USART IrDA Enable MASK */
#define CR3_IREN_RESET            ((uint16_t)0xFFFD)  /* USART IrDA Disable MASK */
#define CR3_CLEAR_MASK            ((uint16_t)0xFCFF)  /* USART CR3 MASK */

#define GTPR_LSB_MASK             ((uint16_t)0x00FF)  /* Guard Time Register LSB MASK */
#define GTPR_MSB_MASK             ((uint16_t)0xFF00)  /* Guard Time Register MSB MASK */

#define USART_IT_MASK             ((uint16_t)0x001F)  /* USART Interrupt MASK */

/* USART flags */
#define USART_FLAG_CTS            ((uint16_t)0x0200)
#define USART_FLAG_LBD            ((uint16_t)0x0100)
#define USART_FLAG_TXE            ((uint16_t)0x0080)
#define USART_FLAG_TC             ((uint16_t)0x0040)
#define USART_FLAG_RXNE           ((uint16_t)0x0020)
#define USART_FLAG_IDLE           ((uint16_t)0x0010)
#define USART_FLAG_ORE            ((uint16_t)0x0008)
#define USART_FLAG_NE             ((uint16_t)0x0004)
#define USART_FLAG_FE             ((uint16_t)0x0002)
#define USART_FLAG_PE             ((uint16_t)0x0001)

/* USART registers */
struct stm32_usart
{
	reg16_t SR;
	uint16_t _RESERVED0;
	reg16_t DR;
	uint16_t _RESERVED1;
	reg16_t BRR;
	uint16_t _RESERVED2;
	reg16_t CR1;
	uint16_t _RESERVED3;
	reg16_t CR2;
	uint16_t _RESERVED4;
	reg16_t CR3;
	uint16_t _RESERVED5;
	reg16_t GTPR;
	uint16_t _RESERVED6;
};

/* USART mode */
#define USART_MODE_RX             ((uint16_t)0x0004)
#define USART_MODE_TX             ((uint16_t)0x0008)

/* USART last bit */
#define USART_LASTBIT_DISABLE     ((uint16_t)0x0000)
#define USART_LASTBIT_ENABLE      ((uint16_t)0x0100)

#if CONFIG_KDEBUG_PORT == 0
	#define UART_BASE ((struct stm32_usart *)USART1_BASE)
#elif CONFIG_KDEBUG_PORT == 1
	#define UART_BASE ((struct stm32_usart *)USART2_BASE)
#elif CONFIG_KDEBUG_PORT == 2
	#define UART_BASE ((struct stm32_usart *)USART3_BASE)
#else
	#error "UART port not supported in this board"
#endif

#define KDBG_WAIT_READY()	while (!(UART_BASE->SR & USART_FLAG_TXE))
#define KDBG_WAIT_TXDONE()	while (!(UART_BASE->SR & USART_FLAG_TC))

#define KDBG_WRITE_CHAR(c)	do { UART_BASE->DR = (c) & 0x1ff; } while(0)

/* Debug unit is used only for debug purposes so does not generate interrupts. */
#define KDBG_MASK_IRQ(old)	do { (void)old; } while(0)

/* Debug unit is used only for debug purposes so does not generate interrupts. */
#define KDBG_RESTORE_IRQ(old)	do { (void)old; } while(0)

typedef uint32_t kdbg_irqsave_t;

#define GPIO_USART1_TX_PIN	(1 << 9)
#define GPIO_USART1_RX_PIN	(1 << 10)

#define GPIO_USART2_TX_PIN	(1 << 2)
#define GPIO_USART2_RX_PIN	(1 << 3)

#define GPIO_USART3_TX_PIN	(1 << 13)
#define GPIO_USART3_RX_PIN	(1 << 14)

INLINE uint16_t evaluate_brr(void)
{
	uint32_t freq, reg, div, frac;

	/* NOTE: PCLK1 has been configured as CPU_FREQ / 2 */
	freq = (CONFIG_KDEBUG_PORT == 0) ? CPU_FREQ : CPU_FREQ / 2;
	div = (0x19 * freq) / (0x04 * CONFIG_KDEBUG_BAUDRATE);
	reg = (div / 0x64) << 0x04;
	frac = div - (0x64 * (reg >> 0x04));
	reg |= ((frac * 0x10 + 0x32) / 0x64) & 0x0f;

	return (uint16_t)reg;
}

/* Initialize UART debug port */
INLINE void kdbg_hw_init(void)
{
	/* Enable clocking on AFIO */
	RCC->APB2ENR |= RCC_APB2_AFIO;
	/* Configure USART pins */
#if CONFIG_KDEBUG_PORT == 0
	RCC->APB2ENR |= RCC_APB2_GPIOA;
	RCC->APB2ENR |= RCC_APB2_USART1;
	stm32_gpioPinConfig((struct stm32_gpio *)GPIOA_BASE, GPIO_USART1_TX_PIN,
				GPIO_MODE_AF_PP, GPIO_SPEED_50MHZ);
#elif CONFIG_KDEBUG_PORT == 1
	RCC->APB2ENR |= RCC_APB2_GPIOA;
	RCC->APB1ENR |= RCC_APB1_USART2;
	stm32_gpioPinConfig((struct stm32_gpio *)GPIOA_BASE, GPIO_USART2_TX_PIN,
				GPIO_MODE_AF_PP, GPIO_SPEED_50MHZ);
#elif  CONFIG_KDEBUG_PORT == 2
	RCC->APB2ENR |= RCC_APB2_GPIOB;
	RCC->APB2ENR |= RCC_APB1_USART3;
	stm32_gpioPinConfig((struct stm32_gpio *)GPIOB_BASE, GPIO_USART3_TX_PIN,
				GPIO_MODE_AF_PP, GPIO_SPEED_50MHZ);
#else
	#error "UART port not supported in this board"
#endif
	/* Enable the USART by writing the UE bit */
	UART_BASE->CR1 |= CR1_RUN_SET;
	/* Configure the desired baud rate */
	UART_BASE->BRR = evaluate_brr();
	/* Set the Transmitter Enable bit in CR1 */
	UART_BASE->CR1 |= USART_MODE_TX;
}

/*!
 * \file
 * <!--
 * Copyright (C) 2003,2004 Develer S.r.l. (http://www.develer.com/)
 * This file is part of DevLib - See devlib/README for information.
 * -->
 *
 * \version $Id$
 *
 * \author Stefano Fedrigo <aleph@develer.com>
 * \author Giovanni Bajo <rasky@develer.com>
 *
 * \brief DSP5680x CPU specific serial I/O driver
 */

/*#*
 *#* $Log$
 *#* Revision 1.6  2004/10/03 20:43:22  bernie
 *#* Import changes from sc/firmware.
 *#*
 *#* Revision 1.5  2004/08/25 14:12:08  rasky
 *#* Aggiornato il comment block dei log RCS
 *#*
 *#* Revision 1.4  2004/07/30 14:27:49  rasky
 *#* Aggiornati alcuni file DSP56k per la nuova libreria di IRQ management
 *#*
 *#* Revision 1.3  2004/06/03 11:27:09  bernie
 *#* Add dual-license information.
 *#*
 *#* Revision 1.2  2004/05/23 18:21:53  bernie
 *#* Trim CVS logs and cleanup header info.
 *#*/

#include "ser.h"
#include "ser_p.h"
#include <drv/irq.h>
#include <debug.h>
#include <hw.h>
#include <DSP56F807.h>

// GPIO E is shared with SPI (in DSP56807). Pins 0&1 are TXD0 and RXD0. To use
//  the serial, we need to disable the GPIO functions on them.
#define REG_GPIO_SERIAL         REG_GPIO_E
#define REG_GPIO_SERIAL_MASK    0x3

// Check flag consistency
#if (SERRF_PARITYERROR != REG_SCI_SR_PF) || \
	(SERRF_RXSROVERRUN != REG_SCI_SR_OR) || \
	(SERRF_FRAMEERROR  != REG_SCI_SR_FE) || \
	(SERRF_NOISEERROR  != REG_SCI_SR_NF)
	#error error flags do not match with register bits
#endif

struct SCI
{
	struct SerialHardware hw;
	struct Serial* serial;
	volatile struct REG_SCI_STRUCT* regs;
	IRQ_VECTOR irq_tx;
	IRQ_VECTOR irq_rx;
};

static inline void enable_tx_irq_bare(volatile struct REG_SCI_STRUCT* regs)
{
	regs->CR |= REG_SCI_CR_TEIE | REG_SCI_CR_TIIE;
}

static inline void enable_rx_irq_bare(volatile struct REG_SCI_STRUCT* regs)
{
	regs->CR |= REG_SCI_CR_RIE | REG_SCI_CR_REIE;
}

static inline void disable_tx_irq_bare(volatile struct REG_SCI_STRUCT* regs)
{
	regs->CR &= ~(REG_SCI_CR_TEIE | REG_SCI_CR_TIIE);
}

static inline void disable_rx_irq_bare(volatile struct REG_SCI_STRUCT* regs)
{
	regs->CR &= ~(REG_SCI_CR_RIE | REG_SCI_CR_REIE);
}

static inline void disable_tx_irq(struct SerialHardware* _hw)
{
	struct SCI* hw = (struct SCI*)_hw;
	volatile struct REG_SCI_STRUCT* regs = hw->regs;

	disable_tx_irq_bare(regs);
}

static inline void enable_tx_irq(struct SerialHardware* _hw)
{
	struct SCI* hw = (struct SCI*)_hw;
	volatile struct REG_SCI_STRUCT* regs = hw->regs;

	enable_tx_irq_bare(regs);
}

static inline void enable_rx_irq(struct SerialHardware* _hw)
{
	struct SCI* hw = (struct SCI*)_hw;
	volatile struct REG_SCI_STRUCT* regs = hw->regs;

	enable_rx_irq_bare(regs);
}

static void tx_isr(const struct SCI *hw)
{
#pragma interrupt warn
	volatile struct REG_SCI_STRUCT* regs = hw->regs;

	if (fifo_isempty(&hw->serial->txfifo))
		disable_tx_irq_bare(regs);
	else
	{
		// Clear transmitter flags before sending data
		(void)regs->SR;
		regs->DR = fifo_pop(&hw->serial->txfifo);
	}
}

static void rx_isr(const struct SCI *hw)
{
#pragma interrupt warn
	volatile struct REG_SCI_STRUCT* regs = hw->regs;

	hw->serial->status |= regs->SR & (SERRF_PARITYERROR |
	                                  SERRF_RXSROVERRUN |
	                                  SERRF_FRAMEERROR |
	                                  SERRF_NOISEERROR);

	if (fifo_isfull(&hw->serial->rxfifo))
		hw->serial->status |= SERRF_RXFIFOOVERRUN;
	else
		fifo_push(&hw->serial->rxfifo, regs->DR);

	// Writing anything to the status register clear the
	//  error bits.
	regs->SR = 0;
}

static void init(struct SerialHardware* _hw, struct Serial* ser)
{
	struct SCI* hw = (struct SCI*)_hw;
	volatile struct REG_SCI_STRUCT* regs = hw->regs;

	// Clear status register (IRQ/status flags)
	(void)regs->SR;
	regs->SR = 0;

	// Clear data register
	(void)regs->DR;

	// Install the handlers and set priorities for both IRQs
	irq_install(hw->irq_tx, (isr_t)tx_isr, hw);
	irq_install(hw->irq_rx, (isr_t)rx_isr, hw);
	irq_setpriority(hw->irq_tx, IRQ_PRIORITY_SCI_TX);
	irq_setpriority(hw->irq_rx, IRQ_PRIORITY_SCI_RX);

	// Activate the RX error interrupts, and RX/TX transmissions
	regs->CR = REG_SCI_CR_TE | REG_SCI_CR_RE;
	enable_rx_irq_bare(regs);

	// Disable GPIO pins for TX and RX lines
	REG_GPIO_SERIAL->PER |= REG_GPIO_SERIAL_MASK;

	hw->serial = ser;
}

static void cleanup(struct SerialHardware* _hw)
{
	// TODO!
	ASSERT(0);
}

static void setbaudrate(struct SerialHardware* _hw, unsigned long rate)
{
	struct SCI* hw = (struct SCI*)_hw;

	// SCI has an internal 16x divider on the input clock, which comes
	//  from the IPbus (see the scheme in user manual, 12.7.3). We apply
	//  it to calculate the period to store in the register.
	hw->regs->BR = (IPBUS_FREQ + rate * 8ul) / (rate * 16ul);
}

static void setparity(struct SerialHardware* _hw, int parity)
{
	// ???
	ASSERT(0);
}


static const struct SerialHardwareVT SCI_VT = 
{
	.init = init,
	.cleanup = cleanup,
	.setbaudrate = setbaudrate,
	.setparity = setparity,
	.enabletxirq = enable_tx_irq,
};

static struct SCI SCIDescs[2] =
{
	{
		.hw = { .table = &SCI_VT },
		.regs = &REG_SCI[0],
		.irq_rx = IRQ_SCI0_RECEIVER_FULL,
		.irq_tx = IRQ_SCI0_TRANSMITTER_READY,
	},

	{
		.hw = { .table = &SCI_VT },
		.regs = &REG_SCI[1],
		.irq_rx = IRQ_SCI1_RECEIVER_FULL,
		.irq_tx = IRQ_SCI1_TRANSMITTER_READY,
	},
};


struct SerialHardware* ser_hw_getdesc(int unit)
{
	ASSERT(unit < 2);
	return &SCIDescs[unit].hw;
}

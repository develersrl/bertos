/**
 * \file
 * <!--
 * Copyright 2007 Develer S.r.l. (http://www.develer.com/)
 * This file is part of DevLib - See README.devlib for information.
 * -->
 *
 * \version $Id$
 *
 * \author Francesco Sacchi <batt@develer.com>
 *
 * \brief System IRQ handler for Atmel AT91 ARM7 processors.
 *
 * In Atmel AT91 ARM7TDMI processors, there are various
 * peripheral interrupt sources.
 * In general, every source has its own interrupt vector, so it
 * is possible to assign a specific handler for each interrupt
 * independently.
 * However, there are a few sources called "system sources" that
 * share a common IRQ line and vector, called "system IRQ".
 * So a unique system IRQ handler is implemented here.
 * This module also contains an interface to manage every source
 * independently. It is possible to assign to every system IRQ
 * a specific IRQ handler.
 *
 * \see sysirq_setHandler
 * \see sysirq_setEnable
 */

#include "sysirq.h"
#include "at91.h"
#include <cfg/cpu.h>

#warning Very untested!

/**
 * Enable/disable the Periodic Interrupt Timer
 * interrupt.
 */
INLINE static void pit_setEnable(bool enable)
{
	if (enable)
		PIT_MR |= BV(PITIEN);
	else
		PIT_MR &= ~BV(PITIEN);
}

/**
 * Table containing all system irqs.
 */
static SysIrq sysirq_tab[] =
{
	/* PIT, Periodic Interval Timer (System timer)*/
	{
		.enabled = false,
		.setEnable = pit_setEnable,
		.handler = NULL,
	},
	/* TODO: add other system sources here */
};

STATIC_ASSERT(countof(sysirq_tab) == SYSIRQ_CNT);


/**
 * System IRQ dispatcher.
 * This is the entry point for all system IRQs in AT91.
 * This function checks for interrupt enable state of
 * various sources (system timer, etc..) and calls
 * the corresponding handler.
 */
static void sysirq_dispatcher(void)
{
	#warning TODO add IRQ prologue/epilogue
	for (int i = 0; i < countof(sysirq_tab); i++)
	{
		if (sysirq_tab[i].enabled
		 && sysirq_tab[i].handler)
			sysirq_tab[i].handler();
	}
}

#define SYSIRQ_PRIORITY 0 ///< default priority for system irqs.


MOD_DEFINE(sysirq);

/**
 * Init system IRQ handling.
 * \note all system interrupts are disabled.
 */
void sysirq_init(void)
{
	cpuflags_t flags;
	IRQ_SAVE_DISABLE(flags);

	/* Disable all system interrupts */
	for (int i = 0; i < countof(sysirq_tab); i++)
		sysirq_tab[i].setEnable(false);

	/* Set the vector. */
	AIC_SVR(SYSC_ID) = sysirq_handler;
	/* Initialize to edge triggered with defined priority. */
	AIC_SMR(SYSC_ID) = BV(AIC_SRCTYPE_INT_EDGE_TRIGGERED) | SYSIRQ_PRIORITY;
	/* Clear interrupt */
	AIC_ICCR = BV(SYSC_ID);

	IRQ_RESTORE(flags);
	MOD_INIT(sysirq);
}


/**
 * Helper function used to set handler for system IRQ \a irq.
 */
void sysirq_setHandler(sysirq_t irq, sysirq_handler_t handler)
{
	ASSERT(irq >= 0);
	ASSERT(irq < SYSIRQ_CNT);
	sysirq_tab[irq].handler = handler;
}

/**
 * Helper function used to enable/disable system IRQ \a irq.
 */
void sysirq_setEnable(sysirq_t irq, bool enable)
{
	ASSERT(irq >= 0);
	ASSERT(irq < SYSIRQ_CNT);

	sysirq_tab[irq].setEnable(enable);
	sysirq_enabled = enable;
}

/**
 * Helper function used to get system IRQ \a irq state.
 */
bool sysirq_enabled(sysirq_t irq)
{
	ASSERT(irq >= 0);
	ASSERT(irq < SYSIRQ_CNT);
	return sysirq_tab[irq].enabled;
}

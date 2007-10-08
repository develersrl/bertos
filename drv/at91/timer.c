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
 * \brief Low-level timer module for Atmel AT91 (inplementation).
 */

#include "timer.h"
#include "at91sam7s.h"
#include "sysirq.h"

#include <cfg/macros.h> // BV()
#include <cfg/module.h>
#include <cfg/cpu.h>


/** HW dependent timer initialization  */
#if (CONFIG_TIMER == TIMER_ON_PIT)
	INLINE void timer_hw_irq(void)
	{
		/* Reset counters, this is needed to reset timer and interrupt flags */
		uint32_t dummy = PIVR;
		(void) dummy;
	}

	INLINE bool timer_hw_triggered(void)
	{
		return PIT_SR & BV(PITS);
	}

	INLINE void timer_hw_init(void)
	{
		cpuflags_t flags;

		MOD_CHECK(sysirq);

		IRQ_SAVE_DISABLE(flags);

		PIT_MR = TIMER_HW_CNT;
		/* Register system interrupt handler. */
		sysirq_setHandler(SYSIRQ_PIT, timer_handler);

		/* Enable interval timer and interval timer interrupts */
		PIT_MR |= BV(PITEN);
		sysirq_setEnable(SYSIRQ_PIT, true);

		/* Reset counters, this is needed to start timer and interrupt flags */
		uint32_t dummy = PIVR;
		(void) dummy;

		IRQ_RESTORE(flags);
	}

	INLINE hptime_t timer_hw_hpread(void)
	{
		/* In the upper part of PIT_PIIR there is unused data */
		return PIIR & CPIV_MASK;
	}

#else
	#error Unimplemented value for CONFIG_TIMER
#endif /* CONFIG_TIMER */

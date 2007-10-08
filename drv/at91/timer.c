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

#include <drv/timer_at91.h>
#include <cfg/macros.h> // BV()

#include <cfg/cpu.h>


/** HW dependent timer initialization  */
#if (CONFIG_TIMER == TIMER_ON_PIT)
	#warning Very untested!
	INLINE static void timer_hw_irq(void)
	{
		/* Reset counters, this is needed to reset timer and interrupt flags */
		volatile uint32_t dummy = PIT_PIVR;
	}

	INLINE static bool timer_hw_triggered(void)
	{
		return PIT_SR & BV(PITS);
	}

	INLINE static void timer_hw_init(void)
	{
		cpuflags_t flags;
		IRQ_SAVE_DISABLE(flags);

		PIT_MR = CLOCK_FREQ / (16 * TIMER_TICKS_PER_SEC) - 1;
		/* Register system interrupt handler. */
		sysirq_setHandler(SYSIRQ_PIT, timer_handler);

		/* Enable interval timer and interval timer interrupts */
		PIT_MR |= BV(PIT_PITEN);
		sysirq_setEnable(SYSIRQ_PIT, true);

		/* Reset counters, this is needed to start timer and interrupt flags */
		volatile uint32_t dummy = PIT_PIVR;

		IRQ_RESTORE(flags);
	}

	INLINE static hptime_t timer_hw_hpread(void)
	{
		/* In the upper part of PIT_PIIR there is unused data */
		return PIT_PIIR & 0xfffff;
	}

#else
	#error Unimplemented value for CONFIG_TIMER
#endif /* CONFIG_TIMER */

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
 * \brief Low-level timer module for Atmel AT91 (interface).
 */

#ifndef DRV_AT91_TIMER_H
#define DRV_AT91_TIMER_H

#include <appconfig.h>     /* CONFIG_TIMER */
#include <cfg/compiler.h>  /* uint8_t */
#include <hw_cpu.h>        /* CLOCK_FREQ */

/**
 * \name Values for CONFIG_TIMER.
 *
 * Select which hardware timer interrupt to use for system clock and softtimers.
 *
 * \{
 */
#define TIMER_ON_PIT 1  ///< System timer on Periodic interval timer

#define TIMER_DEFAULT TIMER_ON_PIT  ///< Default system timer
/* \} */

/*
 * Hardware dependent timer initialization.
 */
#if (CONFIG_TIMER == TIMER_ON_PIT)

	void timer_handler(void);

	#define DEFINE_TIMER_ISR     void timer_handler(void)
	#define TIMER_TICKS_PER_SEC  1000
	#define TIMER_HW_CNT         (CLOCK_FREQ / (16 * TIMER_TICKS_PER_SEC) - 1)

	/** Frequency of the hardware high-precision timer. */
	#define TIMER_HW_HPTICKS_PER_SEC (CLOCK_FREQ / 16)

	/// Type of time expressed in ticks of the hardware high-precision timer
	typedef uint32_t hptime_t;
#else

	#error Unimplemented value for CONFIG_TIMER
#endif /* CONFIG_TIMER */


#endif /* DRV_TIMER_AT91_H */

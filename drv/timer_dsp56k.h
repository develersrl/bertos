/*!
 * \file
 * <!--
 * Copyright 2004 Giovanni Bajo
 * Copyright 2004 Develer S.r.l. (http://www.develer.com/)
 * All Rights Reserved.
 * -->
 *
 * \version $Id$
 *
 * \author Giovanni Bajo <rasky@develer.com>
 *
 * \brief Driver module for DSP56K
 */

/*
 * $Log$
 * Revision 1.1  2004/05/23 18:23:30  bernie
 * Import drv/timer module.
 *
 */

#ifndef DRV_TIMER_DSP56K_H
#define DRV_TIMER_DSP56K_H

#include <DSP56F807.h>
#include <compiler.h>

//! The system timer for events is currently TMRA0. We prefer A/B over C/D because
//  A/B share the pin with the quadrature decoder module, and we do not need
//  pins for our system timer.
//  If you want to change this setting, you need also to modify the IRQ vector table.
#define REG_SYSTEM_TIMER          (REG_TIMER_A + 0)
#define SYSTEM_TIMER_IRQ_VECTOR   42   /* TMRA0 */

#define TIMER_PRESCALER           16

//! Frequency of the hardware high precision timer
#define TIMER_HW_HPTICKS_PER_SEC           (IPBUS_FREQ / TIMER_PRESCALER)

//! Type of time expressed in ticks of the hardware high precision timer
typedef uint16_t hptime_t;

static void timer_hw_init(void)
{
	uint16_t compare;

	// Clear compare flag status and enable interrupt on compare
	REG_SYSTEM_TIMER->SCR &= ~REG_TIMER_SCR_TCF;
	REG_SYSTEM_TIMER->SCR |= REG_TIMER_SCR_TCFIE;

	// Calculate the compare value needed to generate an interrupt exactly
	//  TICKS_PER_SEC times each second (usually, every millisecond). Check that
	//  the calculation is accurate, otherwise there is a precision error
	// (probably the prescaler is too big or too small).
	compare = TIMER_HW_HPTICKS_PER_SEC / TICKS_PER_SEC;
	ASSERT((uint32_t)compare * TICKS_PER_SEC == IPBUS_FREQ / TIMER_PRESCALER);
	REG_SYSTEM_TIMER->CMP1 = compare;

	// The value for reload (at initializationa and after compare is met) is zero
	REG_SYSTEM_TIMER->LOAD = 0;

	// Set the interrupt priority
	irq_setpriority(SYSTEM_TIMER_IRQ_VECTOR, IRQ_PRIORITY_SYSTEM_TIMER);

	// Small preprocessor trick to generate the REG_TIMER_CTRL_PRIMARY_IPBYNN macro
	//  needed to set the prescaler
	#define PP_CAT2(x,y)                      x ## y
	#define PP_CAT(x,y)                       PP_CAT2(x,y)
	#define REG_CONTROL_PRESCALER             PP_CAT(REG_TIMER_CTRL_PRIMARY_IPBY, TIMER_PRESCALER)

	// Setup the counter and start counting
	REG_SYSTEM_TIMER->CTRL =
		REG_TIMER_CTRL_MODE_RISING    |          // count rising edges (normal)
		REG_CONTROL_PRESCALER         |          // frequency (IPbus / TIMER_PRESCALER)
		REG_TIMER_CTRL_LENGTH;                   // up to CMP1, then reload
}

INLINE void timer_hw_irq(void)
{
	// Clear the overflow flag so that we are ready for another interrupt
	REG_SYSTEM_TIMER->SCR &= ~REG_TIMER_SCR_TCF;
}

INLINE hptime_t timer_hw_hpread(void)
{
	return REG_SYSTEM_TIMER->CNTR;
}

void system_timer_isr(void);

#define DEFINE_TIMER_ISR \
	void system_timer_isr(void)

#endif /* DRV_TIMER_DSP56_H */

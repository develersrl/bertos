/*!
 * \file
 * <!--
 * Copyright 2004 Giovanni Bajo
 * Copyright 2004 Develer S.r.l. (http://www.develer.com/)
 * This file is part of DevLib - See devlib/README for information.
 * -->
 *
 * \version $Id$
 *
 * \author Giovanni Bajo <rasky@develer.com>
 *
 * \brief Driver module for DSP56K
 */

/*#*
 *#* $Log$
 *#* Revision 1.6  2004/11/16 22:37:14  bernie
 *#* Replace IPTR with iptr_t.
 *#*
 *#* Revision 1.5  2004/08/25 14:12:08  rasky
 *#* Aggiornato il comment block dei log RCS
 *#*
 *#* Revision 1.4  2004/07/30 14:27:49  rasky
 *#* Aggiornati alcuni file DSP56k per la nuova libreria di IRQ management
 *#*
 *#* Revision 1.3  2004/06/06 18:30:34  bernie
 *#* Import DSP56800 changes from SC.
 *#*
 *#* Revision 1.2  2004/06/03 11:27:09  bernie
 *#* Add dual-license information.
 *#*
 *#* Revision 1.1  2004/05/23 18:23:30  bernie
 *#* Import drv/timer module.
 *#*
 *#*/

#ifndef DRV_TIMER_DSP56K_H
#define DRV_TIMER_DSP56K_H

#include "timer.h"
#include <DSP56F807.h>
#include <compiler.h>
#include <hw.h>
#include <drv/irq.h>

// Calculate register pointer and irq vector from hw.h setting
#define REG_SYSTEM_TIMER          PP_CAT(REG_TIMER_, SYSTEM_TIMER)
#define SYSTEM_TIMER_IRQ_VECTOR   PP_CAT(IRQ_TIMER_, SYSTEM_TIMER)

//! Prescaler for the system timer
#define TIMER_PRESCALER           16

//! Frequency of the hardware high precision timer
#define TIMER_HW_HPTICKS_PER_SEC           (IPBUS_FREQ / TIMER_PRESCALER)

//! Type of time expressed in ticks of the hardware high precision timer
typedef uint16_t hptime_t;

static void system_timer_isr(UNUSED(iptr_t, arg));

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

	// Set the interrupt handler and priority
	irq_install(SYSTEM_TIMER_IRQ_VECTOR, &system_timer_isr, NULL);
	irq_setpriority(SYSTEM_TIMER_IRQ_VECTOR, IRQ_PRIORITY_SYSTEM_TIMER);

	// Small preprocessor trick to generate the REG_TIMER_CTRL_PRIMARY_IPBYNN macro
	//  needed to set the prescaler
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

#define DEFINE_TIMER_ISR \
	static void system_timer_isr(UNUSED(iptr_t, arg))

#endif /* DRV_TIMER_DSP56_H */

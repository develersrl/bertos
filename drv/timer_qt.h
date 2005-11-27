/*!
 * \file
 * <!--
 * Copyright 2005 Develer S.r.l. (http://www.develer.com/)
 * This file is part of DevLib - See README.devlib for information.
 * -->
 *
 * \version $Id$
 *
 * \author Bernardo Innocenti <bernie@develer.com>
 *
 * \brief Low-level timer module for AVR (interface).
 */

/*#*
 *#* $Log$
 *#* Revision 1.1  2005/11/27 03:06:36  bernie
 *#* Qt timer emulation.
 *#*
 *#*/
#ifndef DRV_TIMER_QT_H
#define DRV_TIMER_QT_H

// HW dependent timer initialization

#define DEFINE_TIMER_ISR     void timer_isr(void)
#define TIMER_TICKS_PER_MSEC 1
#define TIMER_HW_CNT         (1<<31) /* We assume 32bit integers here */

/// Type of time expressed in ticks of the hardware high-precision timer.
typedef unsigned int hptime_t;

/// Frequency of the hardware high-precision timer.
#define TIMER_HW_HPTICKS_PER_SEC  1000

/// Not needed, IRQ timer flag cleared automatically.
#define timer_hw_irq() do {} while (0)


#endif /* DRV_TIMER_QT_H */

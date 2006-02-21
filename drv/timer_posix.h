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
 * \brief Low-level timer module for POSIX systems (interface).
 */

/*#*
 *#* $Log$
 *#* Revision 1.2  2006/02/21 21:28:02  bernie
 *#* New time handling based on TIMER_TICKS_PER_SEC to support slow timers with ticks longer than 1ms.
 *#*
 *#* Revision 1.1  2005/11/27 03:58:18  bernie
 *#* Add POSIX timer emulator.
 *#*
 *#* Revision 1.1  2005/11/27 03:06:36  bernie
 *#* Qt timer emulation.
 *#*
 *#*/
#ifndef DRV_TIMER_POSIX_H
#define DRV_TIMER_POSIX_H

// HW dependent timer initialization

#define DEFINE_TIMER_ISR     void timer_isr(UNUSED_ARG(int, arg))
#define TIMER_TICKS_PER_SEC  250
#define TIMER_HW_CNT         (1<<31) /* We assume 32bit integers here */

#include <os/hptime.h>

/// Frequency of the hardware high-precision timer.
#define TIMER_HW_HPTICKS_PER_SEC  HPTIME_TICKS_PER_SECOND

/// Not needed.
#define timer_hw_irq() do {} while (0)


#endif /* DRV_TIMER_QT_H */

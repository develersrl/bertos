/*!
 * \file
 * <!--
 * Copyright 2000 Bernardo Innocenti
 * Copyright 2003,2004 Develer S.r.l. (http://www.develer.com/)
 * This file is part of DevLib - See devlib/README for information.
 * -->
 *
 * \version $Id$
 *
 * \author Bernardo Innocenti <bernie@develer.com>
 *
 * \brief Low-level timer module for AVR
 */

/*#*
 *#* $Log$
 *#* Revision 1.4  2004/12/13 11:51:08  bernie
 *#* DISABLE_INTS/ENABLE_INTS: Convert to IRQ_DISABLE/IRQ_ENABLE.
 *#*
 *#* Revision 1.3  2004/08/25 14:12:08  rasky
 *#* Aggiornato il comment block dei log RCS
 *#*
 *#* Revision 1.2  2004/06/03 11:27:09  bernie
 *#* Add dual-license information.
 *#*
 *#* Revision 1.1  2004/05/23 18:23:30  bernie
 *#* Import drv/timer module.
 *#*
 *#*/

#ifndef TIMER_I196_H
#define TIMER_I196_H

	/*!
	 * Retrigger TIMER2, adjusting the time to account for
	 * the interrupt prologue latency.
	 */
#	define TIMER_RETRIGGER (TIMER2 -= TICKS_RATE)

#	define TIMER_INIT \
		TIMER2 = (65535 - TICKS_RATE); \
		INT_MASK1 |= INT1F_T2OVF; \
		ATOMIC( \
			WSR = 1; \
			IOC3 |= IOC3F_T2_ENA; \
			WSR = 0; \
		)

#define DEFINE_TIMER_ISR \
	INTERRUPT(0x38) void TM2_OVFL_interrupt(void);  \
	INTERRUPT(0x38) void TM2_OVFL_interrupt(void)

#endif /* DRV_TIMER_I196_H */

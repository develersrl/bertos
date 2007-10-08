/**
 * \file
 * <!--
 * Copyright 2006 Develer S.r.l. (http://www.develer.com/)
 * All Rights Reserved.
 * -->
 *
 * \brief Hardware-specific definitions
 *
 * \version $Id$
 *
 * \author Bernardo Innocenti <bernie@develer.com>
 */

/*#*
 *#* $Log$
 *#* Revision 1.1  2006/05/18 00:41:47  bernie
 *#* New triface devlib application.
 *#*
 *#*/

#ifndef AT91SAM7SEK_HW_H
#define AT91SAM7SEK_HW_H

#include <appconfig.h>

/// CPU Clock frequency (14.7456 MHz)
#define CLOCK_FREQ     (14745600UL)


/* Timer IRQ strobe */
//#if CONFIG_TIMER_STROBE
//	#define TIMER_STROBE_ON    ATOMIC(PORTD |= BV(PD0))
//	#define TIMER_STROBE_OFF   ATOMIC(PORTD &= ~BV(PD0))
//	#define TIMER_STROBE_INIT  ATOMIC(PORTD &= ~BV(PD0); DDRD |= BV(PD0))
//#endif /* CONFIG_TIMER_STROBE */

#endif /*  AT91SAM7SEK_HW_H */

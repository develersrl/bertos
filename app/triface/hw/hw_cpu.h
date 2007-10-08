/**
 * \file
 * <!--
 * This file is part of BeRTOS.
 *
 * Bertos is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * As a special exception, you may use this file as part of a free software
 * library without restriction.  Specifically, if other files instantiate
 * templates or use macros or inline functions from this file, or you compile
 * this file and link it with other files to produce an executable, this
 * file does not by itself cause the resulting executable to be covered by
 * the GNU General Public License.  This exception does not however
 * invalidate any other reasons why the executable file might be covered by
 * the GNU General Public License.
 *
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

#ifndef TRIFACE_HW_H
#define TRIFACE_HW_H

#include <appconfig.h>

/// CPU Clock frequency (14.7456 MHz)
#define CLOCK_FREQ     (14745600UL)


/* Timer IRQ strobe */
//#if CONFIG_TIMER_STROBE
//	#define TIMER_STROBE_ON    ATOMIC(PORTD |= BV(PD0))
//	#define TIMER_STROBE_OFF   ATOMIC(PORTD &= ~BV(PD0))
//	#define TIMER_STROBE_INIT  ATOMIC(PORTD &= ~BV(PD0); DDRD |= BV(PD0))
//#endif /* CONFIG_TIMER_STROBE */

#endif /* TRIFACE_HW_H */

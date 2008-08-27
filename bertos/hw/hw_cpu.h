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
 * Copyright 2006, 2008 Develer S.r.l. (http://www.develer.com/)
 * All Rights Reserved.
 * -->
 *
 * \brief Hardware-specific definitions
 *
 * \version $Id$
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 */

#ifndef HW_CPU_H
#define HW_CPU_H

#warning TODO:This is an example implementation, you must implement it!

/// CPU Clock frequency 
#define CLOCK_FREQ     (12288000UL/* Implement me! */)


/* Timer IRQ strobe */
//#if CONFIG_TIMER_STROBE
//	#define TIMER_STROBE_ON    /* Implement me! */
//	#define TIMER_STROBE_OFF   /* Implement me! */
//	#define TIMER_STROBE_INIT  /* Implement me! */
//#endif /* CONFIG_TIMER_STROBE */

#endif /*  HW_CPU_H */

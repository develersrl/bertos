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
 * Copyright 2005 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 * \version $Id$
 *
 * \brief Phase control driver (interface)
 *
 * \version $Id$
 * \author Francesco Sacchi <batt@develer.com>
 */

/*#*
 *#* $Log$
 *#* Revision 1.2  2006/07/19 12:56:26  bernie
 *#* Convert to new Doxygen style.
 *#*
 *#* Revision 1.1  2005/11/04 18:06:44  bernie
 *#* Import into DevLib.
 *#*
 *#* Revision 1.1  2005/05/24 09:17:58  batt
 *#* Move drivers to top-level.
 *#*
 *#* Revision 1.9  2005/05/09 16:34:14  batt
 *#* Change some function names to accomplish coding standard; Add debug phase_initialized; Change duty_t and power_t to uint16_t.
 *#*
 *#* Revision 1.8  2005/05/02 12:37:33  batt
 *#* Split hw triac map in phase_map.h.
 *#*
 *#* Revision 1.7  2005/05/02 09:05:03  batt
 *#* Rename duty_t and power_t in triac_duty_t and triac_power_t
 *#*
 *#* Revision 1.6  2005/04/28 15:10:11  batt
 *#* Use timer API to add and set events.
 *#*
 *#* Revision 1.5  2005/04/28 12:04:46  batt
 *#* Add some comments.
 *#*
 *#* Revision 1.4  2005/04/28 10:35:45  batt
 *#* Complete phase_setpower.
 *#*
 *#* Revision 1.3  2005/04/27 19:23:40  batt
 *#* Reformat.
 *#*
 *#* Revision 1.1  2005/04/27 17:13:56  batt
 *#* Add triac phase control driver.
 *#*
 *#*/


#ifndef DRV_PHASE_H
#define DRV_PHASE_H

#include <drv/timer.h>
#include <phase_map.h>
#include <cfg/debug.h>

#define TRIAC_MAX_DUTY  100
#define TRIAC_MAX_POWER 100
#define TRIAC_POWER_K   TRIAC_MAX_DUTY * (1 / sqrt(2 * TRIAC_MAX_POWER))

/**
 * \name Types for duty and power.
 * \{
 */
typedef uint16_t triac_duty_t;
typedef uint16_t triac_power_t;
/* \} */


DB(extern bool phase_initialized;)

/**
 * \name Type for triac control.
 * \{
 */
typedef struct Triac
{
	Timer  timer;      /**< Timer for phase control. */
	triac_duty_t duty; /**< Duty cycle of the channel. */
	bool   running;    /**< True when the timer is active. */
} Triac;
/* \} */

void phase_setDutyUnlock(TriacDev dev, triac_duty_t duty);
void phase_setDuty(TriacDev dev, triac_duty_t duty);
void phase_setPower(TriacDev dev, triac_power_t power);

void phase_init(void);


#endif /* DRV_PHASE_H */

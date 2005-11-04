/*!
 * \file
 * <!--
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

/*!
 * \name Types for duty and power.
 * \{
 */
typedef uint16_t triac_duty_t;
typedef uint16_t triac_power_t;
/* \} */


DB(extern bool phase_initialized;)

/*!
 * \name Type for triac control.
 * \{
 */
typedef struct Triac
{
	Timer  timer;      /*!< Timer for phase control. */
	triac_duty_t duty; /*!< Duty cycle of the channel. */
	bool   running;    /*!< True when the timer is active. */
} Triac;
/* \} */

void phase_setDutyUnlock(TriacDev dev, triac_duty_t duty);
void phase_setDuty(TriacDev dev, triac_duty_t duty);
void phase_setPower(TriacDev dev, triac_power_t power);

void phase_init(void);


#endif /* DRV_PHASE_H */

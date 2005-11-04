/*!
 * \file
 * <!--
 * Copyright 2005 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 * \version $Id$
 *
 * \brief PWM driver (interface)
 *
 * \version $Id$
 * \author Francesco Sacchi <batt@develer.com>
 */

/*#*
 *#* $Log$
 *#* Revision 1.1  2005/11/04 18:08:49  bernie
 *#* Import into DevLib.
 *#*
 *#* Revision 1.1  2005/05/24 09:17:58  batt
 *#* Move drivers to top-level.
 *#*
 *#* Revision 1.3  2005/05/09 16:36:12  batt
 *#* Change some function names to accomplish coding standard.
 *#*
 *#* Revision 1.2  2005/05/02 16:03:08  batt
 *#* Remove unusefull interrupt saving.
 *#*
 *#* Revision 1.1  2005/05/02 12:36:39  batt
 *#* Add pwm driver.
 *#*
 *#*/
#ifndef DRV_PWM_H
#define DRV_PWM_H

#include <pwm_map.h>
#include <cfg/compiler.h>

typedef uint16_t pwm_duty_t;

void pwm_setDuty(PwmDev dev, pwm_duty_t duty);
void pwm_init(void);

#define PWM_MAX_DUTY 100

#endif /* DRV_PWM_H */

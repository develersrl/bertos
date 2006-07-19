/**
 * \file
 * <!--
 * Copyright 2005 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 * \version $Id$
 *
 * \brief PWM driver (implementation)
 *
 * \version $Id$
 * \author Francesco Sacchi <batt@develer.com>
 */

/*#*
 *#* $Log$
 *#* Revision 1.2  2006/07/19 12:56:26  bernie
 *#* Convert to new Doxygen style.
 *#*
 *#* Revision 1.1  2005/11/04 18:08:49  bernie
 *#* Import into DevLib.
 *#*
 *#* Revision 1.2  2005/05/24 13:35:01  batt
 *#* Add missing; reformat.
 *#*
 *#* Revision 1.1  2005/05/24 09:17:58  batt
 *#* Move drivers to top-level.
 *#*
 *#* Revision 1.4  2005/05/09 16:36:12  batt
 *#* Change some function names to accomplish coding standard.
 *#*
 *#* Revision 1.3  2005/05/02 16:35:47  batt
 *#* Add some comments.
 *#*
 *#* Revision 1.2  2005/05/02 16:02:58  batt
 *#* Remove unusefull interrupt saving.
 *#*
 *#* Revision 1.1  2005/05/02 12:36:39  batt
 *#* Add pwm driver.
 *#*
 *#*/

#include <hw_pwm.h>
#include <drv/pwm.h>
#include <cfg/macros.h>

/**
 * Set duty of pwm channel \a dev.
 */
void pwm_setDuty(PwmDev dev, pwm_duty_t duty)
{
	duty = MIN(duty, (pwm_duty_t)PWM_MAX_DUTY);

	pwm_hw_setDutyUnlock(dev, duty);
}

/**
 * Initialize PWM hw.
 */
void pwm_init(void)
{
	cpuflags_t flags;
	PwmDev dev;

	IRQ_SAVE_DISABLE(flags);

	/* set all pwm to 0 */
	for (dev = 0; dev < PWM_CNT; dev++)
		pwm_setDuty(dev, 0);

	PWM_HW_INIT;

	IRQ_RESTORE(flags);
}

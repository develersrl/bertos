/*!
 * \file
 * <!--
 * Copyright 2005 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 * \version $Id$
 *
 * \brief Phase control driver (implementation)
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
 *#* Revision 1.14  2005/05/20 12:21:05  batt
 *#* Reformat.
 *#*
 *#* Revision 1.13  2005/05/09 16:34:14  batt
 *#* Change some function names to accomplish coding standard; Add debug phase_initialized; Change duty_t and power_t to uint16_t.
 *#*
 *#* Revision 1.12  2005/05/04 17:22:30  batt
 *#* Workaround a Doxygen parsing problem.
 *#*
 *#* Revision 1.11  2005/05/02 09:05:03  batt
 *#* Rename duty_t and power_t in triac_duty_t and triac_power_t
 *#*
 *#* Revision 1.10  2005/05/02 08:48:55  batt
 *#* Disable interrupt only when necessary.
 *#*
 *#* Revision 1.9  2005/04/29 11:52:51  batt
 *#* Remove debug printf; Add a comment.
 *#*
 *#* Revision 1.8  2005/04/29 10:22:56  batt
 *#* Avoid retriggering TRIAC on low duty-cycle.
 *#*
 *#* Revision 1.7  2005/04/29 09:54:36  batt
 *#* Convert to new timer.
 *#*
 *#* Revision 1.6  2005/04/28 17:11:53  batt
 *#* Expand abbreviation.
 *#*
 *#* Revision 1.5  2005/04/28 15:10:11  batt
 *#* Use timer API to add and set events.
 *#*
 *#* Revision 1.4  2005/04/28 12:04:46  batt
 *#* Add some comments.
 *#*
 *#* Revision 1.3  2005/04/28 10:35:45  batt
 *#* Complete phase_setpower.
 *#*
 *#* Revision 1.2  2005/04/27 19:22:49  batt
 *#* Add duty_t, power_t, MAX_DUTY and MAX_POWER
 *#*
 *#* Revision 1.1  2005/04/27 17:13:56  batt
 *#* Add triac phase control driver.
 *#*
 *#*/

#include <drv/timer.h>


#include <cfg/macros.h>
#include <cfg/cpu.h>
#include <cfg/compiler.h>


#include <hw_phase.h>
#include <drv/phase.h>

#include <math.h>

/*! Array  of triacs */
static Triac triacs[TRIAC_CNT];

DB(bool phase_initialized;)

/*!
 * Zerocross interrupt, call when 220V cross zero.
 *
 * This function turn off all triacs that have duty < 100%
 * and arm the triac timers for phase control.
 * This function is frequency adaptive so can work both at 50 or 60Hz.
 */
DEFINE_ZEROCROSS_ISR()
{
	ticks_t period, now;
	static ticks_t prev_time;
	TriacDev dev;

	now = timer_clock_unlocked();
	period = now - prev_time;

	for (dev = 0; dev < TRIAC_CNT; dev++)
	{
		/* Only turn off triac if duty is != 100% */
		if (triacs[dev].duty != TRIAC_MAX_DUTY)
			TRIAC_OFF(dev);
		/* Compute delay from duty */
		timer_setDelay(&triacs[dev].timer, ((period * (TRIAC_MAX_DUTY - triacs[dev].duty) + TRIAC_MAX_DUTY / 2) / TRIAC_MAX_DUTY));

		/* This check avoids inserting the same timer twice
		 * in case of an intempestive zerocross or spike */
		if (triacs[dev].running)
		{
			timer_abort(&triacs[dev].timer);
			//kprintf("[%lu]\n", timer_clock());
		}

		triacs[dev].running = true;
		timer_add(&triacs[dev].timer);
	}
	prev_time = now;
}



/*!
 * Set duty of the triac channel \a dev (interrupt safe).
 */
void phase_setDuty(TriacDev dev, triac_duty_t duty)
{
	cpuflags_t flags;
	IRQ_SAVE_DISABLE(flags);

	phase_setDutyUnlock(dev,duty);

	IRQ_RESTORE(flags);
}



/*!
 * Set duty of the triac channel \a dev (NOT INTERRUPT SAFE).
 */
void phase_setDutyUnlock(TriacDev dev, triac_duty_t duty)
{
	triacs[dev].duty = MIN(duty, (triac_duty_t)TRIAC_MAX_DUTY);
}



/*!
 * Set power of the triac channel \a dev (interrupt safe).
 *
 * This function approsimate the sine wave to a triangular wave to compute
 * RMS power.
 */
void phase_setPower(TriacDev dev, triac_power_t power)
{
	bool greater_fifty = false;
	triac_duty_t duty;

	power = MIN(power, (triac_power_t)TRIAC_MAX_POWER);

	if (power > TRIAC_MAX_POWER / 2)
	{
		greater_fifty = true;
		power = TRIAC_MAX_POWER - power;
	}

	duty = TRIAC_POWER_K * sqrt(power);

	if (greater_fifty)
		duty = TRIAC_MAX_DUTY - duty;
	phase_setDuty(dev, duty);
}



/*!
 * Soft int for each \a _dev triac.
 *
 * The triacs are turned on at different time to achieve phase control.
 */
static void phase_softint(void *_dev)
{
	TriacDev dev = (TriacDev)_dev;

	/* Only turn on if duty is !=0 */
	if (triacs[dev].duty)
		TRIAC_ON(dev);
	triacs[dev].running = false;
}



/*!
 * Initialize phase control driver
 */
void phase_init(void)
{
	cpuflags_t flags;
	TriacDev dev;

	/* Init timers and ensure that all triac are off */
	for (dev = 0; dev < TRIAC_CNT; dev++)
	{
		triacs[dev].duty = 0;
		triacs[dev].running = false;
		SET_TRIAC_DDR(dev);
		TRIAC_OFF(dev);
		timer_set_event_softint(&triacs[dev].timer, (Hook)phase_softint, (void *)dev);
	}
	IRQ_SAVE_DISABLE(flags);

	/* Init zero cross interrupt */
	PHASE_HW_INIT;
	DB(phase_initialized = true;)
	IRQ_RESTORE(flags);
}

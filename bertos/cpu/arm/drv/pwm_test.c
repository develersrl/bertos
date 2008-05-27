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
 *
 * \brief Test for PWM driver (implementation)
 *
 * \version $Id$
 *
 * \author Daniele Basile <asterix@develer.com>
 */

#include <cfg/macros.h>
#include <cfg/debug.h>

#include <cpu/types.h>
#include <cpu/irq.h>

#include <drv/pwm.h>
#include <drv/pwm_at91.h>
#include <drv/timer.h>
#include <drv/sysirq_at91.h>

#include <io/arm.h>

/*
 * Esample of value for duty cycle"
 *
 * - 100% => 0xFFFFFFFF
 * - 80%  => 0xCCCCCCCC
 * - 75%  => 0xBFFFFFFF
 * - 50%  => 0x7FFFFFFF
 * - 25%  => 0x3FFFFFFF
 * - 33%  => 0x55555555
 * - 16%  => 0x2AAAAAAA
 */

#define PWM_TEST_CH0                     0
#define PWM_TEST_CH0_FREQ            100UL // 100Hz
#define PWM_TEST_CH0_DUTY           0xBFFF // 80%

#define PWM_TEST_CH1                     1
#define PWM_TEST_CH1_FREQ           1000UL  // 1000Hz
#define PWM_TEST_CH1_DUTY           0xBFFF  // 75%

#define PWM_TEST_CH2                     2
#define PWM_TEST_CH2_FREQ          12356UL  // 12356Hz
#define PWM_TEST_CH2_DUTY           0x7FFF  // 50%

#define PWM_TEST_CH3                     3
#define PWM_TEST_CH3_FREQ         100000UL  // 100KHz
#define PWM_TEST_CH3_DUTY           0x5555  // 33%

#define PWM_TEST_CH_SET(index) \
	do { \
			pwm_setFrequency(PWM_TEST_CH##index , PWM_TEST_CH##index##_FREQ); \
			pwm_setDuty(PWM_TEST_CH##index, PWM_TEST_CH##index##_DUTY); \
			pwm_enable(PWM_TEST_CH##index, true); \
	} while (0)


/**
 * Setup all needed to test PWM on AT91
 *
 */
int pwm_testSetup(void)
{
	IRQ_ENABLE;
	kdbg_init();
	sysirq_init();
	timer_init();

	pwm_init();

	return 0;
}


/**
 * Test suit for genation of pwm waveform.
 *
 */
int pwm_testRun(void)
{

	kputs("PWM test\n\n");

	kputs("Init pwm..");
	pwm_testSetup();
	kputs("done.\n");

	PWM_TEST_CH_SET(0);
	kprintf("PWM test set ch[%d] =>freq[%ld], duty[%d]\n", PWM_TEST_CH0, PWM_TEST_CH0_FREQ, PWM_TEST_CH0_DUTY);
	PWM_TEST_CH_SET(1);
	kprintf("PWM test set ch[%d] =>freq[%ld], duty[%d]\n", PWM_TEST_CH1, PWM_TEST_CH1_FREQ, PWM_TEST_CH1_DUTY);
	PWM_TEST_CH_SET(2);
	kprintf("PWM test set ch[%d] =>freq[%ld], duty[%d]\n", PWM_TEST_CH2, PWM_TEST_CH2_FREQ, PWM_TEST_CH2_DUTY);
	PWM_TEST_CH_SET(3);
	kprintf("PWM test set ch[%d] =>freq[%ld], duty[%d]\n", PWM_TEST_CH3, PWM_TEST_CH3_FREQ, PWM_TEST_CH3_DUTY);

	return 0;
}

/**
 *
 */
int pwm_testTearDown(void)
{
	/*    */
	return 0;
}

#ifdef _TEST

int main(void)
{
	pwm_testRun();



	kputs("Parto con il test!\n");
	kprintf("PWM CURRENT ch[%d] => cmr[%ld], dty[%ld], prd[%ld], up[%ld]\n", PWM_TEST_CH0, PWM_CMR0, PWM_CDTY0, PWM_CPRD0, PWM_CUPD0);

	for(;;)
	{

		pwm_setDuty(0,0);
		timer_delay(5000);
		kprintf("TEST10 => cmr[%ld], dty[%ld], prd[%ld], up[%ld]\n", PWM_CMR0, PWM_CDTY0, PWM_CPRD0, PWM_CUPD0);


		pwm_setDuty(0,0x7FFF);
		timer_delay(5000);
		kprintf("TEST50 => cmr[%ld], dty[%ld], prd[%ld], up[%ld]\n", PWM_CMR0, PWM_CDTY0, PWM_CPRD0, PWM_CUPD0);


		pwm_setDuty(0,0x5555);
		timer_delay(5000);
		kprintf("TEST33 => cmr[%ld], dty[%ld], prd[%ld], up[%ld]\n", PWM_CMR0, PWM_CDTY0, PWM_CPRD0, PWM_CUPD0);


		pwm_setDuty(0,0xCCCC);
		timer_delay(5000);
		kprintf("TEST80 => cmr[%ld], dty[%ld], prd[%ld], up[%ld]\n", PWM_CMR0, PWM_CDTY0, PWM_CPRD0, PWM_CUPD0);
		kputs("--------\n");


// 		kprintf("PWM test ch[%d] => cmr[%ld], dty[%ld], prd[%ld], up[%ld]\n", PWM_TEST_CH0, PWM_CMR0, PWM_CDTY0, PWM_CPRD0, PWM_CUPD0);
// 		kprintf("PWM test ch[%d] => cmr[%ld], dty[%ld], prd[%ld], up[%ld]\n", PWM_TEST_CH1, PWM_CMR1, PWM_CDTY1, PWM_CPRD1, PWM_CUPD1);
// 		kprintf("PWM test ch[%d] => cmr[%ld], dty[%ld], prd[%ld], up[%ld]\n", PWM_TEST_CH2, PWM_CMR2, PWM_CDTY2, PWM_CPRD2, PWM_CUPD2);
// 		kprintf("PWM test ch[%d] => cmr[%ld], dty[%ld], prd[%ld], up[%ld]\n", PWM_TEST_CH3, PWM_CMR3, PWM_CDTY3, PWM_CPRD3, PWM_CUPD3);
	}

}
#endif



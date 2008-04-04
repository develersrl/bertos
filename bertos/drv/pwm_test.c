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

#include <cpu/types.h>
#include <cpu/irq.h>

#include <drv/pwm.h>
#include <drv/pwm_at91.h>

#include <cfg/macros.h>
#include <cfg/debug.h>

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
#define PWM_TEST_CH0_DUTY           0xCCCC // 80%

#define PWM_TEST_CH1                     1
#define PWM_TEST_CH1_FREQ           1000UL  // 1000Hz
#define PWM_TEST_CH1_DUTY           0xBFFF  // 75%

#define PWM_TEST_CH2                     2
#define PWM_TEST_CH2_FREQ            12356  // 12356Hz
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
 * Test suit for genation of pwm waveform.
 *
 */
void pwm_test(void)
{

	pwm_init();
	kputs("Init pwm..\n");

	PWM_TEST_CH_SET(0);
	kprintf("Set pwm ch[%d] =>freq[%ld], duty[%d]\n", PWM_TEST_CH0, PWM_TEST_CH0_FREQ, PWM_TEST_CH0_DUTY);
	PWM_TEST_CH_SET(1);
	kprintf("Set pwm ch[%d] =>freq[%ld], duty[%d]\n", PWM_TEST_CH1, PWM_TEST_CH1_FREQ, PWM_TEST_CH1_DUTY);
	PWM_TEST_CH_SET(2);
	kprintf("Set pwm ch[%d] =>freq[%ld], duty[%d]\n", PWM_TEST_CH2, PWM_TEST_CH2_FREQ, PWM_TEST_CH2_DUTY);
	PWM_TEST_CH_SET(3);
	kprintf("Set pwm ch[%d] =>freq[%ld], duty[%d]\n", PWM_TEST_CH3, PWM_TEST_CH3_FREQ, PWM_TEST_CH3_DUTY);
}







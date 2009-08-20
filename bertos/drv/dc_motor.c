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
 * Copyright 2008 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 *
 * \brief DC motor driver (implementation)
 *
 * Thi module provide a simple api to controll a DC motor in direction and
 * speed, to allow this we use a  Back-EMF technique.
 *
 * TODO: write a brief..
 *
 * \author Daniele Basile <asterix@develer.com>
 */

#include "dc_motor.h"
#include "hw/hw_dc_motor.h"

// Define logging setting (for cfg/log.h module).
#define LOG_LEVEL         DC_MOTOR_LOG_LEVEL
#define LOG_VERBOSITY     DC_MOTOR_LOG_FORMAT

#include <cfg/log.h>
#include <cfg/debug.h>

#include <algo/pid_control.h>

#include <drv/timer.h>
#include <drv/adc.h>

#include <kern/proc.h>

#include <string.h>

/**
 * DC motor definition.
 */
static DCMotor dcm_all[CONFIG_NUM_DC_MOTOR];

/// Stack process for DC motor poll.
static cpu_stack_t dc_motor_poll_stack[1024];

///< Sample period for all DC motor.
static mtime_t sample_period;

// Only for Debug
LOG_INFOB(static int debug_msg_delay = 0;);

//Forward declaration
static void dc_motor_do(int index);

/**
 * Process to poll DC motor status.
 * To use a Back-EMF technique (see brief for more details),
 * we turn off a motor for max_sample_delay, that value are stored
 * in each DC motor config. For this implementation we assume
 * that have a common max_sample_delay, choose among a max delay
 * to all DC motor configuration.
 * The DC motor off time is choose to allow the out signal to
 * be stable, so we can read and process this value for feedback
 * controll loop.
 * The period (sample_period - max_sample_delay) that every time
 * we turn off a DC motor is choose to have a feedback controll
 * more responsive or less responsive.
 */
static void NORETURN dc_motor_poll(void)
{
	for (;;)
	{
		mtime_t max_sample_delay = 0;
		int i;

		/*
		 * For all DC motor we read and process output singal,
		 * and choose the max value to off time
		 */
		for (i = 0; i < CONFIG_NUM_DC_MOTOR; i++)
		{
			dc_motor_do(i);
			max_sample_delay = MAX(max_sample_delay, dcm_all[i].cfg->sample_delay);
		}

		//Wait for next sampling
		timer_delay(sample_period - max_sample_delay);

		for (i = 0; i < CONFIG_NUM_DC_MOTOR; i++)
		{
			if (dcm_all[i].status & DC_MOTOR_ACTIVE)
			{
				DC_MOTOR_DISABLE(dcm_all[i].index);
				pwm_enable(dcm_all[i].cfg->pwm_dev, false);
			}
		}

		//Wait some time to allow signal to stabilize before sampling
		timer_delay(max_sample_delay);
	}
}

/*
 * Sampling a signal on DC motor and compute
 * a new value of speed according with PID control.
 */
static void dc_motor_do(int index)
{
	DCMotor *dcm = &dcm_all[index];

	dc_speed_t curr_pos;
	pwm_duty_t new_pid;

	//If select DC motor is not active we return
	if (!(dcm->status & DC_MOTOR_ACTIVE))
		return;

	//Acquire the output signal
	curr_pos = ADC_RANGECONV(adc_read(dcm->cfg->adc_ch), dcm->cfg->adc_min, dcm->cfg->adc_max);

	//Compute next value for reaching target speed from current position
	new_pid = pid_control_update(&dcm->pid_ctx, dcm->tgt_speed, curr_pos);


 	LOG_INFOB(if (debug_msg_delay == 20)
 	{
 		LOG_INFO("DCmt[%d]>curr_speed[%d],tgt[%d]\n", dcm->index, curr_pos, dcm->tgt_speed);
 		LOG_INFO("%d,", curr_pos);
 		debug_msg_delay = 0;
 	}
 	debug_msg_delay++;
	kputs("\n"););

 	LOG_WARN("tg[%d], new_pid[%d], pos[%d]\n", dcm->tgt_speed, new_pid, curr_pos);

 	//Apply the compute duty value
	pwm_setDuty(dcm->cfg->pwm_dev, new_pid);

	//Restart dc motor
	pwm_enable(dcm->cfg->pwm_dev, true);

	DC_MOTOR_ENABLE(dcm->index);
}

/**
 * Set spin direction of DC motor.
 *
 * \a index number of DC motor
 * \a dir direction of DC motor
 */
void dc_motor_setDir(int index, bool dir)
{
	DCMotor *dcm = &dcm_all[index];

	if (dir != (dcm->status & DC_MOTOR_DIR))
	{
		//Reset previous direction flag, and set new
		dcm->status &= ~DC_MOTOR_DIR;
		dcm->status |= DC_MOTOR_DIR;

		/*
		 * To set dc motor direction we must also set the
		 * PWM polarity according with dc motor driver chip
		 */
		pwm_setPolarity(dcm->cfg->pwm_dev, dir);
		DC_MOTOR_SET_DIR(index, dir);
	}
}


/**
 * Set DC motor speed.
 */
void dc_motor_setSpeed(int index, dc_speed_t speed)
{
	dcm_all[index].tgt_speed = speed;
}

/**
 * Enable or disable dc motor
 */
void dc_motor_enable(int index, bool state)
{
	DCMotor *dcm = &dcm_all[index];

	/*
	 * Clean all PID stutus variable, becouse
	 * we start with new one.
	 */
	pid_control_reset(&dcm->pid_ctx);

	if (state)
	{
		dcm->status |= DC_MOTOR_ACTIVE;
	}
	else
	{
		pwm_enable(dcm->cfg->pwm_dev, false);
		DC_MOTOR_DISABLE(dcm->index);

		dcm->status &= ~DC_MOTOR_ACTIVE;
	}
}

/**
 * Apply a confinguration to select DC motor.
 */
void dc_motor_setup(int index, DCMotorConfig *dcm_conf)
{
	DCMotor *dcm = &dcm_all[index];

	dcm->cfg = dcm_conf;

	/*
	 * Apply config value.
	 */
	dcm->index = index;

	pwm_setFrequency(dcm->cfg->pwm_dev, dcm->cfg->freq);
	pwm_setPolarity(dcm->cfg->pwm_dev, dcm->cfg->pol);
	pwm_enable(dcm->cfg->pwm_dev, false);

	//Init pid control
	pid_control_init(&dcm->pid_ctx, &dcm->cfg->pid_cfg);

	/*
	 * We are using the same sample period for each
	 * motor, and so we check if this value is the same
	 * for all. The sample period time is defined in pid
	 * configuration.
	 *
	 * TODO: Use a different sample period for each motor
	 * and refactor a module to allow to use a timer interrupt,
	 * in this way we can controll a DC motor also without a
	 * kernel, increasing a portability on other target.
	 */
	if (!sample_period)
		sample_period = dcm->cfg->pid_cfg.sample_period;

	ASSERT(sample_period == dcm->cfg->pid_cfg.sample_period);

	//Set default direction for DC motor
	DC_MOTOR_SET_DIR(dcm->index, dcm->cfg->dir);

	LOG_INFO("DC motort[%d]:\n", dcm->index);
	LOG_INFO("> PID: kp[%lf],ki[%lf],kd[%lf]\n", dcm->cfg->pid_cfg.kd, dcm->cfg->pid_cfg.ki, dcm->cfg->pid_cfg.kd);
	LOG_INFO("> PWM: pwm_dev[%d], freq[%ld], sample[%ld]\n", dcm->cfg->pwm_dev, dcm->cfg->freq, dcm->cfg->sample_delay);
	LOG_INFO("> ADC: adc_ch[%d], adc_max[%d], adc_min[%d]\n", dcm->cfg->adc_ch, dcm->cfg->adc_max, dcm->cfg->adc_min);
	LOG_INFO("> DC: dir[%d], speed[%d]\n", dcm->cfg->dir, dcm->cfg->speed);

}


/**
 * Init DC motor
 */
void dc_motor_init(void)
{
	memset(dcm_all, 0, sizeof(dcm_all));

	//Init a sample period
	sample_period = 0;

	MOTOR_DC_INIT();

	//Create a dc motor poll process
	proc_new(dc_motor_poll, NULL, sizeof(dc_motor_poll_stack), dc_motor_poll_stack);

}


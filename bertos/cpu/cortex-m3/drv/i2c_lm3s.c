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
 * Copyright 2010 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief Driver for the LM3S I2C (implementation)
 *
 */

#include "cfg/cfg_i2c.h"

#define LOG_LEVEL  I2C_LOG_LEVEL
#define LOG_FORMAT I2C_LOG_FORMAT

#include <cfg/log.h>

#include <cfg/debug.h>
#include <cfg/macros.h> // BV()
#include <cfg/module.h>

#include <cpu/detect.h>
#include <cpu/irq.h>

#include <io/cm3_types.h>
#include <io/lm3s.h>

#include <drv/timer.h>
#include <drv/i2c.h>
#include <drv/gpio_lm3s.h>
#include <drv/clock_lm3s.h>

/*
 *
 */
#if 0
	/* I2C 0 */
	#define I2C                        I2C0_MASTER_BASE
	#define SYSCTL_RCGC1_I2C           SYSCTL_RCGC1_I2C0
	#define SYSCTL_RCGC2_GPIO          SYSCTL_RCGC2_GPIOB
	#define GPIO_I2C_SCL_PIN           GPIO_I2C0_SCL_PIN
	#define GPIO_I2C_SDA_PIN           GPIO_I2C0_SDA_PIN
	#define GPIO_PORT_BASE             GPIO_PORTB_BASE
#else
	/* I2C 1 */
	#define I2C                        I2C1_MASTER_BASE
	#define SYSCTL_RCGC1_I2C           SYSCTL_RCGC1_I2C1
	#define SYSCTL_RCGC2_GPIO          SYSCTL_RCGC2_GPIOA
	#define GPIO_I2C_SCL_PIN           GPIO_I2C1_SCL_PIN
	#define GPIO_I2C_SDA_PIN           GPIO_I2C1_SDA_PIN
	#define GPIO_PORT_BASE             GPIO_PORTA_BASE
#endif


/**
 * Send START condition and select slave for write.
 * \c id is the device id comprehensive of address left shifted by 1.
 * The LSB of \c id is ignored and reset to 0 for write operation.
 *
 * \return true on success, false otherwise.
 */
bool i2c_builtin_start_w(uint8_t id)
{
	HWREG(I2C + I2C_O_MSA) = id & ~BV(0);
	return true;
}


/**
 * Send START condition and select slave for read.
 * \c id is the device id comprehensive of address left shifted by 1.
 * The LSB of \c id is ignored and set to 1 for read operation.
 *
 * \return true on success, false otherwise.
 */
bool i2c_builtin_start_r(uint8_t id)
{
	HWREG(I2C + I2C_O_MSA) = id | 1;
	return true;
}


void i2c_builtin_stop(void)
{
}


bool i2c_builtin_put(const uint8_t data)
{
	(void)data;
	return true;
}


int i2c_builtin_get(bool ack)
{
	(void)ack;
	return 0;
}

INLINE bool check_ack(uint32_t mode_mask)
{
	ticks_t start = timer_clock();
	while ( HWREG(I2C + I2C_O_MCS) &  I2C_MCS_ADRACK )
	{
		if (timer_clock() - start > ms_to_ticks(CONFIG_I2C_START_TIMEOUT))
		{
			LOG_ERR("Timeout on I2C_START\n");
			return false;
		}

		HWREG(I2C + I2C_O_MCS) = I2C_MASTER_CMD_BURST_SEND_ERROR_STOP;

		HWREG(I2C + I2C_O_MCS) = mode_mask;
		while( HWREG(I2C + I2C_O_MCS) & I2C_MCS_BUSY );

	}

	return true;
}

/*
 * With this function is allowed only the atomic write.
 */
bool i2c_send(const void *_buf, size_t count)
{
	const uint8_t *buf = (const uint8_t *)_buf;

	if (count == 1)
	{
		HWREGB(I2C + I2C_O_MDR) = *buf;

		HWREG(I2C + I2C_O_MCS) = I2C_MASTER_CMD_SINGLE_SEND;

		while( HWREG(I2C + I2C_O_MCS) & I2C_MCS_BUSY );

		if ( !check_ack(I2C_MASTER_CMD_SINGLE_SEND) )
			return false;

		count = 0;
		buf++;
	}

	if (count > 1)
	{
		HWREGB(I2C + I2C_O_MDR) = *buf++;
		count--;

		HWREG(I2C + I2C_O_MCS) = I2C_MASTER_CMD_BURST_SEND_START;

		while( HWREG(I2C + I2C_O_MCS) & I2C_MCS_BUSY );

		if ( !check_ack(I2C_MASTER_CMD_BURST_SEND_START) )
			return false;

		while(count - 1)
		{
			HWREGB(I2C + I2C_O_MDR) = *buf++;
			count--;
			HWREG(I2C + I2C_O_MCS) = I2C_MASTER_CMD_BURST_SEND_CONT;
			while( HWREG(I2C + I2C_O_MCS) & I2C_MCS_BUSY );

		}

		HWREGB(I2C + I2C_O_MDR) = *buf++;

		HWREG(I2C + I2C_O_MCS) = I2C_MASTER_CMD_BURST_SEND_FINISH;
		while( HWREG(I2C + I2C_O_MCS) & I2C_MCS_BUSY );
	}

	return true;
}

/**
 * In order to read bytes from the i2c we should make some tricks.
 */
bool i2c_recv(void *_buf, size_t count)
{
	uint8_t *buf = (uint8_t *)_buf;

	if (count == 1)
	{
		HWREG(I2C + I2C_O_MCS) = I2C_MASTER_CMD_SINGLE_RECEIVE;
		while( HWREG(I2C + I2C_O_MCS) & I2C_MCS_BUSY );

		if ( !check_ack(I2C_MASTER_CMD_SINGLE_RECEIVE) )
			return false;

		*buf++ = HWREG(I2C + I2C_O_MDR);
		count = 0;
	}

	if (count > 1)
	{

		HWREG(I2C + I2C_O_MCS) = I2C_MASTER_CMD_BURST_RECEIVE_START;
		while( HWREG(I2C + I2C_O_MCS) & I2C_MCS_BUSY );

		if ( !check_ack(I2C_MASTER_CMD_BURST_RECEIVE_START) )
			return false;

		*buf++ = (uint8_t)HWREG(I2C + I2C_O_MDR);
		count--;

		while(count - 1)
		{

			HWREG(I2C + I2C_O_MCS) = I2C_MASTER_CMD_BURST_RECEIVE_CONT;
			while( HWREG(I2C + I2C_O_MCS) & I2C_MCS_BUSY );
			*buf++ = (uint8_t)HWREG(I2C + I2C_O_MDR);
			count--;
		}

		HWREG(I2C + I2C_O_MCS) = I2C_MASTER_CMD_BURST_RECEIVE_FINISH;
		while( HWREG(I2C + I2C_O_MCS) & I2C_MCS_BUSY );

		*buf++ = (uint8_t)HWREG(I2C + I2C_O_MDR);
		count--;
	}

	return true;
}

MOD_DEFINE(i2c);

/**
 * Initialize I2C module.
 */
void i2c_builtin_init(void)
{

	/* Enable the peripheral clock */
	SYSCTL_RCGC1_R |= SYSCTL_RCGC1_I2C;
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIO;

	/* Configure GPIO pins to work as I2C pins */
	lm3s_gpioPinConfig(GPIO_PORT_BASE, GPIO_I2C_SCL_PIN,
		GPIO_DIR_MODE_HW, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_OD_WPU);

	lm3s_gpioPinConfig(GPIO_PORT_BASE, GPIO_I2C_SDA_PIN,
		GPIO_DIR_MODE_HW, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_OD_WPU);

    /*
	 * Compute the clock divider that achieves the fastest speed less than or
     * equal to the desired speed.  The numerator is biased to favor a larger
     * clock divider so that the resulting clock is always less than or equal
     * to the desired clock, never greater.
	 */
    HWREG(I2C + I2C_O_MTPR) = ((CPU_FREQ + (2 * 10 * CONFIG_I2C_FREQ) - 1) / (2 * 10 * CONFIG_I2C_FREQ)) - 1;


	//Enable I2C in master mode
	HWREG(I2C + I2C_O_MCR) |= I2C_MCR_MFE;

	MOD_INIT(i2c);
}

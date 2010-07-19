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
 * \brief Driver for the LPC23xx I2C (implementation)
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

#include <drv/timer.h>
#include <drv/i2c.h>
#include <drv/vic_lpc2.h> /* vic_handler_t */

#include <io/lpc23xx.h>


#define I2C_READBIT                      BV(0)

/*
 *
 */
#if 1
	/* I2C 0 */

	#define I2C_PCONP                    PCONP_PCI2C0
	#define I2C_CONSET                   I20CONSET
	#define I2C_CONCLR                   I20CONCLR
	#define I2C_SCLH                     I20SCLH
	#define I2C_SCLL                     I20SCLL
	#define I2C_STAT                     I20STAT
	#define I2C_DAT                      I20DAT
	#define I2C_SDA_PINSEL_MASK          I2C_SDA0_PINSEL_MASK
	#define I2C_SCL_PINSEL_MASK          I2C_SCL0_PINSEL_MASK
	#define I2C_SDA_PINSEL               I2C_SDA0_PINSEL
	#define I2C_SCL_PINSEL               I2C_SCL0_PINSEL
#else
	/* I2C 1 */
#endif

#define GET_STATUS()   (I2C_STAT)

static uint8_t i2c_builtin_start(void)
{
	I2C_CONSET = BV(I2CON_STA) | BV(I2CON_AA);

	while( !(I2C_CONSET & BV(I2CON_SI)) );

	return true;
}


/**
 * Send START condition and select slave for write.
 * \c id is the device id comprehensive of address left shifted by 1.
 * The LSB of \c id is ignored and reset to 0 for write operation.
 *
 * \return true on success, false otherwise.
 */
bool i2c_builtin_start_w(uint8_t id)
{
	ticks_t start = timer_clock();
	while (i2c_builtin_start())
	{
		uint32_t status = GET_STATUS();

		if ((status == I2C_STAT_SEND) || (status == I2C_STAT_RESEND))
			I2C_DAT = id & ~I2C_READBIT;

		/* Clear the start bit and clear the SI bit */
		I2C_CONCLR = BV(I2CON_SIC) | BV(I2CON_STAC);

		if (status == I2C_STAT_SLAW_ACK)
			return true;
		else if (status == I2C_STAT_ARB_LOST)
			break;
		else if (timer_clock() - start > ms_to_ticks(CONFIG_I2C_START_TIMEOUT))
		{
			LOG_ERR("Timeout on I2C START\n");
			break;
		}
	}
	return false;
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
	if (i2c_builtin_start())
	{
		uint32_t status = GET_STATUS();

		if ((status == I2C_STAT_SEND) || (status == I2C_STAT_RESEND))
			I2C_DAT = id | I2C_READBIT;

		/* Clear the start bit and clear the SI bit */
		I2C_CONCLR = BV(I2CON_SIC) | BV(I2CON_STAC);

		while( !(I2C_CONSET & BV(I2CON_SI)) );

		status = GET_STATUS();

		if (status == I2C_STAT_SLAR_ACK)
			return true;
		else if (status == I2C_STAT_ARB_LOST)
			return false;

	}
	return false;
}


void i2c_builtin_stop(void)
{
	I2C_CONSET = BV(I2CON_STO);
	I2C_CONCLR = BV(I2CON_STAC) | BV(I2CON_SIC) | BV(I2CON_AAC);
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

/*
 * With this function is allowed only the atomic write.
 */
bool i2c_send(const void *_buf, size_t count)
{
	const uint8_t *buf = (const uint8_t *)_buf;
	uint32_t status = 0;

	while (count)
	{
		I2C_DAT = *buf++;
		I2C_CONSET = BV(I2CON_AA);
		I2C_CONCLR = BV(I2CON_SIC);
		count--;

		while( !(I2C_CONSET & BV(I2CON_SI)) );

		status = GET_STATUS();
		if (status == I2C_STAT_DATA_ACK)
			continue;
		else if (status == I2C_STAT_DATA_NACK)
		{
			LOG_ERR("send:%02x\n", (uint8_t)status);
			return false;
		}
		else if (status == 0xf8)
		{
			LOG_ERR("send:%02x\n", (uint8_t)status);
			return false;
		}
		else if (status == 0x10)
		{
			LOG_ERR("send:%02x\n", (uint8_t)status);
			return false;
		}

	}

	return true;
}

/**
 * In order to read bytes from the i2c we should make some tricks.
 */
bool i2c_recv(void *_buf, size_t count)
{
	uint8_t *buf = (uint8_t *)_buf;
	uint8_t status = GET_STATUS();

//	LOG_ERR("recv:%02x\n", (uint8_t)status);

	while (count)
	{
		*buf++ = I2C_DAT;
		if (count > 1)
			I2C_CONSET = BV(I2CON_AA);
		else
			I2C_CONCLR = BV(I2CON_AAC);

		I2C_CONCLR = BV(I2CON_SIC);
		count--;

		while( !(I2C_CONSET & BV(I2CON_SI)) );

		status = GET_STATUS();

		if (status == I2C_STAT_RDATA_ACK)
			continue;
		else if (status == I2C_STAT_RDATA_NACK)
		{
		//	LOG_ERR("recv:%02x\n", (uint8_t)status);
			return true;
		}
		else if (status == 0xf8)
		{
			LOG_ERR("recv:%02x\n", (uint8_t)status);
			return false;
		}
		else if (status == 0x10)
		{
			LOG_ERR("recv:%02x\n", (uint8_t)status);
			return false;
		}
	}

	return true;
}

MOD_DEFINE(i2c);

/**
 * Initialize I2C module.
 */
void i2c_builtin_init(void)
{
	/* Enable I2C clock */
	PCONP |= BV(I2C_PCONP);

	#if (CONFIG_I2C_FREQ > 400000)
		#error i2c frequency is to hight.
	#endif

	I2C_CONCLR = BV(I2CON_I2ENC) | BV(I2CON_STAC) | BV(I2CON_SIC) | BV(I2CON_AAC);

	/*
	 * Bit Frequency = Fplk / (I2C_I2SCLH + I2C_I2SCLL)
	 * value of I2SCLH and I2SCLL must be different
	 */
	PCLKSEL0 &= ~I2C0_PCLK_MASK;
	PCLKSEL0 |= I2C0_PCLK_DIV8;

	I2C_SCLH = (((CPU_FREQ / 8) / CONFIG_I2C_FREQ) / 2) + 1;
	I2C_SCLL = (((CPU_FREQ / 8) / CONFIG_I2C_FREQ) / 2);

	ASSERT(I2C_SCLH > 4 || I2C_SCLL > 4);

	/* Assign pins to SCL and SDA (P0_27, P0_28) */
	PINSEL1 &= ~I2C_PINSEL_MASK;
	PINSEL1 |= I2C_PINSEL;

	// Enable I2C
	I2C_CONSET = BV(I2CON_I2EN);

	MOD_INIT(i2c);
}

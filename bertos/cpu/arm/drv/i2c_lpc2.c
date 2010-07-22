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
	#define I2C_PINSEL_PORT              PINSEL1
	#define I2C_PINSEL                   I2C0_PINSEL
	#define I2C_PINSEL_MASK              I2C0_PINSEL_MASK
	#define I2C_PCLKSEL                  PCLKSEL0
	#define I2C_PCLK_MASK                I2C0_PCLK_MASK
	#define I2C_PCLK_DIV8                I2C0_PCLK_DIV8
#else
	/* I2C 1 */
	#error
#endif

#define GET_STATUS()   ((uint8_t)I2C_STAT)
/*
 * Wait that SI bit is set.
 *
 * Note: this bit is set when the I2C state changes. However, entering
 * state F8 does not set SI since there is nothing for an interrupt service
 * routine to do in that case.
 */
#define WAIT_SI()           while( !(I2C_CONSET & BV(I2CON_SI)) )

static uint8_t i2c_builtin_start(void)
{
	// Clear all pending flags.
	I2C_CONCLR = BV(I2CON_STAC) | BV(I2CON_SIC) | BV(I2CON_AAC);

	// Set start and ack bit.
	I2C_CONSET = BV(I2CON_STA) | BV(I2CON_AA);

	WAIT_SI();

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
		uint8_t status = GET_STATUS();

		/* Start status ok, set addres and the R/W bit */
		if ((status == I2C_STAT_SEND) || (status == I2C_STAT_RESEND))
			I2C_DAT = id & ~I2C_READBIT;

		/* Clear the start bit and clear the SI bit */
		I2C_CONCLR = BV(I2CON_SIC) | BV(I2CON_STAC);

		if (status == I2C_STAT_SLAW_ACK)
			return true;
		else if (status == I2C_STAT_ARB_LOST)
		{
			LOG_ERR("Arbitration lost\n");
			break;
		}
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
		uint8_t status = GET_STATUS();

		/* Start status ok, set addres and the R/W bit */
		if ((status == I2C_STAT_SEND) || (status == I2C_STAT_RESEND))
			I2C_DAT = id | I2C_READBIT;

		/* Clear the start bit and clear the SI bit */
		I2C_CONCLR = BV(I2CON_SIC) | BV(I2CON_STAC);

		WAIT_SI();

		status = GET_STATUS();

		if (status == I2C_STAT_SLAR_ACK)
			return true;

		else if (status == I2C_STAT_SLAR_ACK)
		{
			LOG_ERR("SLAR NACK:%02x\n", status);
		}
		else if (status == I2C_STAT_ARB_LOST)
		{
			LOG_ERR("ARB Lost:%02x\n", status);
		}

	}
	return false;
}


void i2c_builtin_stop(void)
{
	/* Set the stop bit */
	I2C_CONSET = BV(I2CON_STO);
	/* Clear pending flags */
	I2C_CONCLR = BV(I2CON_STAC) | BV(I2CON_SIC) | BV(I2CON_AAC);
}


bool i2c_builtin_put(const uint8_t data)
{
	I2C_DAT = data;
	I2C_CONCLR = BV(I2CON_SIC);

	WAIT_SI();

	uint32_t status = GET_STATUS();

	if (status == I2C_STAT_DATA_ACK)
		return true;
	else if (status == I2C_STAT_DATA_NACK)
	{
		LOG_ERR("Data NACK\n");
		return false;
	}
	else if (status == I2C_STAT_ERROR)
	{
		LOG_ERR("I2C error.\n");
		return false;
	}
	else if (status == I2C_STAT_UNKNOW)
	{
		LOG_ERR("I2C unable to read status.\n");
		return false;
	}

	return false;
}


int i2c_builtin_get(bool ack)
{

	/*
	 * Set ack bit if we want read more byte, otherwise
	 * we disable it
	 */
	if (ack)
		I2C_CONSET = BV(I2CON_AA);
	else
		I2C_CONCLR = BV(I2CON_AAC);

	I2C_CONCLR = BV(I2CON_SIC);

	WAIT_SI();

	uint32_t status = GET_STATUS();

	if (status == I2C_STAT_RDATA_ACK)
		return (uint8_t)I2C_DAT;
	else if (status == I2C_STAT_RDATA_NACK)
		return true;
	else if (status == I2C_STAT_ERROR)
	{
		LOG_ERR("I2C error.\n");
		return EOF;
	}
	else if (status == I2C_STAT_UNKNOW)
	{
		LOG_ERR("I2C unable to read status.\n");
		return EOF;
	}

	return EOF;
}

/*
 * With this function is allowed only the atomic write.
 */
static bool i2c_send1(const void *_buf, size_t count)
{
	const uint8_t *buf = (const uint8_t *)_buf;
	uint8_t status = 0;

	while (count)
	{
		I2C_DAT = *buf++;
		I2C_CONCLR = BV(I2CON_SIC);
		count--;

		WAIT_SI();

		status = GET_STATUS();

		if (status == I2C_STAT_DATA_ACK)
			continue;
		else if (status == I2C_STAT_DATA_NACK)
		{
			LOG_ERR("Data NACK\n");
			return false;
		}
		else if (status == I2C_STAT_ERROR)
		{
			LOG_ERR("I2C error.\n");
			return false;
		}
		else if (status == I2C_STAT_UNKNOW)
		{
			LOG_ERR("I2C unable to read status.\n");
			return false;
		}

	}

	return true;
}

/**
 * In order to read bytes from the i2c we should make some tricks.
 */
static bool i2c_recv1(void *_buf, size_t count)
{
	uint8_t *buf = (uint8_t *)_buf;
	uint8_t status = GET_STATUS();

	/* Ready for read */
	I2C_CONSET = BV(I2CON_AA);
	I2C_CONCLR = BV(I2CON_SIC);

	WAIT_SI();

	while (count)
	{
		*buf++ = I2C_DAT;
		/*
		 * Set ack bit if we want read more byte, otherwise
		 * we disable it
		 */
		if (count > 1)
			I2C_CONSET = BV(I2CON_AA);
		else
			I2C_CONCLR = BV(I2CON_AAC);

		I2C_CONCLR = BV(I2CON_SIC);
		count--;

		WAIT_SI();

		status = GET_STATUS();

		if (status == I2C_STAT_RDATA_ACK)
			continue;
		else if (status == I2C_STAT_RDATA_NACK)
			return true;
		else if (status == I2C_STAT_ERROR)
		{
			LOG_ERR("I2C error.\n");
			return false;
		}
		else if (status == I2C_STAT_UNKNOW)
		{
			LOG_ERR("I2C unable to read status.\n");
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
	I2C_PCLKSEL &= ~I2C_PCLK_MASK;
	I2C_PCLKSEL |= I2C_PCLK_DIV8;

	I2C_SCLH = (((CPU_FREQ / 8) / CONFIG_I2C_FREQ) / 2) + 1;
	I2C_SCLL = (((CPU_FREQ / 8) / CONFIG_I2C_FREQ) / 2);

	ASSERT(I2C_SCLH > 4 || I2C_SCLL > 4);

	/* Assign pins to SCL and SDA (P0_27, P0_28) */
	I2C_PINSEL_PORT &= ~I2C_PINSEL_MASK;
	I2C_PINSEL_PORT |= I2C_PINSEL;

	// Enable I2C
	I2C_CONSET = BV(I2CON_I2EN);

	MOD_INIT(i2c);
}

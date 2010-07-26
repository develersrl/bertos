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

#include <stdarg.h>


struct I2cHardware
{
	int dev;
};

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
#define WAIT_SI() \
	do { \
		ticks_t start = timer_clock(); \
		while( !(I2C_CONSET & BV(I2CON_SI)) ) \
		{ \
			if (timer_clock() - start > ms_to_ticks(CONFIG_I2C_START_TIMEOUT)) \
			{ \
				LOG_ERR("Timeout SI assert\n"); \
				break; \
			} \
		} \
	} while (0)

static void i2c_hw_restart(void)
{
	// Clear all pending flags.
	I2C_CONCLR = BV(I2CON_STAC) | BV(I2CON_SIC) | BV(I2CON_AAC);

	// Set start and ack bit.
	I2C_CONSET = BV(I2CON_STA);

	WAIT_SI();
}


static void i2c_hw_stop(void)
{
	/* Set the stop bit */
	I2C_CONSET = BV(I2CON_STO);
	/* Clear pending flags */
	I2C_CONCLR = BV(I2CON_STAC) | BV(I2CON_SIC) | BV(I2CON_AAC);
}

static void i2c_lpc2_put(I2c *i2c, uint8_t data)
{
	I2C_DAT = data;
	I2C_CONCLR = BV(I2CON_SIC);

	WAIT_SI();

	uint32_t status = GET_STATUS();


	/* Generate the stop if we finish to send all programmed bytes */
	if (i2c->xfer_size == 1)
	{
		if (I2C_TEST_STOP(i2c->flags) == I2C_STOP)
			i2c_hw_stop();
	}

	if (status == I2C_STAT_DATA_NACK)
	{
		LOG_ERR("Data NACK\n");
		i2c->errors |= I2C_NO_ACK;
		i2c_hw_stop();
	}
	else if ((status == I2C_STAT_ERROR) || (status == I2C_STAT_UNKNOW))
	{
		LOG_ERR("I2C error.\n");
		i2c->errors |= I2C_ERR;
		i2c_hw_stop();
	}
}

static uint8_t i2c_lpc2_get(I2c *i2c)
{
	/*
	 * Set ack bit if we want read more byte, otherwise
	 * we disable it
	 */
	if (i2c->xfer_size > 1)
		I2C_CONSET = BV(I2CON_AA);
	else
		I2C_CONCLR = BV(I2CON_AAC);

	I2C_CONCLR = BV(I2CON_SIC);

	WAIT_SI();

	uint32_t status = GET_STATUS();
	uint8_t data = (uint8_t)(I2C_DAT & 0xFF);

	if (status == I2C_STAT_RDATA_ACK)
	{
		return data;
	}
	else if (status == I2C_STAT_RDATA_NACK)
	{
		/*
		 * last byte to read generate the stop if
		 * required
		 */
		if (I2C_TEST_STOP(i2c->flags) == I2C_STOP)
			i2c_hw_stop();

		return data;
	}
	else if ((status == I2C_STAT_ERROR) || (status == I2C_STAT_UNKNOW))
	{
		LOG_ERR("I2C error.\n");
		i2c->errors |= I2C_ERR;
		i2c_hw_stop();
	}

	return 0xFF;
}

MOD_DEFINE(i2c);

static void i2c_lpc2_start(struct I2c *i2c, uint16_t slave_addr)
{
	if (I2C_TEST_START(i2c->flags) == I2C_START_W)
	{
		ticks_t start = timer_clock();
		while (true)
		{
			i2c_hw_restart();

			uint8_t status = GET_STATUS();

			/* Start status ok, set addres and the R/W bit */
			if ((status == I2C_STAT_SEND) || (status == I2C_STAT_RESEND))
				I2C_DAT = slave_addr & ~I2C_READBIT;

			/* Clear the start bit and clear the SI bit */
			I2C_CONCLR = BV(I2CON_SIC) | BV(I2CON_STAC);

			if (status == I2C_STAT_SLAW_ACK)
				break;

			if (status == I2C_STAT_ARB_LOST)
				goto error;

			if (timer_clock() - start > ms_to_ticks(CONFIG_I2C_START_TIMEOUT))
			{
				LOG_ERR("Timeout on I2C START\n");
				i2c->errors |= I2C_NO_ACK;
				i2c_hw_stop();
				break;
			}
		}
	}
	else if (I2C_TEST_START(i2c->flags) == I2C_START_R)
	{
		i2c_hw_restart();

		uint8_t status = GET_STATUS();

		/* Start status ok, set addres and the R/W bit */
		if ((status == I2C_STAT_SEND) || (status == I2C_STAT_RESEND))
			I2C_DAT = slave_addr | I2C_READBIT;

		/* Clear the start bit and clear the SI bit */
		I2C_CONCLR = BV(I2CON_SIC) | BV(I2CON_STAC);

		WAIT_SI();

		status = GET_STATUS();

		if (status == I2C_STAT_SLAR_NACK)
		{
			LOG_ERR("SLAR NACK:%02x\n", status);
			i2c->errors |= I2C_NO_ACK;
			i2c_hw_stop();
		}

		if (status == I2C_STAT_ARB_LOST)
			goto error;
	}
	else
	{
		ASSERT(0);
	}

error:
	LOG_ERR("Arbitration lost\n");
	i2c->errors |= I2C_ARB_LOST;
	i2c_hw_stop();
}

static const I2cVT i2c_lpc_vt =
{
	.start = i2c_lpc2_start,
	.get = i2c_lpc2_get,
	.put = i2c_lpc2_put,
	.send = i2c_swSend,
	.recv = i2c_swRecv,
};

struct I2cHardware i2c_lpc2_hw =
{
	.dev = 0,
};


/**
 * Initialize I2C module.
 */
void i2c_hw_init(I2c *i2c, int dev, uint32_t clock)
{
	i2c->hw = &i2c_lpc2_hw;
	i2c->vt = &i2c_lpc_vt;

	/* Enable I2C clock */
	PCONP |= BV(I2C_PCONP);

	ASSERT(clock <= 400000);

	I2C_CONCLR = BV(I2CON_I2ENC) | BV(I2CON_STAC) | BV(I2CON_SIC) | BV(I2CON_AAC);

	/*
	 * Bit Frequency = Fplk / (I2C_I2SCLH + I2C_I2SCLL)
	 * value of I2SCLH and I2SCLL must be different
	 */
	I2C_PCLKSEL &= ~I2C_PCLK_MASK;
	I2C_PCLKSEL |= I2C_PCLK_DIV8;

	I2C_SCLH = (((CPU_FREQ / 8) / clock) / 2) + 1;
	I2C_SCLL = (((CPU_FREQ / 8) / clock) / 2);

	ASSERT(I2C_SCLH > 4 || I2C_SCLL > 4);

	/* Assign pins to SCL and SDA (P0_27, P0_28) */
	I2C_PINSEL_PORT &= ~I2C_PINSEL_MASK;
	I2C_PINSEL_PORT |= I2C_PINSEL;

	// Enable I2C
	I2C_CONSET = BV(I2CON_I2EN);

	MOD_INIT(i2c);
}

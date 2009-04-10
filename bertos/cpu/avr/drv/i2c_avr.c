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
 * Copyright 2003, 2004, 2005 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief Driver for the AVR ATMega TWI (implementation)
 *
 * \version $Id$
 *
 * \author Stefano Fedrigo <aleph@develer.com>
 * \author Bernie Innocenti <bernie@codewiz.org>
 */

#include <hw/hw_cpufreq.h>  /* CPU_FREQ */

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

#include <compat/twi.h>


/* Wait for TWINT flag set: bus is ready */
#define WAIT_TWI_READY  do {} while (!(TWCR & BV(TWINT)))

/**
 * Send START condition on the bus.
 *
 * \return true on success, false otherwise.
 */
static bool i2c_builtin_start(void)
{
	TWCR = BV(TWINT) | BV(TWSTA) | BV(TWEN);
	WAIT_TWI_READY;

	if (TW_STATUS == TW_START || TW_STATUS == TW_REP_START)
		return true;

	LOG_ERR("!TW_(REP)START: %x\n", TWSR);
	return false;
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
	/*
	 * Loop on the select write sequence: when the eeprom is busy
	 * writing previously sent data it will reply to the SLA_W
	 * control byte with a NACK.  In this case, we must
	 * keep trying until the eeprom responds with an ACK.
	 */
	ticks_t start = timer_clock();
	while (i2c_builtin_start())
	{
		TWDR = id & ~I2C_READBIT;
		TWCR = BV(TWINT) | BV(TWEN);
		WAIT_TWI_READY;

		if (TW_STATUS == TW_MT_SLA_ACK)
			return true;
		else if (TW_STATUS != TW_MT_SLA_NACK)
		{
			LOG_ERR("!TW_MT_SLA_(N)ACK: %x\n", TWSR);
			break;
		}
		else if (timer_clock() - start > ms_to_ticks(CONFIG_I2C_START_TIMEOUT))
		{
			LOG_ERR("Timeout on TWI_MT_START\n");
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
		TWDR = id | I2C_READBIT;
		TWCR = BV(TWINT) | BV(TWEN);
		WAIT_TWI_READY;

		if (TW_STATUS == TW_MR_SLA_ACK)
			return true;

		LOG_ERR("!TW_MR_SLA_ACK: %x\n", TWSR);
	}

	return false;
}


/**
 * Send STOP condition.
 */
void i2c_builtin_stop(void)
{
        TWCR = BV(TWINT) | BV(TWEN) | BV(TWSTO);
}


/**
 * Put a single byte in master transmitter mode
 * to the selected slave device through the TWI bus.
 *
 * \return true on success, false on error.
 */
bool i2c_builtin_put(const uint8_t data)
{
	TWDR = data;
	TWCR = BV(TWINT) | BV(TWEN);
	WAIT_TWI_READY;
	if (TW_STATUS != TW_MT_DATA_ACK)
	{
		LOG_ERR("!TW_MT_DATA_ACK: %x\n", TWSR);
		return false;
	}
	return true;
}

/**
 * Get 1 byte from slave in master transmitter mode
 * to the selected slave device through the TWI bus.
 * If \a ack is true issue a ACK after getting the byte,
 * otherwise a NACK is issued.
 *
 * \return the byte read if ok, EOF on errors.
 */
int i2c_builtin_get(bool ack)
{
	TWCR = BV(TWINT) | BV(TWEN) | (ack ? BV(TWEA) : 0);
	WAIT_TWI_READY;

	if (ack)
	{
		if (TW_STATUS != TW_MR_DATA_ACK)
		{
			LOG_ERR("!TW_MR_DATA_ACK: %x\n", TWSR);
			return EOF;
		}
	}
	else
	{
		if (TW_STATUS != TW_MR_DATA_NACK)
		{
			LOG_ERR("!TW_MR_DATA_NACK: %x\n", TWSR);
			return EOF;
		}
	}

	/* avoid sign extension */
	return (int)(uint8_t)TWDR;
}

MOD_DEFINE(i2c);

/**
 * Initialize TWI module.
 */
void i2c_builtin_init(void)
{
	ATOMIC(
		/*
		 * This is pretty useless according to AVR's datasheet,
		 * but it helps us driving the TWI data lines on boards
		 * where the bus pull-up resistors are missing.  This is
		 * probably due to some unwanted interaction between the
		 * port pin and the TWI lines.
		 */
#if CPU_AVR_ATMEGA64 || CPU_AVR_ATMEGA128 || CPU_AVR_ATMEGA1281
		PORTD |= BV(PD0) | BV(PD1);
		DDRD  |= BV(PD0) | BV(PD1);
#elif CPU_AVR_ATMEGA8
		PORTC |= BV(PC4) | BV(PC5);
		DDRC  |= BV(PC4) | BV(PC5);
#elif CPU_AVR_ATMEGA32
		PORTC |= BV(PC1) | BV(PC0);
		DDRC  |= BV(PC1) | BV(PC0);
#else
		#error Unsupported architecture
#endif

		/*
		 * Set speed:
		 * F = CPU_FREQ / (16 + 2*TWBR * 4^TWPS)
		 */
		#ifndef CONFIG_I2C_FREQ
			#warning Using default value of 300000L for CONFIG_I2C_FREQ
			#define CONFIG_I2C_FREQ  300000L /* ~300 kHz */
		#endif
		#define TWI_PRESC 1       /* 4 ^ TWPS */

		TWBR = (CPU_FREQ / (2 * CONFIG_I2C_FREQ * TWI_PRESC)) - (8 / TWI_PRESC);
		TWSR = 0;
		TWCR = BV(TWEN);
	);
	MOD_INIT(i2c);
}

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
 * \author Bernardo Innocenti <bernie@develer.com>
 */

#include "twi_avr.h"

#include "hw_cpu.h"  /* CLOCK_FREQ */

#include <cfg/cfg_twi.h>
#include <cfg/debug.h>
#include <cfg/macros.h> // BV()

#include <cpu/detect.h>
#include <cpu/irq.h>

#include <compat/twi.h>


/* Wait for TWINT flag set: bus is ready */
#define WAIT_TWI_READY  do {} while (!(TWCR & BV(TWINT)))

#define READ_BIT BV(0)


/**
 * Send START condition on the bus.
 *
 * \return true on success, false otherwise.
 */
static bool twi_start(void)
{
	TWCR = BV(TWINT) | BV(TWSTA) | BV(TWEN);
	WAIT_TWI_READY;

	if (TW_STATUS == TW_START || TW_STATUS == TW_REP_START)
		return true;

	kprintf("!TW_(REP)START: %x\n", TWSR);
	return false;
}


/**
 * Send START condition and select slave for write.
 * \c id is the device id comprehensive of address left shifted by 1.
 * The LSB of \c id is ignored and reset to 0 for write operation.
 *
 * \return true on success, false otherwise.
 */
bool twi_start_w(uint8_t id)
{
	/*
	 * Loop on the select write sequence: when the eeprom is busy
	 * writing previously sent data it will reply to the SLA_W
	 * control byte with a NACK.  In this case, we must
	 * keep trying until the eeprom responds with an ACK.
	 */
	while (twi_start())
	{
		TWDR = id & ~READ_BIT;
		TWCR = BV(TWINT) | BV(TWEN);
		WAIT_TWI_READY;

		if (TW_STATUS == TW_MT_SLA_ACK)
			return true;
		else if (TW_STATUS != TW_MT_SLA_NACK)
		{
			kprintf("!TW_MT_SLA_(N)ACK: %x\n", TWSR);
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
bool twi_start_r(uint8_t id)
{
	if (twi_start())
	{
		TWDR = id | READ_BIT;
		TWCR = BV(TWINT) | BV(TWEN);
		WAIT_TWI_READY;

		if (TW_STATUS == TW_MR_SLA_ACK)
			return true;

		kprintf("!TW_MR_SLA_ACK: %x\n", TWSR);
	}

	return false;
}


/**
 * Send STOP condition.
 */
void twi_stop(void)
{
        TWCR = BV(TWINT) | BV(TWEN) | BV(TWSTO);
}


/**
 * Put a single byte in master transmitter mode
 * to the selected slave device through the TWI bus.
 *
 * \return true on success, false on error.
 */
bool twi_put(const uint8_t data)
{
	TWDR = data;
	TWCR = BV(TWINT) | BV(TWEN);
	WAIT_TWI_READY;
	if (TW_STATUS != TW_MT_DATA_ACK)
	{
		kprintf("!TW_MT_DATA_ACK: %x\n", TWSR);
		return false;
	}
	return true;
}


/**
 * Send a sequence of bytes in master transmitter mode
 * to the selected slave device through the TWI bus.
 *
 * \return true on success, false on error.
 */
bool twi_send(const void *_buf, size_t count)
{
	const uint8_t *buf = (const uint8_t *)_buf;

	while (count--)
	{
		if (!twi_put(*buf++))
			return false;
	}
	return true;
}


/**
 * Receive a sequence of one or more bytes from the
 * selected slave device in master receive mode through
 * the TWI bus.
 *
 * Received data is placed in \c buf.
 *
 * \return true on success, false on error
 */
bool twi_recv(void *_buf, size_t count)
{
	uint8_t *buf = (uint8_t *)_buf;

	/*
	 * When reading the last byte the TWEA bit is not
	 * set, and the eeprom should answer with NACK
	 */
	while (count--)
	{
		TWCR = BV(TWINT) | BV(TWEN) | (count ? BV(TWEA) : 0);
		WAIT_TWI_READY;

		if (count)
		{
			if (TW_STATUS != TW_MR_DATA_ACK)
			{
				kprintf("!TW_MR_DATA_ACK: %x\n", TWSR);
				return false;
			}
		}
		else
		{
			if (TW_STATUS != TW_MR_DATA_NACK)
			{
				kprintf("!TW_MR_DATA_NACK: %x\n", TWSR);
				return false;
			}
		}
		*buf++ = TWDR;
	}

	return true;
}


/**
 * Initialize TWI module.
 */
void twi_init(void)
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
#else
		#error Unsupported architecture
#endif

		/*
		 * Set speed:
		 * F = CLOCK_FREQ / (16 + 2*TWBR * 4^TWPS)
		 */
		#ifndef CONFIG_TWI_FREQ
			#warning Using default value of 300000L for CONFIG_TWI_FREQ
			#define CONFIG_TWI_FREQ  300000L /* ~300 kHz */
		#endif
		#define TWI_PRESC 1       /* 4 ^ TWPS */

		TWBR = (CLOCK_FREQ / (2 * CONFIG_TWI_FREQ * TWI_PRESC)) - (8 / TWI_PRESC);
		TWSR = 0;
		TWCR = BV(TWEN);
	);
}

/*!
 * \file
 * <!--
 * Copyright 2003, 2004, 2005 Develer S.r.l. (http://www.develer.com/)
 * This file is part of DevLib - See devlib/README for information.
 * -->
 *
 * \brief Driver for the AVR ATMega TWI (implementation)
 *
 * \version $Id$
 *
 * \author Stefano Fedrigo <aleph@develer.com>
 * \author Bernardo Innocenti <bernie@develer.com>
 */

/*#*
 *#* $Log$
 *#* Revision 1.2  2005/01/25 08:36:56  bernie
 *#* CONFIG_TWI_FREQ: New config param.
 *#*
 *#* Revision 1.1  2005/01/06 16:09:40  aleph
 *#* Split twi/eeprom functions from eeprom module in separate twi module
 *#*
 *#*/

#include "twi.h"
#include "config.h"
#include <debug.h>
#include <hw.h>
#include <cpu.h>
#include <macros.h> // BV()

#include <avr/twi.h>


/* Wait for TWINT flag set: bus is ready */
#define WAIT_TWI_READY  do {} while (!(TWCR & BV(TWINT)))

/*! \name EEPROM control codes */
/*@{*/
#define SLA_W  0xA0
#define SLA_R  0xA1
/*@}*/


/*!
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


/*!
 * Send START condition and select slave for write.
 *
 * \return true on success, false otherwise.
 */
bool twi_start_w(uint8_t slave_addr)
{
	ASSERT(slave_addr < 8);

	/*
	 * Loop on the select write sequence: when the eeprom is busy
	 * writing previously sent data it will reply to the SLA_W
	 * control byte with a NACK.  In this case, we must
	 * keep trying until the eeprom responds with an ACK.
	 */
	while (twi_start())
	{
		TWDR = SLA_W | (slave_addr << 1);
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


/*!
 * Send START condition and select slave for read.
 *
 * \return true on success, false otherwise.
 */
bool twi_start_r(uint8_t slave_addr)
{
	ASSERT(slave_addr < 8);

	if (twi_start())
	{
		TWDR = SLA_R | (slave_addr << 1);
		TWCR = BV(TWINT) | BV(TWEN);
		WAIT_TWI_READY;

		if (TW_STATUS == TW_MR_SLA_ACK)
			return true;

		kprintf("!TW_MR_SLA_ACK: %x\n", TWSR);
	}

	return false;
}


/*!
 * Send STOP condition.
 */
void twi_stop(void)
{
	TWCR = BV(TWINT) | BV(TWEN) | BV(TWSTO);
}


/*!
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
		TWDR = *buf++;
		TWCR = BV(TWINT) | BV(TWEN);
		WAIT_TWI_READY;
		if (TW_STATUS != TW_MT_DATA_ACK)
		{
			kprintf("!TW_MT_DATA_ACK: %x\n", TWSR);
			return false;
		}
	}

	return true;
}


/*!
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


/*!
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
#if defined(__AVR_ATmega64__)
		PORTD |= BV(PD0) | BV(PD1);
		DDRD |= BV(PD0) | BV(PD1);
#elif defined(__AVR_ATmega8__)
		PORTC |= BV(PC4) | BV(PC5);
		DDRC |= BV(PC4) | BV(PC5);
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

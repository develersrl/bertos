/*!
 * \file
 * <!--
 * Copyright 2003, 2004 Develer S.r.l. (http://www.develer.com/)
 * All Rights Reserved.
 * -->
 *
 * \version $Id$
 *
 * \author Stefano Fedrigo <aleph@develer.com>
 *
 * \brief I2C eeprom driver
 */

/*
 * $Log$
 * Revision 1.1  2004/07/20 17:11:18  bernie
 * Import into DevLib.
 *
 */

#include "eeprom.h"
#include <mware/byteorder.h> /* cpu_to_be16() */
#include <drv/kdebug.h>
#include <hw.h>

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

	DB(kprintf("!TW_(REP)START: %x\n", TWSR);)
	return false;
}


/*!
 * Send START condition and select slave for write.
 *
 * \return true on success, false otherwise.
 */
static bool twi_start_w(uint8_t slave_addr)
{
	//TRACE;

	/* Do a loop on the select write sequence because if the
	 * eeprom is busy writing precedently sent data it will respond
	 * with NACK to the SLA_W control byte. In this case we have
	 * to try until the eeprom reply with an ACK.
	 */
	while (twi_start())
	{
		TWDR = SLA_W | ((slave_addr & 0x5) << 1);
		TWCR = BV(TWINT) | BV(TWEN);
		WAIT_TWI_READY;

		if (TW_STATUS == TW_MT_SLA_ACK)
			return true;
		else if (TW_STATUS != TW_MT_SLA_NACK)
		{
			DB(kprintf("!TW_MT_SLA_(N)ACK: %x\n", TWSR);)
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
static bool twi_start_r(uint8_t slave_addr)
{
	//TRACE;

	if (twi_start())
	{
		TWDR = SLA_R | ((slave_addr & 0x5) << 1);
		TWCR = BV(TWINT) | BV(TWEN);
		WAIT_TWI_READY;

		if (TW_STATUS == TW_MR_SLA_ACK)
			return true;

		DB(kprintf("!TW_MR_SLA_ACK: %x\n", TWSR);)
	}

	return false;
}


/*!
 * Send STOP condition.
 */
static void twi_stop(void)
{
	//TRACE;

	TWCR = BV(TWINT) | BV(TWEN) | BV(TWSTO);
}


/*!
 * Send a sequence of bytes in master transmitter mode
 * to the selected slave device through the TWI bus.
 *
 * \return true on success, false on error.
 */
static bool twi_send(const uint8_t *buf, size_t count)
{
	//TRACE;

	while (count--)
	{
		TWDR = *buf++;
		TWCR = BV(TWINT) | BV(TWEN);
		WAIT_TWI_READY;
		if (TW_STATUS != TW_MT_DATA_ACK)
		{
			DB(kprintf("!TW_MT_DATA_ACK: %x\n", TWSR);)
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
static bool twi_recv(uint8_t *buf, size_t count)
{
	//TRACE;

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
				DB(kprintf("!TW_MR_DATA_ACK: %x\n", TWSR);)
				return false;
			}
		}
		else
		{
			if (TW_STATUS != TW_MR_DATA_NACK)
			{
				DB(kprintf("!TW_MR_DATA_NACK: %x\n", TWSR);)
				return false;
			}
		}
		*buf++ = TWDR;
	}

	return true;
}


/*!
 * Copy \c count bytes from buffer \c buf to
 * eeprom at address \c addr.
 *
 * \note No check is done for data crossing page
 *       boundaries.
 */
bool eeprom_write(e2addr_t addr, const void *buf, size_t count)
{
	// eeprom accepts address as big endian
	addr = cpu_to_be16(addr);

	bool res =
		twi_start_w(0)
		&& twi_send((uint8_t *)&addr, sizeof(addr))
		&& twi_send(buf, count);

	twi_stop();

	return res;
}


/*!
 * Copy \c count bytes at address \c addr
 * from eeprom to RAM to buffer \c buf.
 */
bool eeprom_read(e2addr_t addr, void *buf, size_t count)
{
	// eeprom accepts address as big endian
	addr = cpu_to_be16(addr);

	bool res =
		twi_start_w(0)
		&& twi_send((uint8_t *)&addr, sizeof(addr))
		&& twi_start_r(0)
		&& twi_recv(buf, count);

	twi_stop();

	return res;
}


/*!
 * Write a single character \a c at address \a addr.
 */
bool eeprom_write_char(e2addr_t addr, char c)
{
	return eeprom_write(addr, &c, 1);
}


/*!
 * Read a single character at address \a addr.
 *
 * \return the requested character or -1 in case of failure.
 */
int eeprom_read_char(e2addr_t addr)
{
	char c;

	if (eeprom_read(addr, &c, 1))
		return c;
	else
		return -1;
}


/*!
 * Initialize TWI module.
 */
void eeprom_init(void)
{
	cpuflags_t flags;
	DISABLE_IRQSAVE(flags);

	DDRD |= BV(PORTD0) | BV(PORTD1);
	PORTD |= BV(PORTD0) | BV(PORTD1);

	/*
	 * Set speed:
	 * F = CLOCK_FREQ / (16 + 2*TWBR * 4^TWPS)
	 */
#	define TWI_FREQ  300000  /* 300 kHz */
#	define TWI_PRESC 1       /* 4 ^ TWPS */

	TWBR = (CLOCK_FREQ / (2 * TWI_FREQ * TWI_PRESC)) - (8 / TWI_PRESC);
	TWSR = 0;

	ENABLE_IRQRESTORE(flags);
}


#ifdef _DEBUG

void eeprom_test(void)
{
	static const char magic[] = "Humpty Dumpty";
	char buf[sizeof magic];

	// Write something to EEPROM and read it back
	eeprom_write(0, magic, sizeof magic);
	eeprom_read(0, buf, sizeof buf);
	kprintf("EEPROM read: %s\n", buf);
}

#endif // _DEBUG


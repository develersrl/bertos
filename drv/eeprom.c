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
 * \author Bernardo Innocenti <bernie@develer.com>
 *
 * \brief Driver for the 24xx16 and 24xx256 I2C EEPROMS (implementation)
 *
 * \note This implementation is AVR specific.
 */

/*
 * $Log$
 * Revision 1.4  2004/08/10 06:57:22  bernie
 * eeprom_erase(): New function.
 *
 * Revision 1.3  2004/07/29 22:57:09  bernie
 * Add 24LC16 support.
 *
 * Revision 1.2  2004/07/22 01:24:43  bernie
 * Document AVR dependency.
 *
 * Revision 1.1  2004/07/20 17:11:18  bernie
 * Import into DevLib.
 *
 */
#include "eeprom.h"
#include <mware/byteorder.h> /* cpu_to_be16() */
#include <drv/kdebug.h>
#include <hw.h>
#include <string.h> // memset()

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
	ASSERT(slave_addr < 8);

	if (twi_start())
	{
		TWDR = SLA_R | (slave_addr << 1);
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
 */
bool eeprom_write(e2addr_t addr, const void *buf, size_t count)
{
	bool result = true;
	ASSERT(addr + count <= EEPROM_SIZE);

	while (count && result)
	{
		/*
		 * Split write in multiple sequential mode operations that
		 * don't cross page boundaries.
		 */
		size_t size =
			MIN(count, (size_t)(EEPROM_BLKSIZE - (addr & (EEPROM_BLKSIZE - 1))));

	#if CONFIG_EEPROM_TYPE == EEPROM_24XX16
		/*
		 * The 24LC16 uses the slave address as a 3-bit
		 * block address.
		 */
		uint8_t blk_addr = (uint8_t)((addr >> 8) & 0x07);
		uint8_t blk_offs = (uint8_t)addr;

		result =
			twi_start_w(blk_addr)
			&& twi_send(&blk_offs, sizeof blk_offs)
			&& twi_send(buf, size);

	#elif CONFIG_EEPROM_TYPE == EEPROM_24XX256

		// 24LC256 wants big-endian addresses
		uint16_t addr_be = cpu_to_be16(addr);

		result =
			twi_start_w(0)
			&& twi_send((uint8_t *)&addr_be, sizeof addr_be)
			&& twi_send(buf, size);

	#else
		#error Unknown device type
	#endif

		twi_stop();

		// DEBUG
		//kprintf("addr=%d, count=%d, size=%d, *#?=%d\n",
		//	addr, count, size,
		//	(EEPROM_BLKSIZE - (addr & (EEPROM_BLKSIZE - 1)))
		//);

		/* Update count and addr for next operation */
		count -= size;
		addr += size;
		buf = ((const char *)buf) + size;
	}

	return result;
}


/*!
 * Copy \c count bytes at address \c addr
 * from eeprom to RAM to buffer \c buf.
 */
bool eeprom_read(e2addr_t addr, void *buf, size_t count)
{
	ASSERT(addr + count <= EEPROM_SIZE);

#if CONFIG_EEPROM_TYPE == EEPROM_24XX16
	/*
	 * The 24LC16 uses the slave address as a 3-bit
	 * block address.
	 */
	uint8_t blk_addr = (uint8_t)((addr >> 8) & 0x07);
	uint8_t blk_offs = (uint8_t)addr;

	bool res =
		twi_start_w(blk_addr)
		&& twi_send(&blk_offs, sizeof blk_offs)
		&& twi_start_r(blk_addr)
		&& twi_recv(buf, count);

#elif CONFIG_EEPROM_TYPE == EEPROM_24XX256

	// 24LC256 wants big-endian addresses
	addr = cpu_to_be16(addr);

	bool res =
		twi_start_w(0)
		&& twi_send((uint8_t *)&addr, sizeof(addr))
		&& twi_start_r(0)
		&& twi_recv(buf, count);
#else
	#error Unknown device type
#endif

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
 * Erase specified part of eeprom, writing 0xFF.
 *
 * \param addr  starting address
 * \param len   length of block to erase
 */
void eeprom_erase(e2addr_t addr, size_t count)
{
	uint8_t buf[EEPROM_BLKSIZE];
	memset(buf, 0xFF, sizeof buf);

	// Clear all but struct hw_info at start of eeprom
	while (count)
	{
		size_t size = MIN(count, sizeof buf);
		eeprom_write(addr, buf, size);
		addr += size;
		count -= size;
	}
}


/*!
 * Initialize TWI module.
 */
void eeprom_init(void)
{
	cpuflags_t flags;
	DISABLE_IRQSAVE(flags);

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
	#define TWI_FREQ  300000  /* 300 kHz */
	#define TWI_PRESC 1       /* 4 ^ TWPS */

	TWBR = (CLOCK_FREQ / (2 * TWI_FREQ * TWI_PRESC)) - (8 / TWI_PRESC);
	TWSR = 0;
	TWCR = BV(TWEN);

	ENABLE_IRQRESTORE(flags);
}


#ifdef _DEBUG

#include <string.h>

void eeprom_test(void)
{
	static const char magic[13] = "Humpty Dumpty";
	char buf[sizeof magic + 1];
	size_t i;

	// Write something to EEPROM using unaligned sequential writes
	for (i = 0; i < 42; ++i)
		eeprom_write(i * sizeof magic, magic, sizeof magic);

	// Read back with single-byte reads
	for (i = 0; i < 42 * sizeof magic; ++i)
	{
		eeprom_read(i, buf, 1);
		kprintf("EEPROM byte read: %c (%d)\n", buf[0], buf[0]);
		ASSERT(buf[0] == magic[i % sizeof magic]);
	}

	// Read back again using sequential reads
	for (i = 0; i < 42; ++i)
	{
		memset(buf, 0, sizeof buf);
		eeprom_read(i * sizeof magic, buf, sizeof magic);
		kprintf("EEPROM seq read @ 0x%x: '%s'\n", i * sizeof magic, buf);
		ASSERT(memcmp(buf, magic, sizeof magic) == 0);
	}
}

#endif // _DEBUG

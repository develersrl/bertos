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
 * Copyright 2011 Onno <developer@gorgoz.org>
 *
 * -->
 *
 * \brief Driver for the AVR ATXMega TWI (implementation) based on the ATMega version
 *
 * \author Onno <developer@gorgoz.org>
 * notest:all
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
#include <cpu/types.h>

#include <drv/timer.h>
#include <drv/i2c.h>

#include <cpu/power.h>

#include <avr/io.h>
#include <util/twi.h> //AVRLIBC TWI bit mask definitions
#include <stdbool.h>

#if !CONFIG_I2C_DISABLE_OLD_API
	#error I2C_OLD_API is not implemented
#endif

/*
 * New Api
 */
struct I2cHardware
{
	volatile TWI_t *twi;
};

/* Baud register setting calculation. Formula described in datasheet. */
#define TWI_BAUD(F_SYS, F_TWI) (DIV_ROUND(F_SYS, (2 * F_TWI)) - 5)

// Wait until write interrupt is set
#define WAIT_UNTIL_WRITE_INTERRUPT_SET(TWI) \
		do { \
			while (((TWI)->MASTER.STATUS & TWI_MASTER_WIF_bm) == 0) \
				cpu_relax(); \
		} while (0)

#define WAIT_UNTIL_READ_OR_WRITE_INTERRUPT_SET(TWI) \
		do { \
			while (((TWI)->MASTER.STATUS & (TWI_MASTER_WIF_bm | TWI_MASTER_RIF_bm)) == 0) \
				cpu_relax(); \
		} while (0)

#define MASTER_STATE_IS_IDLE(TWI)	(((TWI)->MASTER.STATUS & TWI_MASTER_BUSSTATE_gm) == TWI_MASTER_BUSSTATE_IDLE_gc)
#define MASTER_STATE_IS_OWNER(TWI)	(((TWI)->MASTER.STATUS & TWI_MASTER_BUSSTATE_gm) == TWI_MASTER_BUSSTATE_OWNER_gc)
#define MASTER_STATE_IS_BUSY(TWI)	(((TWI)->MASTER.STATUS & TWI_MASTER_BUSSTATE_gm) == TWI_MASTER_BUSSTATE_BUSY_gc)
#define MASTER_STATE_IS_UNKOWN(TWI)	(((TWI)->MASTER.STATUS & TWI_MASTER_BUSSTATE_gm) == TWI_MASTER_BUSSTATE_UNKNOWN_gc)

// Wait until ready for the next transaction
#define WAIT_UNTIL_READY_FOR_TRANSACTION(TWI) \
		do { \
			   while (MASTER_STATE_IS_BUSY(TWI))\
				cpu_relax(); \
		} while (0)


#define READ_INTERRUPT_FLAG_IS_SET(TWI)	(((TWI)->MASTER.STATUS & TWI_MASTER_RIF_bm) != 0)
#define ARBLOST_OR_BUSERR_OCCURED(TWI) (((TWI)->MASTER.STATUS & (TWI_MASTER_BUSERR_bm | TWI_MASTER_ARBLOST_bm)) != 0)
#define ARBLOST_OCCURED(TWI) (((TWI)->MASTER.STATUS & TWI_MASTER_ARBLOST_bm) != 0)
#define BUSERR_OCCURED(TWI) (((TWI)->MASTER.STATUS & TWI_MASTER_BUSERR_bm) != 0)
#define ACK_RECEIVED(TWI) (((TWI)->MASTER.STATUS & TWI_MASTER_RXACK_bm) == 0)
#define NACK_RECEIVED(TWI) (((TWI)->MASTER.STATUS & TWI_MASTER_RXACK_bm) != 0)
#define CLEAR_BUSERROR(TWI) ((TWI)->MASTER.STATUS |= TWI_MASTER_BUSERR_bm)
#define SET_IDLESTATE(TWI)	((TWI)->MASTER.STATUS = ((TWI)->MASTER.STATUS & (~TWI_MASTER_BUSSTATE_gm)) | TWI_MASTER_BUSSTATE_IDLE_gc)

/**
 * Send STOP condition.
 */
INLINE void i2c_hw_stop(volatile TWI_t *twi)
{
	twi->MASTER.CTRLC |= TWI_MASTER_CMD_STOP_gc;
}

static void i2c_xmega_start(I2c *i2c, uint16_t slave_addr)
{
	/*
	 * Loop on the select write sequence: when the eeprom is busy
	 * writing previously sent data it will reply to the SLA_W
	 * control byte with a NACK.  In this case, we must
	 * keep trying until the slave responds with an ACK.
	 */
	ticks_t start = timer_clock();
	volatile TWI_t *twi = i2c->hw->twi;
	bool stop_loop = false;

	//check if there is a buserror
	//if so, try to clear it!
	if (BUSERR_OCCURED(twi))
	{
		LOG_WARN("Clearing BusError from TWI device\r\n");
		CLEAR_BUSERROR(twi);
		LOG_WARN("Forcing TWI Device to IDLE state\r\n");
		SET_IDLESTATE(twi);
		LOG_WARN("Current status is: %d\r\n", twi->MASTER.STATUS);
	}

	while (!stop_loop)
	{
		//Wait until we are in IDLE state.
		//Due to a set timeout, we will always retun to IDLE state
		//in due time.
		WAIT_UNTIL_READY_FOR_TRANSACTION(twi);

		//Write the address
		//This will first generate the start condition
		if (I2C_TEST_START(i2c->flags) == I2C_START_W)
		{
			//start a write action
			uint8_t write_address = ((uint8_t)slave_addr << 1) & ~0x01;
			twi->MASTER.ADDR = write_address;

			//Wait until the write interrupt flag is set.
			//this will also be set when an error occurs.
			WAIT_UNTIL_WRITE_INTERRUPT_SET(twi);
		}
		else
		{
			uint8_t read_address = ((uint8_t)slave_addr << 1) | 0x01;
			twi->MASTER.ADDR = read_address;

			//Wait until the read or write interrupt flag is set.
			//In this case, a write interrupt flag is set on an error
			//a read interrupt on an ack
			WAIT_UNTIL_READ_OR_WRITE_INTERRUPT_SET(twi);
		}


		//check if Arbitration Lost of Buserror has occured
		if (ARBLOST_OR_BUSERR_OCCURED(twi))
		{
			if (ARBLOST_OCCURED(twi))
			{
				i2c->errors |= I2C_ARB_LOST;
			}

			if (BUSERR_OCCURED(twi))
			{
				i2c->errors |= I2C_ERR;
			}
			LOG_ERR("Start error [%x]\n", twi->MASTER.STATUS);

			// reset i2c
			i2c->xfer_size = 0;
			// try to send a stop signal
			i2c_hw_stop(twi);
			stop_loop = true;
		}
		else if (ACK_RECEIVED(twi))
		{
			//ack received
			stop_loop = true;
		}
		else
		{
			//nack received
			//ignore if writing, the slave device might just be busy
			//with a precious command.
			if (I2C_TEST_START(i2c->flags) == I2C_START_W)
			{
				//Just check if the start timeout has occured
				if (timer_clock() - start > ms_to_ticks(CONFIG_I2C_START_TIMEOUT))
				{
					LOG_ERR("Start timeout\n");
					i2c->errors |= I2C_START_TIMEOUT;
					i2c_hw_stop(twi);
					stop_loop = true;
				}
			}
			else
			{
				//while reading.... stop
				LOG_ERR("Start addr NACK[%x]\n", twi->MASTER.STATUS);
				i2c->errors |= I2C_NO_ACK;
				i2c_hw_stop(twi);
				stop_loop = true;
			}
		}
	}
}

static void i2c_xmega_putc(I2c *i2c, const uint8_t data)
{
	volatile TWI_t *twi = i2c->hw->twi;

	twi->MASTER.DATA = data;

	WAIT_UNTIL_WRITE_INTERRUPT_SET(twi);

	if (ARBLOST_OR_BUSERR_OCCURED(twi))
	{
		if (ARBLOST_OCCURED(twi))
		{
			i2c->errors |= I2C_ARB_LOST;
		}

		if (BUSERR_OCCURED(twi))
		{
			i2c->errors |= I2C_ERR;
		}

		LOG_ERR("Data write error [%x]\n", twi->MASTER.STATUS);
		i2c_hw_stop(twi);
	}
	else if (NACK_RECEIVED(twi))
	{
		LOG_ERR("Data nack[%x]\n", twi->MASTER.STATUS);
		i2c->errors |= I2C_DATA_NACK;
		i2c_hw_stop(twi);
	}

	if ((i2c->xfer_size == 1) && (I2C_TEST_STOP(i2c->flags) == I2C_STOP))
	{
		i2c_hw_stop(twi);
	}
}

static uint8_t i2c_xmega_getc(I2c *i2c)
{
	volatile TWI_t *twi = i2c->hw->twi;

	//check if the RIF flag is set
	if (READ_INTERRUPT_FLAG_IS_SET(twi))
	{
		//read the available data
		uint8_t data = twi->MASTER.DATA;
		//if this is the last byte to receive, send nack otherwise ack.
		if (i2c->xfer_size == 1)
		{
			//nack needs to be send
			//check if a stop needs to be generated afther the ack, of a repeat start.
			if (I2C_TEST_STOP(i2c->flags) == I2C_STOP)
			{
				//send nack and stop
				twi->MASTER.CTRLC = TWI_MASTER_ACKACT_bm | TWI_MASTER_CMD_STOP_gc;
			}
			else
			{
				//only set the nack. The repeated start will be send on the next
				//call to i2c_xmega_start
				twi->MASTER.CTRLC = TWI_MASTER_ACKACT_bm;
			}
		}
		else
		{
			//send ack and request next byte
			twi->MASTER.CTRLC = TWI_MASTER_CMD_RECVTRANS_gc;
			//Wait until the read or write interrupt flag is set.
			//In this case, a write interrupt flag is set on an error
			//a read interrupt on an ack
			WAIT_UNTIL_READ_OR_WRITE_INTERRUPT_SET(twi);
			//check if Arbitration Lost of Buserror has occured
			if ( ARBLOST_OR_BUSERR_OCCURED(twi))
			{
				if (ARBLOST_OCCURED(twi))
				{
					i2c->errors |= I2C_ARB_LOST;
				}

				if (BUSERR_OCCURED(twi))
				{
					i2c->errors |= I2C_ERR;
				}

				LOG_ERR("Data error [%x]\n", twi->MASTER.STATUS);
				i2c_hw_stop(twi);
				return 0xFF;
			}
		}
		return data;
	}
	else
	{
		LOG_ERR("Data RIF not set[%x]\n", twi->MASTER.STATUS);
		i2c->errors |= I2C_ERR;
		i2c_hw_stop(twi);
		return 0xFF;
	}
}


static const I2cVT i2c_xmega_vt =
{
	.start = i2c_xmega_start,
	.getc = i2c_xmega_getc,
	.putc = i2c_xmega_putc,
	.write = i2c_genericWrite,
	.read = i2c_genericRead,
};

struct I2cHardware i2c_xmega_hw[] =
{
	{ /* I2C0 */
		.twi = &TWIC,
	},
	{ /* ICC1 */
		.twi = &TWIE,
	},
	#if CPU_AVR_XMEGA_A1
	{ /* I2C2 */
		.twi = &TWID,
	},
	{ /* ICC3 */
		.twi = &TWIF,
	},
	#endif
};

/**
 * Initialize I2C module.
 */
void i2c_hw_init(I2c *i2c, int dev, uint32_t clock)
{
	i2c->hw = &i2c_xmega_hw[dev];
	i2c->vt = &i2c_xmega_vt;

	volatile TWI_t *twi = i2c->hw->twi;

	//enable TWI Master Mode
	twi->MASTER.CTRLA = TWI_MASTER_ENABLE_bm;
	//set an Interactive Bus Timeout
	twi->MASTER.CTRLB = TWI_MASTER_TIMEOUT_50US_gc;
	//set baud rate
	ASSERT(clock);
	twi->MASTER.BAUD = TWI_BAUD(CPU_FREQ, clock);
	//set status to idle
	twi->MASTER.STATUS = TWI_MASTER_BUSSTATE_IDLE_gc;
}

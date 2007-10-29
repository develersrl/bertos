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
 * Copyright 2007 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 *  \brief Function library for AT45DBXX Data Flash memory.
 *
 *
 * \version $Id: dataflash.c 15379 2007-03-28 15:46:09Z asterix $
 * \author Daniele Basile <asterix@develer.com>
 */


#include <appconfig.h>

#include <avr/io.h>
#include <cfg/macros.h>
#include <cfg/debug.h>
#include <cfg/module.h>
#include <drv/timer.h>
#include <drv/spi.h>
#include <drv/dataflash.h>

#include "hw_spi.h"


/**
 * Global variable for store current and previous data
 * flash memory page address during operation of writing.
 */
dataflash_t previous_page = 0;
bool page_modified = false;


/**
 * Send a generic command to data flash memory.
 * This function send only 4 byte, for opcode, page address and
 * byte address.
 */
static void send_cmd(dataflash_t page_addr, dataflash_t byte_addr, DFlashOpcode opcode)
{

	/*
	 * Make sure to toggle CS signal in order,
	 * and reset dataflash command decoder.
	 *
	 * \note This is equivalent to CS_DISABLE() immediately followed by CS_ENABLE()
	 */
	CS_TOGGLE();


	/*
	 * To send one command to data flash memory, we send 4 byte.
	 * First byte is opcode command, second and third byte are
	 * page address, in last byte we write a byte page address.
	 * (see datasheet for more detail).
	 *
	 * \note Generaly a defaul memory page size is more than 256 byte.
	 *  In this case we need for addressing a byte in one page more than
	 *  8 bit, so we put in fourth byte low part of address byte, and
	 *  hight part of address byte in third byte togheter low par of page
	 *  address.
	 *
	 * \{
	 */

	/*
	 * Send opcode.
	 */
	spi_sendRecv(opcode);

	/*
	 *  Send page address.
	 * \{
	 */
	spi_sendRecv((uint8_t)(page_addr >> (16 - DATAFLASH_PAGE_ADDRESS_BIT)));
	spi_sendRecv((uint8_t)((page_addr << (DATAFLASH_PAGE_ADDRESS_BIT - 8)) + (byte_addr >> 8)));
	/*\}*/

	/*
	 * Send byte page address.
	 */
	spi_sendRecv((uint8_t)byte_addr);

	/* \} */

}

/**
 * Reset dataflash memory function.
 *
 * This function reset data flash memory
 * with one pulse reset long about 10usec.
 *
 */
static void dataflash_reset(void)
{
	CS_ENABLE();
	RESET_ENABLE();
	timer_delayHp(us_to_hptime(RESET_PULSE_WIDTH));
	CS_DISABLE();
	RESET_DISABLE();
	timer_delayHp(us_to_hptime(RESET_PULSE_WIDTH));
}

/**
 * dataflash init function.
 * This function initialize a micro pin and
 * SPI driver, and test if data flash memory
 * density is the same wich define in dataflash.h.
 */
MOD_DEFINE(dataflash);
static bool dataflash_pin_init(void)
{
	uint8_t stat;

	MOD_CHECK(spi);

	RESET_DISABLE();
	WRITE_ENABLE(); //pilot wp pin.

	RESET_OUT();
	WP_OUT();

	dataflash_reset();

	stat = dataflash_stat();

	MOD_INIT(dataflash);

	/*
	 * 2,3,4,5 bits of 1 byte status register
	 * indicate a device density of dataflash memory
	 * (see datasheet for more detail.)
	 */
	GET_ID_DESITY_DEVICE(stat);

	if(stat == DATAFLASH_ID_DEVICE_DENSITY)
		return true;
	else
		return false;

}


/**
 * Read status register of dataflah memory.
 *
 */
static uint8_t dataflash_stat(void)
{
	uint8_t stat;

	/*
	 * Make sure to toggle CS signal in order,
	 * and reset dataflash command decoder.
	 * \{
	 */
	CS_TOGGLE();

	stat = spi_sendRecv(DFO_READ_STATUS);
	stat = spi_sendRecv(0x00);

	return stat;
}


/**
 * Send one command to data flash memory, and
 * return status register value.
 *
 */
static uint8_t dataflash_cmd(dataflash_t page_addr, dataflash_t byte_addr, DFlashOpcode opcode)
{

	send_cmd(page_addr, byte_addr, opcode);

	CS_DISABLE();
	CS_ENABLE();

	/*
	 * We chech data flash memory state, and wait until busy-flag
	 * is hight.
	 */
	while(!(dataflash_stat() & BUSY_BIT));

	return (dataflash_stat());

}

/**
 * Read one byte from main data flash memory or buffer data
 * flash memory.
 */
static uint8_t dataflash_read_byte(dataflash_t page_addr, dataflash_t byte_addr, DFlashOpcode opcode)
{
	uint8_t data;

	send_cmd(page_addr, byte_addr, opcode);

#if CONFIG_DATA_FLASH == DATAFLASH_AT45DB041B
	if(opcode == DFO_READ_FLASH_MEM_BYTE)
	{
		/*
		 * Send 24 don't care bit.
		 * \{
		 */
		spi_sendRecv(0x00);
		spi_sendRecv(0x00);
		spi_sendRecv(0x00);
		/* \} */

	}
#endif

	spi_sendRecv(0x00);         //Send 8 don't care bit.
	data = spi_sendRecv(0x00);  //Read byte.
	CS_DISABLE();

	return data;
}

/**
 * Read \a len bytes from main data flash memory or buffer data
 * flash memory, and put it in \a *block.
 */
static void dataflash_read_block(dataflash_t page_addr, dataflash_t byte_addr, DFlashOpcode opcode, uint8_t *block, dataflashSize_t len)
{

	send_cmd(page_addr, byte_addr, opcode);

	if(opcode == DFO_READ_FLASH_MEM_BYTE)
	{
		/*
		 * Send 24 don't care bit.
		 * \{
		 */
		spi_sendRecv(0x00);
		spi_sendRecv(0x00);
		spi_sendRecv(0x00);
		/* \} */
	}

	spi_sendRecv(0x00);   //Send 8 don't care bit.
	spi_read(block, len); //Read len bytes ad put in block buffer.


	CS_DISABLE();

}


/**
 * Write \a len bytes in buffer buffer data flash memory.
 *
 * \note Isn't possible to write bytes directly in main memory data
 * flash. To perform write in main memory you must before write in buffer
 * data flash memory, an then send command to write page in main memory.
 */
static void dataflash_write_block(dataflash_t byte_addr, DFlashOpcode opcode, uint8_t *block, dataflashSize_t len)
{

	send_cmd(0x00, byte_addr, opcode);

	spi_write(block, len); //Write len bytes.

	CS_DISABLE();

}


/**
 * Load selct page from dataflash memory to buffer.
 */
static void dataflash_loadPage(dataflash_t page_addr)
{
	dataflash_cmd(page_addr, 0x00, DFO_MOV_MEM_TO_BUFF1);

	CS_DISABLE();
}

/**
 * Flush select page (stored in buffer) in data flash main memory page.
 */
void dataflash_flush(void)
{
	if (page_modified)
	{
		dataflash_cmd(previous_page, 0x00, DFO_WRITE_BUFF1_TO_MEM_E);

		CS_DISABLE();
		page_modified = false;

		kprintf("\n::=> Flush page:... <%ld>\n", previous_page);
	}
}

/* Kfile interface section */

/**
 * Open data flash file \a fd
 * \a name and \a mode are unused, cause flash memory is
 * threated like one file.
 */
static bool dataflash_open(struct _KFile *fd, UNUSED_ARG(const char *, name), UNUSED_ARG(int, mode))
{
	MOD_CHECK(dataflash);

	previous_page = 0;

	/* Load select page memory from data flash memory*/
	dataflash_loadPage(previous_page);

	kprintf("dataflash file opened\n");
	return true;
}

/**
 * Close file \a fd
 */
static bool dataflash_close(UNUSED_ARG(struct _KFile *,fd))
{
	dataflash_flush();
	kprintf("dataflash file closed\n");
	return true;
}

/**
 * Move \a fd file seek position of \a offset bytes
 * from current position.
 */
static int32_t dataflash_seek(struct _KFile *fd, int32_t offset, KSeekMode whence)
{
}

/**
 * Read from file \a fd \a size bytes and put it in buffer \a buf
 * \return the number of bytes read.
 */
static size_t dataflash_read(struct _KFile *fd, void *buf, size_t size)
{

//TODO:

	dataflashAddr_t byte_addr;
	dataflashAddr_t page_addr;

	uint8_t data;

	kprintf(" Read at address:... %ld ",addr);

	/*
	 * We select from absolute address page address
	 * and byte address in page.
	 * \{
	 */
	page_addr = addr / (dataflashAddr_t)DATAFLASH_PAGE_SIZE;
	byte_addr = addr % (dataflashAddr_t)DATAFLASH_PAGE_SIZE;
	/* \} */

	kprintf(" [page-<%ld>, byte-<%ld>]", page_addr, byte_addr);

	/*
	 * Flush current page in main memory if
	 * we had been written a byte in memory
	 */
	dataflash_flush();

	/*
	 * Read byte in main page data flash memory.
	 */
	data = dataflash_read_byte(page_addr, byte_addr, DFO_READ_FLASH_MEM_BYTE);

	kprintf(" ::=> Read data: %02x\n",data);

	return data;
}

/**
 * Write program memory.
 * Write \a size bytes from buffer \a _buf to file \a fd
 * \note Write operations are buffered.
 */
static size_t dataflash_write(struct _KFile *fd, const void *_buf, size_t size)
{
}

/**
 * Init data flash memory interface.
 */
void dataflash_init(struct _KFile *fd)
{
	// Set up data flash programming functions.
	fd->open = dataflash_open;
	fd->close = dataflash_close;
	fd->read = dataflash_read;
	fd->write = dataflash_write;
	fd->seek = dataflash_seek;

	// Init data flash memory and micro pin.
	ASSERT(dataflash_pin_init());
}
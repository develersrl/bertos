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
 *  \brief Function library for serial Flash memory.
 *
 * Module provide a kfile interface, that ensure an abstraction
 * from comunication channel and give a standard interface.
 * Typicaly this kind of memory use an SPI bus, but you should
 * use another comunication channel you have defined.
 *
 * \version $Id$
 * \author Daniele Basile <asterix@develer.com>
 */

/*
 * We use a spi bus, thus include hardware specific definition.
 * If you use another channel you must redefine this macros.
 */
#include "hw_spi.h"

#include <appconfig.h>

#include <cfg/macros.h>
#include <cfg/debug.h>

#include <drv/timer.h>
#include <drv/flash25.h>

#if CONFIG_KERNEL
#include <kern/proc.h>
#endif


/**
 * Global definition of channel handler (usually SPI).
 */
static KFile *channel;

/**
 * Wait until flash memory is ready.
 */
static void flash25_waitReady(void)
{
	uint8_t stat;

	while (1)
	{
		CS_ENABLE();

		kfile_putc(FLASH25_RDSR, channel);
		stat = kfile_getc(channel);

		CS_DISABLE();

		if (!(stat & RDY_BIT))
			break;
		#if CONFIG_KERNEL
		else
			proc_switch();
		#endif
	}
}

/**
 * Send a single command to serial flash memory.
 */
static void flash25_sendCmd(Flash25Opcode cmd)
{
	CS_ENABLE();

	kfile_putc(cmd, channel);

	CS_DISABLE();
}

/**
 * flash25 init function.
 * This function init a comunication channel and
 * try to read manufacturer id of serial memory,
 * then check if is equal to selected type.
 */
static bool flash25_pin_init(void)
{
	uint8_t device_id;
	uint8_t manufacturer;

	SPI_HW_INIT();

	CS_ENABLE();
	/*
	 * Send read id productor opcode on
	 * comunication channel
	 * TODO:controllare se ha senso
	 */
	kfile_putc(FLASH25_RDID, channel);

	manufacturer = kfile_getc(channel);
	device_id = kfile_getc(channel);

	CS_DISABLE();

	if((FLASH25_MANUFACTURER_ID == manufacturer) &&
		(FLASH25_DEVICE_ID == device_id))
		return true;
	else
		return false;
}

/**
 * Reopen a serial memory interface.
 *
 * For serial memory this function reinit only
 * the size and seek_pos in kfile stucture.
 * Return a kfile pointer, after assert check.
 */
static KFile * flash25_reopen(struct KFile *fd)
{
	KFILE_ASSERT_GENERIC(fd);

	fd->seek_pos = 0;
	fd->size = FLASH25_MEM_SIZE;

	kprintf("flash25 file opened\n");
	return fd;
}

/**
 * Close a serial memory interface.
 *
 * For serial memory this funtion do nothing,
 * and return always 0.
 */
static int flash25_close(UNUSED_ARG(struct KFile *,fd))
{
	kprintf("flash25 file closed\n");
	return 0;
}

/**
 * Read \a _buf lenght \a size byte from serial flash memmory.
 *
 * For read in serial flash memory we
 * enble cs pin and send one byte of read opcode,
 * and then 3 byte of address of memory cell we
 * want to read. After the last byte of address we
 * can read data from so pin.
 *
 * \return the number of bytes read.
 */
static size_t flash25_read(struct KFile *fd, void *buf, size_t size)
{
	uint8_t *data = (uint8_t *)buf;

	KFILE_ASSERT_GENERIC(fd);

	ASSERT(fd->seek_pos + size <= fd->size);
	size = MIN((uint32_t)size, fd->size - fd->seek_pos);

	//kprintf("Reading at addr[%lu], size[%d]\n", fd->seek_pos, size);
	CS_ENABLE();

	kfile_putc(FLASH25_READ, channel);


	/*
	 * Address that we want to read.
	 */
	kfile_putc((fd->seek_pos >> 16) & 0xFF, channel);
	kfile_putc((fd->seek_pos >> 8) & 0xFF, channel);
	kfile_putc(fd->seek_pos & 0xFF, channel);

	kfile_read(channel, data, size);

	CS_DISABLE();

	fd->seek_pos += size;

	return size;
}

/**
 * Write \a _buf in serial flash memory
 *
 * Before to write data into flash we must enable
 * memory writing. To do this we send a WRE command opcode.
 * After this command the flash is ready to be write, and so
 * we send a PROGRAM opcode followed to 3 byte of
 * address memory, at the end of last address byte
 * we can send the data.
 * When we finish to send all data, we disable cs
 * and flash write received data bytes on its memory.
 *
 * \note: WARNING: you could write only on erased memory section!
 * Each write time you could write max a memory page size,
 * because if you write more than memory page size the
 * address roll over to first byte of page.
 *
 * \return the number of bytes write.
 */
static size_t flash25_write(struct KFile *fd, const void *_buf, size_t size)
{
	flash25Offset_t offset;
	flash25Size_t total_write = 0;
	flash25Size_t wr_len;
	const uint8_t *data = (const uint8_t *) _buf;

	KFILE_ASSERT_GENERIC(fd);
	ASSERT(fd->seek_pos + size <= fd->size);

	size = MIN((flash25Size_t)size, fd->size - fd->seek_pos);

	while (size)
	{
		offset = fd->seek_pos % (flash25Size_t)FLASH25_PAGE_SIZE;
		wr_len = MIN((flash25Size_t)size, FLASH25_PAGE_SIZE - (flash25Size_t)offset);

		kprintf("[seek_pos-<%lu>, offset-<%d>]\n", fd->seek_pos, offset);

		/*
		 * We check serial flash memory state, and wait until ready-flag
		 * is high.
		 */
		flash25_waitReady();

		/*
		 * Start write cycle.
		 * We could write only data not more long than one
		 * page size.
		 *
		 * To write on serial flash memory we must first
		 * enable write with a WREN opcode command, before
		 * the PROGRAM opcode.
		 *
		 * \note: the same byte cannot be reprogrammed without
		 * erasing the whole sector first.
		 */
		flash25_sendCmd(FLASH25_WREN);

		CS_ENABLE();
		kfile_putc(FLASH25_PROGRAM, channel);

		/*
		 * Address that we want to write.
		 */
		kfile_putc((fd->seek_pos >> 16) & 0xFF, channel);
		kfile_putc((fd->seek_pos >> 8) & 0xFF, channel);
		kfile_putc(fd->seek_pos & 0xFF, channel);

		kfile_write(channel, data, wr_len);

		CS_DISABLE();

		data += wr_len;
		fd->seek_pos += wr_len;
		size -= wr_len;
		total_write += wr_len;
	}

	kprintf("written %u bytes\n", total_write);
	return total_write;
}

/**
 * Sector erase function.
 *
 * Erase a select \p sector of serial flash memory.
 *
 * \note A sector size is FLASH25_SECTOR_SIZE.
 * This operation could take a while.
 */
void flash25_sectorErase(Flash25Sector sector)
{

	/*
	 * Erase a sector could take a while,
	 * for debug we measure that time
	 * see datasheet to compare this time.
	 */
	DB(ticks_t start_time = timer_clock());

	CS_ENABLE();

	/*
	 * To erase a sector of serial flash memory we must first
	 * enable write with a WREN opcode command, before
	 * the SECTOR_ERASE opcode. Sector is automatically
	 * determinate if any address within the sector
	 * is selected.
	 */
	kfile_putc(FLASH25_WREN, channel);
	kfile_putc(FLASH25_SECTORE_ERASE, channel);

	/*
	 * Address inside the sector that we want to
	 * erase.
	 */
	kfile_putc(sector, channel);

	CS_DISABLE();

	/*
	 * We check serial flash memory state, and wait until ready-flag
	 * is hight.
	 */
	flash25_waitReady();

	DB(kprintf("Erased sector [%d] in %d ms\n", sector, ticks_to_ms(timer_clock() - start_time)));

}

/**
 * Chip erase function.
 *
 * Erase all sector of serial flash memory.
 *
 * \note This operation could take a while.
 */
void flash25_chipErase(void)
{
	/*
	 * Erase all chip could take a while,
	 * for debug we measure that time
	 * see datasheet to compare this time.
	 */
	DB(ticks_t start_time = timer_clock());

	/*
	 * To erase serial flash memory we must first
	 * enable write with a WREN opcode command, before
	 * the CHIP_ERASE opcode.
	 */
	flash25_sendCmd(FLASH25_WREN);
	flash25_sendCmd(FLASH25_CHIP_ERASE);

	/*
	 * We check serial flash memory state, and wait until ready-flag
	 * is high.
	 */
	flash25_waitReady();

	DB(kprintf("Erased all memory in %d ms\n", ticks_to_ms(timer_clock() - start_time)));

}

/**
 * Init data flash memory interface.
 */
void flash25_init(struct KFile *fd, struct KFile *_channel)
{
	 //Set kfile struct type as a generic kfile structure.
	DB(fd->_type = KFT_GENERIC);

	// Set up data flash programming functions.
	fd->reopen = flash25_reopen;
	fd->close = flash25_close;
	fd->read = flash25_read;
	fd->write = flash25_write;
	fd->seek = kfile_genericSeek;

	/*
	 * Init a local channel structure and flash kfile interface.
	 */
	channel = _channel;
	flash25_reopen(fd);

	/*
	 * Init data flash memory and micro pin.
	 */
	if (!flash25_pin_init())
		ASSERT(0);
}

#if CONFIG_TEST

/**
 * Test function for flash25.
 *
 * \note: This implentation use a SPI channel.
 */
bool flash25_test(KFile *channel)
{
	KFile fd;
	uint8_t test_buf[256];

	/*
	 * Init a spi kfile interface and
	 * flash driver.
	 */
	flash25_init(&fd, channel);

	kprintf("Init serial flash\n");

	flash25_chipErase();

	flash25_sectorErase(FLASH25_SECT1);
	flash25_sectorErase(FLASH25_SECT2);
	flash25_sectorErase(FLASH25_SECT3);
	flash25_sectorErase(FLASH25_SECT4);

	/*
	 * Launche a kfile test interface.
	 */
	kprintf("Kfile test start..\n");
	if (!kfile_test(&fd, test_buf, NULL, sizeof(test_buf)))
		return false;

	return true;
}
#endif /* CONFIG_TEST */

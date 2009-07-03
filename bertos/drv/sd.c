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
 * -->
 *
 * \brief Function library for secure digital memory.
 *
 * \version $Id$
 * \author Francesco Sacchi <batt@develer.com>
 */


#include "sd.h"
#include "hw/hw_sd.h"
#include <kern/kfile.h>
#include <drv/timer.h>

#include <fs/fat.h>

#include <cfg/log.h>
#include <cpu/power.h>

#include <string.h> /* memset */

/**
 * Card Specific Data
 * read directly from the card.
 */
typedef struct CardCSD
{
	uint16_t block_len;  ///< Length of a block
	uint32_t block_num;  ///< Number of block on the card
	uint16_t capacity;   ///< Card capacity in MB
} CardCSD;

#define SD_IN_IDLE    0x01
#define SD_STARTTOKEN 0xFE

#define TIMEOUT_NAC   256

#define SD_DEFAULT_BLOCKLEN 512

/**
 * SPI communication channel.
 */
static KFile *fd;

/**
 * Current SD status.
 */
static bool sd_status;

#define SD_BUSY_TIMEOUT ms_to_ticks(200)

static bool sd_select(bool state)
{
	if (state)
	{
		ticks_t start = timer_clock();
		do
		{
			SD_CS_ON();
			if (kfile_getc(fd) == 0xff)
				return true;
			SD_CS_OFF();
			kfile_putc(0xff, fd);
			kfile_flush(fd);
			cpu_relax();
		}
		while (timer_clock() - start < SD_BUSY_TIMEOUT);

		LOG_ERR("sd_select timeout\n");
		return false;
	}
	else
	{
		kfile_putc(0xff, fd);
		kfile_flush(fd);
		SD_CS_OFF();
		return true;
	}
}

static int16_t sd_waitR1(void)
{
	uint8_t datain;

	for (int i = 0; i < TIMEOUT_NAC; i++)
	{
	  datain = kfile_getc(fd);
	  if (datain != 0xff)
		return (int16_t)datain;
	}
	LOG_ERR("Timeout waiting R1\n");
	return EOF;
}

static int16_t sd_sendCommand(uint8_t cmd, uint32_t param, uint8_t crc)
{
	/* The 7th bit of command must be a 1 */
	kfile_putc(cmd | 0x40, fd);

	/* send parameter */
	kfile_putc((param >> 24) & 0xFF, fd);
	kfile_putc((param >> 16) & 0xFF, fd);
	kfile_putc((param >> 8) & 0xFF, fd);
	kfile_putc((param) & 0xFF, fd);

	kfile_putc(crc, fd);

	return sd_waitR1();
}


static bool sd_getBlock(void *buf, size_t len)
{
	uint8_t token;
	uint16_t crc;

	for (int i = 0; i < TIMEOUT_NAC; i++)
	{
		token = kfile_getc(fd);
		if (token != 0xff)
		{
			if (token == SD_STARTTOKEN)
			{
				if (kfile_read(fd, buf, len) == len)
				{
					if (kfile_read(fd, &crc, sizeof(crc)) == sizeof(crc))
						/* check CRC here if needed */
						return true;
					else
						LOG_ERR("get_block error getting crc\n");
				}
				else
					LOG_ERR("get_block len error: %d\n", (int)len);
			}
			else
				LOG_ERR("get_block token error: %02X\n", token);

			return false;
		}
	}

	LOG_ERR("get_block timeout waiting token\n");
	return false;
}

#define SD_SELECT() \
do \
{ \
	if (!sd_select(true)) \
	{ \
		LOG_ERR("%s failed, card busy\n", __func__); \
		return EOF; \
	} \
} \
while (0)

#define SD_SETBLOCKLEN 0x50

static int16_t sd_setBlockLen(uint32_t newlen)
{
	SD_SELECT();

	int16_t r1 = sd_sendCommand(SD_SETBLOCKLEN, newlen, 0);

	sd_select(false);
	return r1;
}

#define SD_SEND_CSD 0x49

static int16_t sd_getCSD(CardCSD *csd)
{
	SD_SELECT();

	int16_t r1 = sd_sendCommand(SD_SEND_CSD, 0, 0);

	if (r1)
	{
		LOG_ERR("send_csd failed: %04X\n", r1);
		sd_select(false);
		return r1;
	}

	uint8_t buf[16];
	bool res = sd_getBlock(buf, sizeof(buf));
	sd_select(false);

	if (res)
	{
		uint16_t mult = (1L << ((((buf[9] & 0x03) << 1) | ((buf[10] & 0x80) >> 7)) + 2));
		uint16_t c_size = (((uint16_t)(buf[6] & 0x03)) << 10) | (((uint16_t)buf[7]) << 2) |
				  (((uint16_t)(buf[8] & 0xC0)) >> 6);

		csd->block_len = (1L << (buf[5] & 0x0F));
		csd->block_num = (c_size + 1) * mult;
		csd->capacity = (csd->block_len * csd->block_num) >> 20; // in MB

		LOG_INFO("block_len %d bytes, block_num %ld, total capacity %dMB\n", csd->block_len, csd->block_num, csd->capacity);
		return 0;
	}
	else
		return EOF;
}


#define SD_READ_SINGLEBLOCK 0x51

static int16_t sd_readBlock(void *buf, uint32_t addr)
{
	SD_SELECT();

	int16_t r1 = sd_sendCommand(SD_READ_SINGLEBLOCK, addr, 0);

	if (r1)
	{
		LOG_ERR("read single block failed: %04X\n", r1);
		sd_select(false);
		return r1;
	}

	bool res = sd_getBlock(buf, SD_DEFAULT_BLOCKLEN);
	sd_select(false);
	if (!res)
	{
		LOG_ERR("read single block failed reading data\n");
		return EOF;
	}
	else
		return 0;
}

#define SD_WRITE_SINGLEBLOCK 0x58
#define SD_DATA_ACCEPTED     0x05

static int16_t sd_writeBlock(const void *buf, uint32_t addr)
{
	SD_SELECT();

	int16_t r1 = sd_sendCommand(SD_WRITE_SINGLEBLOCK, addr, 0);

	if (r1)
	{
		LOG_ERR("write single block failed: %04X\n", r1);
		sd_select(false);
		return r1;
	}

	kfile_putc(SD_STARTTOKEN, fd);
	kfile_write(fd, buf, SD_DEFAULT_BLOCKLEN);
	/* send fake crc */
	kfile_putc(0, fd);
	kfile_putc(0, fd);
	uint8_t dataresp = (kfile_getc(fd) & 0x1F);
	sd_select(false);

	// FIXME: sometimes dataresp is 0, find out why.
	if (dataresp != SD_DATA_ACCEPTED)
	{
		LOG_ERR("write single block failed: %02X\n", dataresp);
		return EOF;
	}
	else
		return 0;
}


bool sd_test(void)
{
	CardCSD csd;
	sd_getCSD(&csd);

	uint8_t buf[SD_DEFAULT_BLOCKLEN];

	if (sd_readBlock(buf, 0) != 0)
		return false;

	kputchar('\n');
	for (int i = 0; i < SD_DEFAULT_BLOCKLEN; i++)
	{
		kprintf("%02X ", buf[i]);
		buf[i] = i;
		if (!((i+1) % 16))
			kputchar('\n');
	}

	if (sd_writeBlock(buf, 0) != 0)
		return false;

	memset(buf, 0, sizeof(buf));
	if (sd_readBlock(buf, 0) != 0)
		return false;

	kputchar('\n');
	for (int i = 0; i < SD_DEFAULT_BLOCKLEN; i++)
	{
		kprintf("%02X ", buf[i]);
		buf[i] = i;
		if (!((i+1) % 16))
			kputchar('\n');
	}

	return true;
}

#define SD_GO_IDLE_STATE     0x40
#define SD_GO_IDLE_STATE_CRC 0x95
#define SD_SEND_OP_COND      0x41
#define SD_SEND_OP_COND_CRC  0xF9

#define SD_START_DELAY  ms_to_ticks(10)
#define SD_INIT_TIMEOUT ms_to_ticks(1000)
#define SD_IDLE_RETRIES 4

bool sd_init(KFile *_fd)
{
	uint16_t r1;

	ASSERT(_fd);
	fd = _fd;

	SD_CS_INIT();
	SD_CS_OFF();

	/* Wait a few moments for supply voltage to stabilize */
	timer_delay(SD_START_DELAY);

	/* Give 80 clk pulses to wake up the card */
	for (int i = 0; i < 10; i++)
		kfile_putc(0xff, fd);
	kfile_flush(fd);

	for (int i = 0; i < SD_IDLE_RETRIES; i++)
	{
		SD_SELECT();
		r1 = sd_sendCommand(SD_GO_IDLE_STATE, 0, SD_GO_IDLE_STATE_CRC);
		sd_select(false);

		if (r1 == SD_IN_IDLE)
			break;
	}

	if (r1 != SD_IN_IDLE)
	{
		LOG_ERR("go_idle_state failed: %04X\n", r1);
		return false;
	}

	ticks_t start = timer_clock();

	/* Wait for card to start */
	do
	{
		SD_SELECT();
		r1 = sd_sendCommand(SD_SEND_OP_COND, 0, SD_SEND_OP_COND_CRC);
		sd_select(false);
		cpu_relax();
	}
	while (r1 != 0 && timer_clock() - start < SD_INIT_TIMEOUT);

	if (r1)
	{
		LOG_ERR("send_op_cond failed: %04X\n", r1);
		return false;
	}

	r1 = sd_setBlockLen(SD_DEFAULT_BLOCKLEN);

	if (r1)
	{
		LOG_ERR("setBlockLen failed: %04X\n", r1);
		return false;
	}

	sd_status = !r1;
	return sd_status;
}

DSTATUS sd_disk_initialize(BYTE drv)
{
	return sd_disk_status(drv);
}

DSTATUS sd_disk_status(BYTE drv)
{
	ASSERT(!drv);

	if (sd_status)
		return RES_OK;
	else
		return STA_NOINIT;
}

DRESULT sd_disk_read(BYTE drv, BYTE* buf, DWORD sector, BYTE count)
{
	ASSERT(!drv);

	if (!sd_status)
		return RES_NOTRDY;

	while (count--)
	{
		if (sd_readBlock(buf, sector * SD_DEFAULT_BLOCKLEN))
			return RES_ERROR;
		buf += SD_DEFAULT_BLOCKLEN;
		sector++;
	}
	return RES_OK;
}

DRESULT sd_disk_write(BYTE drv, const BYTE* buf, DWORD sector, BYTE count)
{
	ASSERT(!drv);

	if (!sd_status)
		return RES_NOTRDY;

	while (count--)
	{
		if (sd_writeBlock(buf, sector * SD_DEFAULT_BLOCKLEN))
			return RES_ERROR;
		buf += SD_DEFAULT_BLOCKLEN;
		sector++;
	}
	return RES_OK;
}

DRESULT sd_disk_ioctl(BYTE drv, BYTE cmd, void* buf)
{
	ASSERT(!drv);

	if (!sd_status)
		return RES_NOTRDY;

	switch (cmd)
	{
		case CTRL_SYNC:
			return RES_OK;

		case GET_SECTOR_SIZE:
			*(WORD *)buf = SD_DEFAULT_BLOCKLEN;
			return RES_OK;

		case GET_SECTOR_COUNT:
		{
			CardCSD csd;
			if (sd_getCSD(&csd))
				return RES_ERROR;
			else
			{
				*(DWORD *)buf = csd.block_num;
				return RES_OK;
			}

		}

		case GET_BLOCK_SIZE:
			*(DWORD *)buf = 1;
			return RES_OK;

		default:
			LOG_ERR("unknown command: [%d]\n", cmd);
			return RES_PARERR;
	}
}

DWORD get_fattime(void)
{
	return 0;
}

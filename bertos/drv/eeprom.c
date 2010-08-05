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
 * Copyright 2003, 2004, 2005, 2010 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief Driver for the 24xx16 and 24xx256 I2C EEPROMS (implementation)
 *
 * \author Stefano Fedrigo <aleph@develer.com>
 * \author Bernie Innocenti <bernie@codewiz.org>
 */

#include "eeprom.h"

#include "cfg/cfg_i2c.h"

#include <cfg/macros.h>  // MIN()
#include <cfg/debug.h>
#include <cfg/module.h>  // MOD_CHECK()

#include <cpu/attr.h>
#include <drv/i2c.h>

#include <drv/wdt.h>

#include <cpu/byteorder.h> // cpu_to_be16()

#include <string.h>  // memset()

/**
 * EEPROM ID code
 */
#define EEPROM_ID  0xA0

/**
 * This macros form the correct slave address for EEPROMs
 */
#define EEPROM_ADDR(x) (EEPROM_ID | (((uint8_t)((x) & 0x07)) << 1))


/**
 * Array used to describe EEPROM memory devices currently supported.
 */
static const EepromInfo mem_info[] =
{
	{
		/* 24XX08 */
		.has_dev_addr = false,
		.blk_size = 0x10,
		.e2_size = 0x400,
	},
	{
		/* 24XX16 */
		.has_dev_addr = false,
		.blk_size = 0x10,
		.e2_size = 0x800,
	},
	{
		/* 24XX256 */
		.has_dev_addr = true,
		.blk_size = 0x40,
		.e2_size = 0x8000,
	},
	{
		/* 24XX512 */
		.has_dev_addr = true,
		.blk_size = 0x80,
		.e2_size = 0x10000,
	},
	{
		/* 24XX1024 */
		.has_dev_addr = true,
		.blk_size = 0x100,
		.e2_size = 0x20000,
	},

	/* Add other memories here */
};

STATIC_ASSERT(countof(mem_info) == EEPROM_CNT);

/**
 * Check that the contents of an EEPROM range
 * match with a provided data buffer.
 *
 * \return true on success.
 * \note Seek position of \a fd will not change.
 */
bool eeprom_verify(Eeprom *fd, const void *buf, size_t count)
{
	uint8_t verify_buf[16];
	bool result = true;

	/* Save seek position */
	kfile_off_t prev_seek = fd->fd.seek_pos;

	while (count && result)
	{
		/* Split read in smaller pieces */
		size_t size = MIN(count, sizeof verify_buf);

		/* Read back buffer */
		if (eeprom_read(&fd->fd, verify_buf, size))
		{
			if (memcmp(buf, verify_buf, size) != 0)
			{
				TRACEMSG("Data mismatch!");
				result = false;
			}
		}
		else
		{
			TRACEMSG("Read error!");
			result = false;
		}

		/* Update count and addr for next operation */
		count -= size;
		buf = ((const char *)buf) + size;
	}

	/* Restore previous seek position */
	fd->fd.seek_pos = prev_seek;
	return result;
}

/**
 * Erase specified part of eeprom, writing 0xFF.
 *
 * \a addr   starting address
 * \a count  length of block to erase
 * \note Seek position is unchanged.
 * \return true if ok, false otherwise.
 */
bool eeprom_erase(Eeprom *fd, e2addr_t addr, e2_size_t count)
{
	e2blk_size_t blk_size = mem_info[fd->type].blk_size;
	uint8_t buf[blk_size];
	kfile_off_t prev_off = fd->fd.seek_pos;
	bool res = true;
	size_t size;

	memset(buf, 0xFF, blk_size);


	kfile_seek(&fd->fd, addr, KSM_SEEK_SET);

	/*
	 * Optimization: this first write id used to realign
	 * current address to block boundaries.
	 */

	wdt_reset();
	size = MIN(count, (e2_size_t)(blk_size - (addr & (blk_size - 1))));
	if (kfile_write(&fd->fd, buf, size) != size)
	{
		fd->fd.seek_pos = prev_off;
		return false;
	}
	count -= size;

	/* Clear all */
	while (count)
	{
		/* Long operation, reset watchdog */
		wdt_reset();

		size = MIN(count, (e2_size_t)sizeof buf);
		if (kfile_write(&fd->fd, buf, size) != size)
		{
			res = false;
			break;
		}

		count -= size;
	}
	fd->fd.seek_pos = prev_off;
	return res;
}


/**
 * Initialize EEPROM module.
 * \a fd is the Kfile context.
 * \a type is the eeprom device we want to initialize (\see EepromType)
 * \a addr is the i2c devide address (usually pins A0, A1, A2).
 * \a verify is true if you want that every write operation will be verified.
 */
void eeprom_init(Eeprom *fd, EepromType type, e2dev_addr_t addr, bool verify)
{
	MOD_CHECK(i2c);
	ASSERT(type < EEPROM_CNT);

	memset(fd, 0, sizeof(*fd));
	DB(fd->fd._type = KFT_EEPROM);

	fd->type = type;
	fd->addr = addr;
	fd->fd.size = mem_info[fd->type].e2_size;

	// Setup eeprom programming functions.
	fd->fd.read = eeprom_read;
	if (verify)
		fd->fd.write = eeprom_writeVerify;
	else
		fd->fd.write = eeprom_writeRaw;
	fd->fd.close = kfile_genericClose;

	fd->fd.seek = kfile_genericSeek;
}


static size_t eeprom_writeDirect(KBlock *blk, block_idx_t idx, const void *buf, size_t offset, size_t size)
{
	Eeprom *fd = EEPROM_CAST_KBLOCK(blk);
	e2dev_addr_t dev_addr;
	uint8_t addr_buf[2];
	uint8_t addr_len;
	uint32_t abs_addr = blk->blk_size * idx + offset;

	STATIC_ASSERT(countof(addr_buf) <= sizeof(e2addr_t));


	/* clamp size to memory limit (otherwise may roll back) */
	ASSERT(idx <= blk->blk_cnt);
	size = MIN(size, blk->blk_size - offset);

	if (mem_info[fd->type].has_dev_addr)
	{
		dev_addr = fd->addr;
		addr_len = 2;
	}
	else
	{
		dev_addr = (e2dev_addr_t)((abs_addr >> 8) & 0x07);
		addr_len = 1;
	}

	if (mem_info[fd->type].has_dev_addr)
	{
		addr_buf[0] = (abs_addr >> 8) & 0xFF;
		addr_buf[1] = (abs_addr & 0xFF);
	}
	else
	{
		dev_addr = (e2dev_addr_t)((abs_addr >> 8) & 0x07);
		addr_buf[0] = (abs_addr & 0xFF);
	}

	i2c_start_w(fd->i2c, EEPROM_ADDR(dev_addr),  addr_len + size, I2C_STOP);
	i2c_write(fd->i2c, addr_buf, addr_len);
	i2c_write(fd->i2c, buf, size);

	if (i2c_error(fd->i2c))
		return 0;

	return size;
}


static size_t eeprom_readDirect(struct KBlock *_blk, block_idx_t idx, void *_buf, size_t offset, size_t size)
{
	Eeprom *blk = EEPROM_CAST_KBLOCK(_blk);
	uint8_t addr_buf[2];
	uint8_t addr_len;
	size_t rd_len = 0;
	uint8_t *buf = (uint8_t *)_buf;
	uint32_t abs_addr = mem_info[blk->type].blk_size * idx + offset;

	STATIC_ASSERT(countof(addr_buf) <= sizeof(e2addr_t));

	/* clamp size to memory limit (otherwise may roll back) */
	ASSERT(idx <= blk->blk.blk_cnt);
	size = MIN(size, blk->blk.blk_size - offset);

	e2dev_addr_t dev_addr;
	if (mem_info[blk->type].has_dev_addr)
	{
		dev_addr = blk->addr;
		addr_len = 2;
		addr_buf[0] = (abs_addr >> 8) & 0xFF;
		addr_buf[1] = (abs_addr & 0xFF);
	}
	else
	{
		dev_addr = (e2dev_addr_t)((abs_addr >> 8) & 0x07);
		addr_len = 1;
		addr_buf[0] = (abs_addr & 0xFF);
	}


	i2c_start_w(blk->i2c, EEPROM_ADDR(dev_addr),  addr_len, I2C_NOSTOP);
	i2c_write(blk->i2c, addr_buf, addr_len);

	i2c_start_r(blk->i2c, EEPROM_ADDR(dev_addr), size, I2C_STOP);
	i2c_read(blk->i2c, buf, size);

	if (i2c_error(blk->i2c))
		   return rd_len;

	rd_len += size;

	return rd_len;
}

static int kblockEeprom_dummy(UNUSED_ARG(struct KBlock *,b))
{
	return 0;
}


static const KBlockVTable eeprom_unbuffered_vt =
{
	.readDirect = eeprom_readDirect,
	.writeDirect = eeprom_writeDirect,

	.error = kblockEeprom_dummy,
	.clearerr = (kblock_clearerr_t)kblockEeprom_dummy,
};

/**
 * Initialize EEPROM module.
 * \param b is the Kblock context.
 * \param type is the eeprom device we want to initialize (\see EepromType)
 * \param i2c context for i2c channel
 * \param addr is the i2c devide address (usually pins A0, A1, A2).
 */
void eeprom_init_5(Eeprom *blk, I2c *i2c, EepromType type, e2dev_addr_t addr, bool verify)
{
	ASSERT(type < EEPROM_CNT);

	memset(blk, 0, sizeof(*blk));
	DB(blk->blk.priv.type = KBT_EEPROM);

	blk->type = type;
	blk->addr = addr;
	blk->i2c = i2c;

	blk->blk.blk_size = mem_info[type].blk_size;
	blk->blk.blk_cnt = mem_info[type].e2_size / mem_info[type].blk_size;
	blk->blk.priv.flags |= KB_PARTIAL_WRITE;
	blk->blk.priv.vt = &eeprom_unbuffered_vt;
}



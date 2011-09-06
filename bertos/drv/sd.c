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
 * \author Francesco Sacchi <batt@develer.com>
 */


#include "sd.h"
#include "hw/hw_sd.h"
#include "cfg/cfg_sd.h"

#define LOG_LEVEL  SD_LOG_LEVEL
#define LOG_FORMAT SD_LOG_FORMAT
#include <cfg/log.h>

#include <io/kblock.h>

#include <string.h>


#ifdef SD_INCLUDE_SPI_SOURCE
	#include <drv/sd_spi.c>
#endif

void sd_writeTest(Sd *sd)
{
	uint8_t buf[SD_DEFAULT_BLOCKLEN];
	memset(buf, 0, sizeof(buf));

	for (block_idx_t i = 0; i < sd->b.blk_cnt; i++)
	{
		LOG_INFO("writing block %ld: %s\n", i, (kblock_read(&sd->b, i, buf, 0, SD_DEFAULT_BLOCKLEN) == SD_DEFAULT_BLOCKLEN) ? "OK" : "FAIL");
	}
}

bool sd_test(Sd *sd)
{
	uint8_t buf[SD_DEFAULT_BLOCKLEN];

	if (kblock_read(&sd->b, 0, buf, 0, sd->b.blk_size) != sd->b.blk_size)
		return false;

	kputchar('\n');
	for (int i = 0; i < SD_DEFAULT_BLOCKLEN; i++)
	{
		kprintf("%02X ", buf[i]);
		buf[i] = i;
		if (!((i+1) % 16))
			kputchar('\n');
	}

	if (kblock_write(&sd->b, 0, buf, 0, SD_DEFAULT_BLOCKLEN) != SD_DEFAULT_BLOCKLEN)
		return false;

	memset(buf, 0, sizeof(buf));
	if (kblock_read(&sd->b, 0, buf, 0, sd->b.blk_size) != sd->b.blk_size)
		return false;

	kputchar('\n');
	for (block_idx_t i = 0; i < sd->b.blk_size; i++)
	{
		kprintf("%02X ", buf[i]);
		buf[i] = i;
		if (!((i+1) % 16))
			kputchar('\n');
	}

	return true;
}




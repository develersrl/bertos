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
 * Copyright 2009 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief TAS5706A Power DAC i2c driver.
 *
 *
 * \version $Id$
 * \author Francesco Sacchi <batt@develer.com>
 */

#include "tas5706a.h"
#include <cfg/module.h>

#include <drv/i2c.h>
#include <drv/timer.h>

#include "hw/hw_tas5706a.h"
#include "cfg/cfg_tas5706a.h"

#define TAS_ADDR 0x36

typedef uint8_t tas_addr_t;

static bool tas5706a_send(tas_addr_t addr, const void *buf, size_t len)
{
	bool ret = i2c_start_w(TAS_ADDR) && i2c_put(addr) && i2c_send(buf, len);
	i2c_stop();
	return ret;
}

INLINE bool tas5706a_putc(tas_addr_t addr, uint8_t ch)
{
	return tas5706a_send(addr, &ch, sizeof(ch));
}

static bool tas5706a_recv(tas_addr_t addr, void *buf, size_t len)
{
	bool ret = i2c_start_w(TAS_ADDR) && i2c_put(addr) && i2c_start_r(TAS_ADDR) && i2c_recv(buf, len);
	i2c_stop();
	return ret;
}

INLINE int tas5706a_getc(tas_addr_t addr)
{
	uint8_t ch;
	if (tas5706a_recv(addr, &ch, sizeof(ch)))
		return (int)(uint8_t)ch;
	else
		return EOF;
}

#define TRIM_REG   0x1B
#define SYS_REG2   0x05
#define VOLUME_REG 0x07
#define MUTE_VOL 0xFF

#define DB_TO_REG(db) ((24 - (db)) * 2)

void tas5706a_init(void)
{
	MOD_CHECK(i2c);
	MOD_CHECK(timer);
	TAS5706A_PIN_INIT();
	timer_delay(200);
	TAS5706A_SETPOWERDOWN(false);
	TAS5706A_SETMUTE(false);
	TAS5706A_MCLK_INIT();
	timer_delay(2);
	TAS5706A_SETRESET(false);
	timer_delay(20);
	tas5706a_putc(TRIM_REG, 0x00);

	tas5706a_putc(VOLUME_REG, DB_TO_REG(CONFIG_TAS_MAX_VOL));

	/* Unmute */
	tas5706a_putc(SYS_REG2, 0);
}

#define CH1_VOL_REG 0x08
#define CH2_VOL_REG 0x09
#define CH3_VOL_REG 0x0A
#define CH4_VOL_REG 0x0B

void tas5706a_setVolume(Tas5706aCh ch, tas5706a_vol_t vol)
{
	ASSERT(ch < TAS_CNT);
	ASSERT(vol <= TAS_VOL_MAX);

	tas_addr_t addr1, addr2;

	switch(ch)
	{
		case TAS_CH1:
			addr1 = CH1_VOL_REG;
			addr2 = CH3_VOL_REG;
			break;
		case TAS_CH2:
			addr1 = CH2_VOL_REG;
			addr2 = CH4_VOL_REG;
			break;
		default:
			ASSERT(0);
			return;
	}

	uint8_t vol_att = 0xff - ((vol * 0xff) / TAS_VOL_MAX);

	tas5706a_putc(addr1, vol_att);
	tas5706a_putc(addr2, vol_att);
}

void tas5706a_setLowPower(bool val)
{
	TAS5706A_SETPOWERDOWN(val);
	TAS5706A_SETMUTE(val);
}


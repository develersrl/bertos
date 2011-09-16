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
 * Copyright 2011 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \author Daniele Basile <asterix@develer.com>
 *
 * \brief WM8731 Audio codec 2 wire driver.
 *
 */

#include "wm8731.h"

#include "hw/hw_wm8731.h"
#include "cfg/cfg_wm8731.h"

// Define logging setting (for cfg/log.h module).
#define LOG_LEVEL   WM8731_LOG_LEVEL
#define LOG_FORMAT  WM8731_LOG_FORMAT
#include <cfg/log.h>
#include <cfg/macros.h>

#include <cpu/irq.h>

#include <drv/i2c.h>


static void wm8731_write(Wm8731 *ctx, uint8_t reg, uint16_t value)
{

	uint16_t tmp = ((reg & 0x7F) << 9) | (value & 0x1FF);

	i2c_start_w(ctx->i2c, ctx->addr, 2, I2C_STOP);
	i2c_putc(ctx->i2c, (uint8_t)((tmp & 0xFF00) >> 8));
	i2c_putc(ctx->i2c, (uint8_t)(tmp & 0xFF));

	int err = i2c_error(ctx->i2c);
	if (err)
		LOG_ERR("Error[%d] while send command to codec.\n", err);

}

#define RANGECONV(data, y1, y2) (((((int32_t)(data)) * ((y2) - (y1))) / ((1 << 8) - 1)) + (y1))
void wm8731_setVolume(Wm8731 *ctx, uint16_t device, uint8_t volume)
{
	uint16_t value;

	if (device & WM8731_LINE_IN)
	{
		if (!volume)
		{
			wm8731_write(ctx, WM8731_REG_RIGHT_LINEIN, BV(WM8731_LINMUTE_BIT) | BV(WM8731_RLINBOTH_BIT));
			wm8731_write(ctx, WM8731_REG_LEFT_LINEIN, BV(WM8731_LINMUTE_BIT) | BV(WM8731_LRINBOTH_BIT));
		}

		value = DIV_ROUND(volume * WM8731_LINVOL_BITS_MASK, 100);

		wm8731_write(ctx, WM8731_REG_RIGHT_LINEIN, ~BV(WM8731_LINMUTE_BIT) | value);
		wm8731_write(ctx, WM8731_REG_LEFT_LINEIN, ~BV(WM8731_RINMUTE_BIT) | value);
		LOG_INFO("Set LINE IN vol[%d]%% raw[%d]\n", volume, value);
	}

	if (device & WM8731_HEADPHONE)
	{
		value = DIV_ROUND(volume * WM8731_RHPVOL_BITS_MASK, 100);

		wm8731_write(ctx, WM8731_REG_RIGHT_HPOUT, value | BV(WM8731_RZCEN_BIT) | BV(WM8731_RLHPBOTH_BIT));
		wm8731_write(ctx, WM8731_REG_LEFT_HPOUT, value  | BV(WM8731_LZCEN_BIT) | BV(WM8731_LRHPBOTH_BIT));
		LOG_INFO("Set HEADPHONE vol[%d]%% raw[%d]\n", volume, value);
	}

}

void wm8731_powerOn(Wm8731 *ctx)
{
	LOG_INFO("Power on codec\n");
	wm8731_write(ctx, WM8731_REG_PWDOWN_CTRL, ~BV(WM8731_POWEROFF_BIT) & 0x7F);
}

void wm8731_powerOff(Wm8731 *ctx)
{
	LOG_INFO("Power off codec\n");
	wm8731_write(ctx, WM8731_REG_PWDOWN_CTRL, BV(WM8731_POWEROFF_BIT) & 0x7F);
}

void wm8731_powerOnDevices(Wm8731 *ctx, uint16_t device)
{
	wm8731_write(ctx, WM8731_REG_PWDOWN_CTRL, ~device & 0x7F);
	LOG_INFO("Turn on the devices[%x]\n", ~device & 0x7F);
}

void wm8731_powerOffDevices(Wm8731 *ctx, uint16_t device)
{
	wm8731_write(ctx, WM8731_REG_PWDOWN_CTRL, device & 0x7F);
	LOG_INFO("Turn off the devices[%x]\n", device);
}

void wm8731_init(Wm8731 *ctx, I2c *i2c, uint8_t codec_addr)
{
	ctx->i2c = i2c;
	ctx->addr = codec_addr;

	WM8731_PIN_INIT();
	WM8731_MCLK_INIT();

	LOG_INFO("Init WM8731 codec.\n");

	/* Reset codec and active it */
	wm8731_write(ctx, WM8731_REG_RESET, 0);


	/* Configure the codec */
	wm8731_write(ctx, WM8731_REG_DIGITAL_PATH_CTRL, CONFIG_WM8731_DEEMP | CONFIG_WM8731_DAPC | (CONFIG_WM8731_DACMU << WM8731_DACMU));

	#if CONFIG_WM8731_MICBOOST
	wm8731_write(ctx, WM8731_REG_ANALOGUE_PATH_CTRL, BV(WM8731_MICBOOST) | CONFIG_WM8731_INSEL | CONFIG_WM8731_BYPASS | CONFIG_WM8731_SIDEATT);
	#else
	wm8731_write(ctx, WM8731_REG_ANALOGUE_PATH_CTRL, (CONFIG_WM8731_INSEL | CONFIG_WM8731_BYPASS | CONFIG_WM8731_SIDEATT) & ~BV(WM8731_MICBOOST));
	#endif

	#if CONFIG_WM8731_MS
	wm8731_write(ctx, WM8731_REG_DA_INTERFACE_FORMAT, CONFIG_WM8731_INTERFACE_FORMAT | CONFIG_WM8731_IWL_BITS | BV(WM8731_MS_BIT));
	#else
	wm8731_write(ctx, WM8731_REG_DA_INTERFACE_FORMAT,
		(CONFIG_WM8731_INTERFACE_FORMAT | CONFIG_WM8731_IWL_BITS | BV(WM8731_MS_BIT)) & ~BV(WM8731_MS_BIT));
	#endif

	wm8731_write(ctx, WM8731_REG_SAMPLECTRL, CONFIG_WM8731_SAMPLING_RATES);

	/* By default we turn on all devices and disable only the outclock */
	wm8731_write(ctx, WM8731_REG_ACTIVE_CTRL, 1);
	wm8731_write(ctx, WM8731_REG_PWDOWN_CTRL, 0x40);
}

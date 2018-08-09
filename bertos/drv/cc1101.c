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
 * Copyright 2012 Develer S.r.l. (http://www.develer.com/)
 * All Rights Reserved.
 * -->
 *
 * \brief Texas radio chip CC1101 interface.
 *
 * \author Daniele Basile <asterix@develer.com>
 */

#include "cc1101.h"

#include "hw/hw_cc1101.h"
#include "hw/hw_spi.h"

#include <cpu/types.h>
#include <cpu/power.h>

#include <drv/timer.h>
#include <drv/spi.h>

#define WAIT_SO_LOW()  \
	do { \
		ticks_t start = timer_clock(); \
		while(SPI_HW_IS_MISO_HIGH()) \
		{ \
			if (timer_clock() - start > ms_to_ticks(CC1101_TIMEOUT_ERR)) \
				break; \
			cpu_relax(); \
		} \
	} while(0)

/**
 * Read data from chip at given address.
 * If we read that more one byte we use the burst mode (see datasheet for more
 * details).
 */
uint8_t cc1101_read(uint8_t addr, uint8_t *buf, size_t len)
{
	SS_ACTIVE();
	WAIT_SO_LOW();

	uint8_t status = 0xFF;

	status = spi_sendRecv(addr | 0xc0);
	spi_read(buf, len);

	SS_INACTIVE();
    return status;
}

/**
 * Write data to chip at given address.
 * If we write that more one byte we use the burst mode (see datasheet for more
 * details).
 */
uint8_t cc1101_write(uint8_t addr, const uint8_t *buf, size_t len)
{
	SS_ACTIVE();
	WAIT_SO_LOW();

	uint8_t status = 0xFF;
	if (len == 1)
	{
		spi_sendRecv(addr);
		status = spi_sendRecv(buf[0]);
	}
	else
	{
		status = spi_sendRecv(addr | 0x40);
		spi_write(buf, len);
	}

	SS_INACTIVE();
    return status;
}

/**
 * Send the strobe command.
 * These commands not have any data, the chip exec command
 * by only writing to one strobe address command.
 */
uint8_t cc1101_strobe(uint8_t addr)
{
	SS_ACTIVE();
	WAIT_SO_LOW();

    uint8_t status = spi_sendRecv(addr);

	SS_INACTIVE();
    return status;
}

/**
 * Manual chip reset, refer to datasheet for
 * the procedure.
 */
void cc1101_powerOnReset(void)
{
	SPI_HW_SCK_ACTIVE();
	SPI_HW_MOSI_LOW();
	timer_udelay(10);

	SS_INACTIVE();
	timer_udelay(10);
	SS_ACTIVE();
	timer_udelay(1);
	SS_INACTIVE();
	timer_udelay(40);

	cc1101_strobe(CC1101_SRES);

	timer_udelay(1);
	SS_ACTIVE();
	WAIT_SO_LOW();
	timer_udelay(50);
	SS_INACTIVE();
}

/**
 * Reset the chip and apply the settings.
 */
void cc1101_init(const Setting  *settings)
{
	CC1101_HW_INIT();

	cc1101_powerOnReset();

	for (int i = 0; settings[i].addr != 0xFF && settings[i].data != 0xFF; i++)
	{
		cc1101_write(settings[i].addr, &settings[i].data, sizeof(uint8_t));
	}
}

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
 * Copyright 2003, 2004, 2006 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2000 Bernie Innocenti <bernie@codewiz.org>
 *
 * -->
 *
 * \brief Emulated SPI Master for DSP firmware download (impl.)
 *
 *
 * \author Francesco Sacchi <batt@develer.com>
 * \author Daniele Basile <asterix@develer.com>
 */


#include "spi_bitbang.h"

#include "hw/hw_spi.h"
#include "cfg/cfg_spi.h"

#include <cfg/module.h>

#include <cpu/irq.h>

#include <drv/spi.h>

INLINE void spi_bitbang_assertSS(Spi *spi)
{
	(void)spi;
	ATOMIC(SPI_HW_SS_ACTIVE());
}

INLINE void spi_bitbang_deassertSS(Spi *spi)
{
	(void)spi;
	ATOMIC(SPI_HW_SS_INACTIVE());
}

/**
 * Send byte \c c over MOSI line, CONFIG_SPI_DATAORDER first.
 * SS pin state is left unchanged.
 */
INLINE uint8_t spi_bitbang_sendRecv(Spi *spi, uint8_t c)
{
	(void)spi;
	uint8_t data = 0;
	uint8_t shift = SPI_DATAORDER_START;


	ATOMIC(
		for (int i = 0; i < 8; i++)
		{
			/* Shift the i-th bit to MOSI */
			if (c & shift)
				SPI_HW_MOSI_HIGH();
			else
				SPI_HW_MOSI_LOW();
			/* Assert clock */
			SPI_HW_SCK_ACTIVE();
			data |= SPI_HW_IS_MISO_HIGH() ? shift : 0;
			/* De-assert clock */
			SPI_HW_SCK_INACTIVE();
			SPI_DATAORDER_SHIFT(shift);
		}
	);
	return data;
}

static const SpiVT spi_bitbang_vt =
{
	.assertSS = spi_bitbang_assertSS,
	.deassertSS = spi_bitbang_deassertSS,
	.sendRecv = spi_bitbang_sendRecv,
};

void spi_hw_bitbangInit(Spi *spi, int dev)
{
	(void)dev;
	spi->vt = &spi_bitbang_vt;
	ATOMIC(SPI_HW_BITBANG_INIT());
}



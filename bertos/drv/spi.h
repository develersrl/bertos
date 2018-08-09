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
 * \brief Spi interface
 *
 * \author Daniele Basile <asterix@develer.com>
 *
 * $WIZ$ module_name = "spi"
 * $WIZ$ module_configuration = "bertos/cfg/cfg_spi.h"
 * $WIZ$ module_hw = "bertos/hw/hw_spi.h"
 * $WIZ$ module_depends = "spi_bitbang"
 *
 */

#ifndef DRV_SPI_H
#define DRV_SPI_H

#include "hw/hw_spi.h"
#include "cfg/cfg_spi.h"

#include <cfg/compiler.h>
#include <cfg/macros.h>
#include <cfg/debug.h>

#include <drv/spi.h>

#include <cpu/types.h>
#include <cpu/attr.h>

/**
 * \name SPI bitbang devices enum
 */
enum
{
	SPI_BITBANG_OLD = -1,
	SPI_BITBANG0 = 1000, ///< Use bitbang on port 0
	SPI_BITBANG1,        ///< Use bitbang on port 1
	SPI_BITBANG2,
	SPI_BITBANG3,
	SPI_BITBANG4,
	SPI_BITBANG5,
	SPI_BITBANG6,
	SPI_BITBANG7,
	SPI_BITBANG8,
	SPI_BITBANG9,

	SPI_BITBANG_CNT  /**< Number of i2c ports */
};

/**
 * Define send and receive order bit.
 *
 * $WIZ$ ordet_bit_list = "SPI_LSB_FIRST", "SPI_MSB_FIRST"
 */
#define SPI_LSB_FIRST 1
#define SPI_MSB_FIRST 2


/**
 * SPI Errors
 */
#define SPI_SENDRECV_TIMEOUT     BV(0)

#include CPU_HEADER(spi)

struct SpiHardware;
struct Spi;

typedef void (*spi_assertSS_t)(struct Spi *spi);
typedef void (*spi_deassertSS_t)(struct Spi *spi);
typedef uint8_t (*spi_sendRecv_t)(struct Spi *spi, uint8_t c);

typedef struct SpiVT
{
	spi_assertSS_t    assertSS;
	spi_deassertSS_t  deassertSS;
	spi_sendRecv_t    sendRecv;
} SpiVT;

typedef struct Spi
{
	int errors;
	int flags;
	struct SpiHardware* hw;
	const struct SpiVT *vt;
} Spi;

INLINE int spi_error(Spi *spi)
{
	ASSERT(spi);
	int err = spi->errors;
	spi->errors = 0;

	return err;
}

INLINE void spi_assertSS(Spi *spi)
{
	ASSERT(spi);
	ASSERT(spi->vt);
	ASSERT(spi->vt->assertSS);
	spi->vt->assertSS(spi);
}

INLINE void spi_deassertSS(Spi *spi)
{
	ASSERT(spi);
	ASSERT(spi->vt);
	ASSERT(spi->vt->deassertSS);
	spi->vt->deassertSS(spi);
}

INLINE uint8_t spi_sendRecv(Spi *spi, uint8_t c)
{
	ASSERT(spi);
	ASSERT(spi->vt);
	ASSERT(spi->vt->sendRecv);

	return spi->vt->sendRecv(spi, c);
}

/**
 * Read \param len from spi, and put it in \param _buff.
 */
INLINE void spi_read(Spi *spi, void *_buff, size_t len)
{
	ASSERT(spi);
	ASSERT(spi->vt);
	ASSERT(spi->vt->sendRecv);

	ASSERT(_buff);
	uint8_t *buff = (uint8_t *)_buff;

	while (len--)
		/* Read byte from spi and put it in buffer. */
		*buff++ = spi->vt->sendRecv(spi, CONFING_SPI_NOP);
}

/**
 * Write \param len to spi, and take it from \param _buff.
 */
INLINE void spi_write(Spi *spi, const void *_buff, size_t len)
{
	ASSERT(spi);
	ASSERT(spi->vt);
	ASSERT(spi->vt->sendRecv);

	ASSERT(_buff);
	const uint8_t *buff = (const uint8_t *)_buff;

	while (len--)
		/* Write byte pointed at by *buff to spi */
		spi->vt->sendRecv(spi, *buff++);
}

void spi_hw_init(Spi *spi, int dev, uint32_t clock);
void spi_hw_bitbangInit(Spi *spi, int dev);

#define spi_init(spi, dev, clock)   ((((dev) >= (int)SPI_BITBANG0)) ? \
									spi_hw_bitbangInit((spi), (dev)) : \
									spi_hw_init((spi), (dev), (clock)))

#endif


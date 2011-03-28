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
 * \defgroup dac Generic DAC driver
 * \ingroup drivers
 * \{
 * \brief Digital to Analog Converter driver (DAC).
 *
 * <b>Configuration file</b>: cfg_dac.h
 *
 * \author Daniele Basile <asterix@develer.com>
 *
 * $WIZ$ module_name = "dac"
 * $WIZ$ module_configuration = "bertos/cfg/cfg_dac.h"
 * $WIZ$ module_supports = "sam3x"
 */


#ifndef DRV_DAC_H
#define DRV_DAC_H

#include <cfg/compiler.h>
#include <cfg/debug.h>
#include <cfg/macros.h>
#include <cpu/attr.h>
#include CPU_HEADER(dac)

struct DacContext;

typedef int (*DacWriteFunc_t) (struct DacContext *ctx, unsigned channel, uint16_t sample);
typedef void (*SetChannelMaskFunc_t) (struct DacContext *ctx, uint32_t mask);
typedef void (*SetSamplingRate_t) (struct DacContext *ctx, uint32_t rate);
typedef void (*DmaConversionBufFunc_t) (struct DacContext *ctx, void *buf, size_t len);
typedef bool (*DmaConversionIsFinished_t) (struct DacContext *ctx);
typedef void (*DmaStartStreamingFunc_t) (struct DacContext *ctx, void *buf, size_t len, size_t slicelen);
typedef void (*DmaStopFunc_t) (struct DacContext *ctx);

typedef struct DacContext
{
	DacWriteFunc_t write;
	SetChannelMaskFunc_t setCh;
	SetSamplingRate_t setSampleRate;
	DmaConversionBufFunc_t conversion;
	DmaConversionIsFinished_t isFinished;
	DmaStartStreamingFunc_t start;
	DmaStopFunc_t stop;
	size_t slicelen;

	DB(id_t _type);
} DacContext;

INLINE int dac_write(DacContext *ctx, unsigned channel, uint16_t sample)
{
	ASSERT(ctx->write);
	return ctx->write(ctx, channel, sample);
}

INLINE void dac_setChannelMask(struct DacContext *ctx, uint32_t mask)
{
	ASSERT(ctx->setCh);
	ctx->setCh(ctx, mask);
}

INLINE void dac_setSamplingRate(struct DacContext *ctx, uint32_t rate)
{
	ASSERT(ctx->setSampleRate);
	ctx->setSampleRate(ctx, rate);
}

/**
 * Convert \param len samples stored into \param buf.
 */
INLINE void dac_dmaConversionBuffer(struct DacContext *ctx, void *buf, size_t len)
{
	ASSERT(ctx->conversion);
	ctx->conversion(ctx, buf, len);
}

/**
 * Check if a dma transfer is finished.
 *
 * Useful for kernel-less applications.
 */
INLINE bool dac_dmaIsFinished(struct DacContext *ctx)
{
	ASSERT(ctx->isFinished);
	return ctx->isFinished(ctx);
}

/**
 * \param slicelen Must be a divisor of len, ie. len % slicelen == 0.
 */
INLINE void dac_dmaStartStreaming(struct DacContext *ctx, void *buf, size_t len, size_t slicelen)
{
	ASSERT(ctx->start);
	ASSERT(len % slicelen == 0);
	ctx->slicelen = slicelen;
	ctx->start(ctx, buf, len, slicelen);
}

INLINE void dac_dmaStop(struct DacContext *ctx)
{
	ASSERT(ctx->stop);
	ctx->stop(ctx);
}

#define dac_bits() DAC_BITS

void dac_init(struct DacContext *ctx);

/** \} */ //defgroup dac
#endif /* DRV_DAC_H */

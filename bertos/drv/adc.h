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
 * Copyright 2005 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \defgroup adc Generic ADC driver
 * \ingroup drivers
 * \{
 * \brief Analog to Digital Converter driver (ADC).
 *
 * <b>Configuration file</b>: cfg_adc.h
 *
 * \author Francesco Sacchi <batt@develer.com>
 *
 * $WIZ$ module_name = "adc"
 * $WIZ$ module_configuration = "bertos/cfg/cfg_adc.h"
 * $WIZ$ module_supports = "not atmega103"
 */


#ifndef DRV_ADC_H
#define DRV_ADC_H

#include "cfg/cfg_adc.h"

#include <cfg/compiler.h>
#include <cfg/debug.h>
#include <cfg/macros.h>
#include <cpu/attr.h>
#include CPU_HEADER(adc)

#define CONFIG_ADC_STREAMING_API   0

/** Type for ADC return value. */
typedef uint16_t adcread_t;

/** Type for channel */
typedef uint8_t adc_ch_t;

struct AdcContext;

typedef void (*streaming_hook_t) (struct AdcContext *ctx, void *buf, size_t slicelen);

typedef adcread_t (*adc_read_t) (struct AdcContext *ctx, adc_ch_t channel);
typedef void (*adc_set_channel_mask_t) (struct AdcContext *ctx, uint32_t mask);
typedef void (*adc_set_sampling_rate_t) (struct AdcContext *ctx, uint32_t rate);
//typedef void (*adc_set_streaming_hook_t) (struct AdcContext *ctx, size_t hook_slicelen, streaming_hook_t hook);
typedef void (*adc_dma_acquire_buf_t) (struct AdcContext *ctx, void *buf, size_t len);
typedef bool (*adc_dma_acquisition_finished_t) (struct AdcContext *ctx);
typedef void (*adc_dma_start_streaming_t) (struct AdcContext *ctx);
typedef void *(*adc_dma_wait_conversion_t) (struct AdcContext *ctx);
typedef void (*adc_dma_stop_t) (struct AdcContext *ctx);

typedef struct AdcContext
{
	adc_read_t read;
#if CONFIG_ADC_STREAMING_API
	adc_set_channel_mask_t set_ch;
	adc_set_sampling_rate_t setSampleRate;
	//adc_set_streaming_hook_t setStreamingHook;
	adc_dma_acquire_buf_t acquire;
	adc_dma_acquisition_finished_t isFinished;
	adc_dma_start_streaming_t start;
	adc_dma_wait_conversion_t wait;
	adc_dma_stop_t stop;
	uint8_t *base_buf;
	uint8_t * volatile head;
	uint8_t * volatile tail;
	uint8_t * volatile sliceend;
	size_t buf_len;
	size_t slicelen;
	size_t hook_slicelen;
	streaming_hook_t streaming_hook;
#endif
	DB(id_t _type);
} AdcContext;

INLINE adcread_t adc_read2(AdcContext *ctx, adc_ch_t channel)
{
	ASSERT(ctx->read);
	return ctx->read(ctx, channel);
}

#if CONFIG_ADC_STREAMING_API
	INLINE void adc_setChannelMask(struct AdcContext *ctx, uint32_t mask)
	{
		ASSERT(ctx->set_ch);
		ctx->set_ch(ctx, mask);
	}

	INLINE void adc_setSamplingRate(struct AdcContext *ctx, uint32_t rate)
	{
		ASSERT(ctx->setSampleRate);
		ctx->setSampleRate(ctx, rate);
	}

	INLINE void adc_setStreamingHook(struct AdcContext *ctx, size_t hook_slicelen, streaming_hook_t hook)
	{
		//ASSERT(ctx->setStreamingHook);
		ASSERT(hook_slicelen);
		ASSERT(hook);

		ctx->hook_slicelen = hook_slicelen;
		ctx->streaming_hook = hook;
		//ctx->setStreamingHook(ctx, hook_slicelen, hook);
	}

	INLINE void adc_dmaAcquireBuffer(struct AdcContext *ctx, void *buf, size_t len)
	{
		ASSERT(ctx->acquire);
		ctx->acquire(ctx, buf, len);
	}

	/**
	 * Check if a dma transfer is finished.
	 *
	 * Useful for kernel-less applications.
	 */
	INLINE bool adc_dmaIsFinished(struct AdcContext *ctx)
	{
		ASSERT(ctx->isFinished);
		return ctx->isFinished(ctx);
	}

	/**
	 * \param slicelen Must be a divisor of len, ie. len % slicelen == 0.
	 */
	INLINE void adc_dmaStartStreaming(struct AdcContext *ctx, void *buf, size_t len, size_t slicelen)
	{
		ASSERT(ctx->start);
		ASSERT(len % slicelen == 0);
		DB(
			if (ctx->streaming_hook)
			{
				ASSERT(len % ctx->hook_slicelen == 0);
				ASSERT(slicelen % ctx->hook_slicelen == 0);
			}
		);
		ASSERT(buf);

		ctx->buf_len = len;
		ctx->base_buf = ctx->head = ctx->tail = buf;
		ctx->slicelen = slicelen;
		ctx->start(ctx);
	}

	INLINE void *adc_dmaWaitConversion(struct AdcContext *ctx)
	{
		ASSERT(ctx->wait);
		return ctx->wait(ctx);
	}

	INLINE void adc_dmaStop(struct AdcContext *ctx)
	{
		ASSERT(ctx->stop);
		ctx->stop(ctx);
	}
#endif

#define adc_bits() ADC_BITS

adcread_t adc_read(adc_ch_t ch);
void adc_init(void);

/**
 * Macro used to convert data from adc range (0...(2 ^ADC_BITS - 1)) to
 * \a y1 ... \a y2 range.
 * \note \a y1, \a y2 can be negative, and put in ascending or descending order as well.
 * \note \a data and \a y2 are evaluated only once, \a y1 twice.
 */
#define ADC_RANGECONV(data, y1, y2) (((((int32_t)(data)) * ((y2) - (y1))) / ((1 << ADC_BITS) - 1)) + (y1))

/** \} */ //defgroup adc
#endif /* DRV_ADC_H */

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
 * \defgroup i2s Generic I2S driver
 * \ingroup drivers
 * \{
 * \brief
 *
 * <b>Configuration file</b>: cfg_i2s.h
 *
 * \author Daniele Basile <asterix@develer.com>
 *
 * $WIZ$ module_name = "i2s"
 * $WIZ$ module_configuration = "bertos/cfg/cfg_i2s.h"
 * $WIZ$ module_supports = "not all"
 */


#ifndef DRV_I2S_H
#define DRV_I2S_H

#warning __FILTER_NEXT_WARNING__
#warning This API is ALPHA! we could change it..

#include <cfg/compiler.h>
#include <cfg/debug.h>
#include <cfg/macros.h>

#include <cpu/attr.h>

#include CPU_HEADER(i2s)

struct I2sContext;
struct I2s;

typedef int (*i2s_write_t) (struct I2s *i2s, uint32_t sample);
typedef uint32_t (*i2s_read_t) (struct I2s *i2s);
typedef void (*i2s_dma_tx_buf_t) (struct I2s *i2s, void *buf, size_t len);
typedef void (*i2s_dma_rx_buf_t) (struct I2s *i2s, void *buf, size_t len);
typedef bool (*i2s_dma_tx_is_finished_t) (struct I2s *i2s);
typedef bool (*i2s_dma_rx_is_finished_t) (struct I2s *i2s);
typedef void (*i2s_dma_callback_t) (struct I2s *i2s, void *_buf, size_t len);
typedef void (*i2s_dma_start_streaming_t) (struct I2s *i2s, void *buf, size_t len, size_t slice_len);
typedef void (*i2s_dma_wait_t) (struct I2s *i2s);
typedef void (*i2s_dma_stop_t) (struct I2s *i2s);

typedef struct I2sContext
{
	i2s_write_t                write;
	i2s_dma_tx_buf_t           tx_buf;
	i2s_dma_tx_is_finished_t   tx_isFinish;
	i2s_dma_callback_t         tx_callback;
	i2s_dma_start_streaming_t  tx_start;
	i2s_dma_wait_t             tx_wait;
	i2s_dma_stop_t             tx_stop;
	size_t tx_slice_len;

	i2s_read_t                 read;
	i2s_dma_rx_buf_t           rx_buf;
	i2s_dma_rx_is_finished_t   rx_isFinish;
	i2s_dma_callback_t         rx_callback;
	i2s_dma_start_streaming_t  rx_start;
	i2s_dma_wait_t             rx_wait;
	i2s_dma_stop_t             rx_stop;
	size_t rx_slice_len;

	DB(id_t _type);

} I2sContext;

typedef struct I2s
{
	I2sContext ctx;
	struct I2sHardware *hw;
} I2s;

INLINE int i2s_write(I2s *i2s, uint32_t sample)
{
	ASSERT(i2s->ctx.write);
	return i2s->ctx.write(i2s, sample);
}


INLINE uint32_t i2s_read(I2s *i2s)
{
	ASSERT(i2s->ctx.read);
	return i2s->ctx.read(i2s);
}

/*
 * Check if a dma transfer is finished.
 *
 * Useful for kernel-less applications.
 */
INLINE bool i2s_dmaTxIsFinished(I2s *i2s)
{
	ASSERT(i2s->ctx.tx_isFinish);
	return i2s->ctx.tx_isFinish(i2s);
}

INLINE bool i2s_dmaRxIsFinished(I2s *i2s)
{
	ASSERT(i2s->ctx.rx_isFinish);
	return i2s->ctx.rx_isFinish(i2s);
}

INLINE void i2s_dmaTxBuffer(I2s *i2s, void *buf, size_t len)
{
	ASSERT(i2s->ctx.tx_buf);
	i2s->ctx.tx_buf(i2s, buf, len);
}

INLINE void i2s_dmaRxBuffer(I2s *i2s, void *buf, size_t len)
{
	ASSERT(i2s->ctx.rx_buf);
	i2s->ctx.rx_buf(i2s, buf, len);
}


INLINE void i2s_dmaTxWait(I2s *i2s)
{
	ASSERT(i2s->ctx.tx_wait);
	i2s->ctx.tx_wait(i2s);
}


INLINE void i2s_dmaStartTxStreaming(I2s *i2s, void *buf, size_t len, size_t slice_len, i2s_dma_callback_t callback)
{
	ASSERT(i2s->ctx.tx_start);
	ASSERT(len % slice_len == 0);
	ASSERT(callback);

	i2s->ctx.tx_callback = callback;
	i2s->ctx.tx_slice_len = slice_len;
	i2s->ctx.tx_start(i2s, buf, len, slice_len);
}

INLINE void i2s_dmaTxStop(I2s *i2s)
{
	ASSERT(i2s->ctx.tx_stop);
	i2s->ctx.tx_stop(i2s);
}

INLINE void i2s_dmaStartRxStreaming(I2s *i2s, void *buf, size_t len, size_t slice_len, i2s_dma_callback_t callback)
{
	ASSERT(i2s->ctx.rx_start);
	ASSERT(len % slice_len == 0);
	ASSERT(callback);

	i2s->ctx.rx_callback = callback;
	i2s->ctx.rx_slice_len = slice_len;
	i2s->ctx.rx_start(i2s, buf, len, slice_len);
}

INLINE void i2s_dmaRxStop(I2s *i2s)
{
	ASSERT(i2s->ctx.rx_stop);
	i2s->ctx.rx_stop(i2s);
}

void i2s_init(I2s *i2s, int channel);

/** \} */ //defgroup i2s
#endif /* DRV_I2S_H */

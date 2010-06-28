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
 * \author Francesco Sacchi <batt@develer.com>
 *
 * \brief KBlock interface
 */


#include "kblock.h"
#include <cfg/log.h>

INLINE size_t kblock_readDirect(struct KBlock *b, block_idx_t index, void *buf, size_t offset, size_t size)
{
	KB_ASSERT_METHOD(b, readDirect);
	return b->priv.vt->readDirect(b, index, buf, offset, size);
}

INLINE size_t kblock_readBuf(struct KBlock *b, void *buf, size_t offset, size_t size)
{
	KB_ASSERT_METHOD(b, readBuf);
	ASSERT(offset + size <= b->blk_size);

	return b->priv.vt->readBuf(b, buf, offset, size);
}

INLINE size_t kblock_writeBuf(struct KBlock *b, const void *buf, size_t offset, size_t size)
{
	KB_ASSERT_METHOD(b, writeBuf);
	ASSERT(offset + size <= b->blk_size);
	return b->priv.vt->writeBuf(b, buf, offset, size);
}

INLINE int kblock_load(struct KBlock *b, block_idx_t index)
{
	KB_ASSERT_METHOD(b, load);
	ASSERT(index < b->blk_cnt);

	return b->priv.vt->load(b, b->priv.blk_start + index);
}

INLINE int kblock_store(struct KBlock *b, block_idx_t index)
{
	KB_ASSERT_METHOD(b, store);
	ASSERT(index < b->blk_cnt);

	return b->priv.vt->store(b, b->priv.blk_start + index);
}



size_t kblock_read(struct KBlock *b, block_idx_t idx, void *_buf, size_t offset, size_t size)
{
	size_t tot_rd = 0;
	uint8_t *buf = (uint8_t *)_buf;

	ASSERT(b);
	ASSERT(buf);

	while (size)
	{
		size_t len = MIN(size, b->blk_size - offset);
		size_t rlen;

		if (idx == b->priv.curr_blk)
			rlen = kblock_readBuf(b, buf, offset, len);
		else
			rlen = kblock_readDirect(b, idx, buf, offset, len);

		tot_rd += rlen;
		if (rlen != len)
			break;

		idx++;
		size -= rlen;
		offset = 0;
		buf += rlen;
	}

	return tot_rd;
}


int kblock_flush(struct KBlock *b)
{
	ASSERT(b);

	if (b->priv.cache_dirty)
	{
		if (kblock_store(b, b->priv.curr_blk) == 0)
			b->priv.cache_dirty = false;
		else
			return EOF;
	}
	return 0;
}


static bool kblock_loadPage(struct KBlock *b, block_idx_t idx)
{
	ASSERT(b);

	if (idx != b->priv.curr_blk)
	{
		if (kblock_flush(b) != 0 || kblock_load(b, idx) != 0)
				return false;

		b->priv.curr_blk = idx;
	}
	return true;
}


size_t kblock_write(struct KBlock *b, block_idx_t idx, const void *_buf, size_t offset, size_t size)
{
	size_t tot_wr = 0;
	const uint8_t *buf = (const uint8_t *)_buf;

	ASSERT(b);
	ASSERT(buf);

	while (size)
	{
		size_t len = MIN(size, b->blk_size - offset);
		size_t wlen;

		if (!kblock_loadPage(b, idx))
			break;

		wlen = kblock_writeBuf(b, buf, offset, len);
		b->priv.cache_dirty = true;

		tot_wr += wlen;
		if (wlen != len)
			break;

		idx++;
		size -= wlen;
		offset = 0;
		buf += wlen;
	}

	return tot_wr;
}

int kblock_copy(struct KBlock *b, block_idx_t idx1, block_idx_t idx2)
{
	ASSERT(b);

	if (!kblock_loadPage(b, idx1))
		return EOF;

	b->priv.curr_blk = idx2;
	b->priv.cache_dirty = true;
	return 0;
}


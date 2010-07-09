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

#define LOG_LEVEL LOG_LVL_ERR
#define LOG_FORMAT LOG_FMT_VERBOSE

#include <cfg/log.h>
#include <string.h>

INLINE size_t kblock_readDirect(struct KBlock *b, block_idx_t index, void *buf, size_t offset, size_t size)
{
	KB_ASSERT_METHOD(b, readDirect);
	ASSERT(index < b->blk_cnt);
	return b->priv.vt->readDirect(b, b->priv.blk_start + index, buf, offset, size);
}

INLINE int kblock_writeBlock(struct KBlock *b, block_idx_t index, const void *buf)
{
	KB_ASSERT_METHOD(b, writeBlock);
	ASSERT(index < b->blk_cnt);
	return b->priv.vt->writeBlock(b, b->priv.blk_start + index, buf);
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

	LOG_INFO("index %d\n", index);
	return b->priv.vt->load(b, b->priv.blk_start + index);
}

INLINE int kblock_store(struct KBlock *b, block_idx_t index)
{
	KB_ASSERT_METHOD(b, store);
	ASSERT(index < b->blk_cnt);

	LOG_INFO("index %d\n", index);
	return b->priv.vt->store(b, b->priv.blk_start + index);
}

INLINE void kblock_setDirty(struct KBlock *b, bool dirty)
{
	if (dirty)
		b->priv.flags |= KB_CACHE_DIRTY;
	else
		b->priv.flags &= ~KB_CACHE_DIRTY;
}



size_t kblock_read(struct KBlock *b, block_idx_t idx, void *buf, size_t offset, size_t size)
{
	ASSERT(b);
	ASSERT(buf);
	ASSERT(offset + size <= b->blk_size);
	LOG_INFO("blk_idx %d, offset %d, size %d\n", idx, offset, size);

	if (kblock_buffered(b) && idx == b->priv.curr_blk)
		return kblock_readBuf(b, buf, offset, size);
	else
		return kblock_readDirect(b, idx, buf, offset, size);
}


int kblock_flush(struct KBlock *b)
{
	ASSERT(b);

	if (!kblock_buffered(b))
		return 0;

	if (kblock_cacheDirty(b))
	{
		LOG_INFO("flushing block %d\n", b->priv.curr_blk);
		if (kblock_store(b, b->priv.curr_blk) == 0)
			kblock_setDirty(b, false);
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
		LOG_INFO("loading block %d\n", idx);
		if (kblock_flush(b) != 0 || kblock_load(b, idx) != 0)
				return false;

		b->priv.curr_blk = idx;
	}
	return true;
}


size_t kblock_write(struct KBlock *b, block_idx_t idx, const void *buf, size_t offset, size_t size)
{
	ASSERT(b);
	ASSERT(buf);
	ASSERT(idx < b->blk_cnt);
	ASSERT(offset + size <= b->blk_size);

	LOG_INFO("blk_idx %d, offset %d, size %d\n", idx, offset, size);

	if (kblock_buffered(b))
	{
		if (!kblock_loadPage(b, idx))
			return 0;

		kblock_setDirty(b, true);
		return kblock_writeBuf(b, buf, offset, size);
	}
	else
	{
		ASSERT(offset == 0);
		ASSERT(size == b->blk_size);
		return (kblock_writeBlock(b, idx, buf) == 0) ? size : 0;
	}
}

int kblock_copy(struct KBlock *b, block_idx_t src, block_idx_t dest)
{
	ASSERT(b);
	ASSERT(src < b->blk_cnt);
	ASSERT(dest < b->blk_cnt);
	ASSERT(kblock_buffered(b));

	if (!kblock_loadPage(b, src))
		return EOF;

	b->priv.curr_blk = dest;
	kblock_setDirty(b, true);
	return 0;
}

int kblock_swLoad(struct KBlock *b, block_idx_t index)
{
	return (kblock_readDirect(b, index, b->priv.buf, 0, b->blk_size) == b->blk_size) ? 0 : EOF;
}

int kblock_swStore(struct KBlock *b, block_idx_t index)
{
	return kblock_writeBlock(b, index, b->priv.buf);
}

size_t kblock_swReadBuf(struct KBlock *b, void *buf, size_t offset, size_t size)
{
	ASSERT(buf);
	ASSERT(offset + size <= b->blk_size);

	memcpy(buf, (uint8_t *)b->priv.buf + offset, size);
	return size;
}

size_t kblock_swWriteBuf(struct KBlock *b, const void *buf, size_t offset, size_t size)
{
	ASSERT(buf);
	ASSERT(offset + size <= b->blk_size);
	memcpy((uint8_t *)b->priv.buf + offset, buf, size);
	return size;
}

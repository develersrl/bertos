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
 * Copyright 2010 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \author Francesco Sacchi <batt@develer.com>
 *
 * \brief KBlock interface
 */


#include "kblock_ram.h"
#include <string.h>


static int kblockram_load(KBlock *b, block_idx_t index)
{
	KBlockRam *r = KBLOCKRAM_CAST(b);
	memcpy(r->b.priv.pagebuf, r->membuf + index * r->b.blk_size, r->b.blk_size);
	return 0;
}

static int kblockram_store(struct KBlock *b, block_idx_t index)
{
	KBlockRam *r = KBLOCKRAM_CAST(b);
	memcpy(r->membuf + index * r->b.blk_size, r->b.priv.pagebuf, r->b.blk_size);
	return 0;
}

static size_t kblockram_readBuf(struct KBlock *b, void *buf, size_t offset, size_t size)
{
	KBlockRam *r = KBLOCKRAM_CAST(b);
	memcpy(buf, (uint8_t *)r->b.priv.pagebuf + offset, size);
	return size;
}

static size_t kblockram_writeBuf(struct KBlock *b, const void *buf, size_t offset, size_t size)
{
	KBlockRam *r = KBLOCKRAM_CAST(b);
	memcpy((uint8_t *)r->b.priv.pagebuf + offset, buf, size);
	return size;
}

static void * kblockram_map(struct KBlock *b, size_t offset, UNUSED_ARG(size_t, size))
{
	return (uint8_t *)b->priv.pagebuf + offset;
}


static int kblockram_unmap(UNUSED_ARG(struct KBlock *, b), UNUSED_ARG(size_t, offset), UNUSED_ARG(size_t, size))
{
	return 0;
}

static int kblockram_error(struct KBlock *b)
{
	return b->priv.flags;
}

static int kblockram_dummy(UNUSED_ARG(struct KBlock *,b))
{
	return 0;
}

static KBlockVTable kblockram_vt =
{
	.readBuf = kblockram_readBuf,
	.writeBuf = kblockram_writeBuf,
	.load = kblockram_load,
	.store = kblockram_store,
	.map = kblockram_map,
	.unmap = kblockram_unmap,
	.error = kblockram_error,
	.clearerr = kblockram_dummy,
	.close = kblockram_dummy,
};

void kblockram_init(KBlockRam *ram, void *buf, size_t size, size_t block_size)
{
	ASSERT(buf);
	ASSERT(size);
	ASSERT(block_size);

	memset(ram, 0, sizeof(*ram));
	
	DB(ram->b.priv.type = KBT_KBLOCKRAM);
	
	// First page used as page buffer
	ram->b.blk_cnt = (size / block_size) - 1;
	ram->b.priv.pagebuf = buf;
	ram->b.priv.pagebuf_size = block_size;
	
	ram->membuf = (uint8_t *)buf + block_size;
	ram->b.blk_size = block_size;	
	ram->b.vt = &kblockram_vt;
}

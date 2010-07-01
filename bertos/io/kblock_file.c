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
 * \brief KBlock interface over libc files.
 *
 * notest: avr
 * notest: arm
 */


#include "kblock_file.h"
#include <string.h>
#include <stdio.h>


static int kblockfile_load(KBlock *b, block_idx_t index)
{
	KBlockFile *f = KBLOCKFILE_CAST(b);
	fseek(f->fp, index * b->blk_size, SEEK_SET);
	return (fread(f->pagebuf, 1, b->blk_size, f->fp) == b->blk_size) ? 0 : EOF;
}

static int kblockfile_store(struct KBlock *b, block_idx_t index)
{
	KBlockFile *f = KBLOCKFILE_CAST(b);
	fseek(f->fp, index * b->blk_size, SEEK_SET);
	return (fwrite(f->pagebuf, 1, b->blk_size, f->fp) == b->blk_size) ? 0 : EOF;
}

static size_t kblockfile_readBuf(struct KBlock *b, void *buf, size_t offset, size_t size)
{
	KBlockFile *f = KBLOCKFILE_CAST(b);
	memcpy(buf, f->pagebuf + offset, size);
	return size;
}

static size_t kblockfile_readDirect(struct KBlock *b, block_idx_t index, void *buf, size_t offset, size_t size)
{
	KBlockFile *f = KBLOCKFILE_CAST(b);
	fseek(f->fp, index * b->blk_size + offset, SEEK_SET);
	return fread(buf, 1, size, f->fp);
}

static size_t kblockfile_writeBuf(struct KBlock *b, const void *buf, size_t offset, size_t size)
{
	KBlockFile *f = KBLOCKFILE_CAST(b);
	memcpy(f->pagebuf + offset, buf, size);
	return size;
}

static int kblockfile_error(struct KBlock *b)
{
	KBlockFile *f = KBLOCKFILE_CAST(b);
	return ferror(f->fp);
}


static int kblockfile_claererr(struct KBlock *b)
{
	KBlockFile *f = KBLOCKFILE_CAST(b);
	clearerr(f->fp);
	return 0;
}


static int kblockfile_close(struct KBlock *b)
{
	KBlockFile *f = KBLOCKFILE_CAST(b);

	return fflush(f->fp) | fclose(f->fp);
}


static KBlockVTable kblockfile_vt =
{
	.readDirect = kblockfile_readDirect,
	.readBuf = kblockfile_readBuf,
	.writeBuf = kblockfile_writeBuf,
	.load = kblockfile_load,
	.store = kblockfile_store,

	.error = kblockfile_error,
	.clearerr = kblockfile_claererr,
	.close = kblockfile_close,
};


void kblockfile_init(KBlockFile *f, FILE *fp, void *buf, size_t block_size, block_idx_t block_count)
{
	ASSERT(f);
	ASSERT(fp);
	ASSERT(buf);
	ASSERT(block_size);

	memset(f, 0, sizeof(*f));

	DB(f->b.priv.type = KBT_KBLOCKFILE);

	f->fp = fp;
	f->pagebuf = buf;
	f->b.blk_size = block_size;
	f->b.blk_cnt = block_count;
	f->b.priv.vt = &kblockfile_vt;
	kblockfile_load(&f->b, 0);
	f->b.priv.curr_blk = 0;
	f->b.priv.cache_dirty = false;
}

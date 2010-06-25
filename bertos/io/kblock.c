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


static void *kblock_swMap(struct KBlock *b, size_t offset, size_t size)
{
	return (kblock_readBuf(b, b->priv.pagebuf, offset, size) == size) ? b->priv.pagebuf : NULL;
}


static int kblock_swUnmap(struct KBlock *b, size_t offset, size_t size)
{
	return (kblock_writeBuf(b, b->priv.pagebuf, offset, size) == size) ? 0 : EOF;
}


void *kblock_unsupportedMap(struct KBlock *b, UNUSED_ARG(size_t, offset), UNUSED_ARG(size_t, size))
{
	LOG_WARN("This driver does not support block mapping: use kblock_addMapping() to add generic mapping functionality.\n");
	b->priv.flags |= BV(KBS_ERR_MAP_NOT_AVAILABLE);
	return NULL;
}

void kblock_addMapping(struct KBlock *dev, void *buf, size_t size)
{
	ASSERT(buf);
	ASSERT(size);
	ASSERT(dev);
	
	dev->vt->map = kblock_swMap;
	dev->vt->unmap = kblock_swUnmap;

	dev->priv.pagebuf = buf;
	dev->priv.pagebuf_size = size;
}

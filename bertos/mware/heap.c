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
 * Copyright 2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 1999,2000,2001 Bernie Innocenti <bernie@codewiz.org>
 *
 * -->
 *
 * \brief Heap subsystem (public interface).
 *
 * \version $Id$
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 */

#include "heap.h"

#include <cfg/macros.h>           // IS_POW2()
#include <cfg/debug.h>            // ASSERT()

#include <string.h>           // memset()

/* NOTE: struct size must be a 2's power! */
typedef struct _MemChunk
{
	struct _MemChunk *next;
	size_t size;
} MemChunk;

STATIC_ASSERT(IS_POW2(sizeof(MemChunk)));

#define FREE_FILL_CODE     0xDEAD
#define ALLOC_FILL_CODE    0xBEEF

void heap_init(struct Heap* h, void* memory, size_t size)
{
#ifdef _DEBUG
	memset(memory, FREE_FILL_CODE, size);
#endif

	/* Initialize heap with a single big chunk */
	h->FreeList = (MemChunk *)memory;
	h->FreeList->next = NULL;
	h->FreeList->size = size;
}


void *heap_allocmem(struct Heap* h, size_t size)
{
	MemChunk *chunk, *prev;

	/* Round size up to the allocation granularity */
	size = ROUND2(size, sizeof(MemChunk));

	/* Handle allocations of 0 bytes */
	if (!size)
		size = sizeof(MemChunk);

	/* Walk on the free list looking for any chunk big enough to
	 * fit the requested block size.
	 */
	for (prev = (MemChunk *)&h->FreeList, chunk = h->FreeList;
		chunk;
		prev = chunk, chunk = chunk->next)
	{
		if (chunk->size >= size)
		{
			if (chunk->size == size)
			{
				/* Just remove this chunk from the free list */
				prev->next = chunk->next;
				#ifdef _DEBUG
					memset(chunk, ALLOC_FILL_CODE, size);
				#endif
				return (void *)chunk;
			}
			else
			{
				/* Allocate from the END of an existing chunk */
				chunk->size -= size;
				#ifdef _DEBUG
					memset((uint8_t *)chunk + chunk->size, ALLOC_FILL_CODE, size);
				#endif
				return (void *)((uint8_t *)chunk + chunk->size);
			}
		}
	}

	return NULL; /* fail */
}


void heap_freemem(struct Heap* h, void *mem, size_t size)
{
	MemChunk *prev;
	ASSERT(mem);

#ifdef _DEBUG
	memset(mem, FREE_FILL_CODE, size);
#endif

	/* Round size up to the allocation granularity */
	size = ROUND2(size, sizeof(MemChunk));

	/* Handle allocations of 0 bytes */
	if (!size)
		size = sizeof(MemChunk);

	/* Special case: first chunk in the free list */
	ASSERT((uint8_t*)mem != (uint8_t*)h->FreeList);
	if (((uint8_t *)mem) < ((uint8_t *)h->FreeList))
	{
		/* Insert memory block before the current free list head */
		prev = (MemChunk *)mem;
		prev->next = h->FreeList;
		prev->size = size;
		h->FreeList = prev;
	}
	else /* Normal case: not the first chunk in the free list */
	{
		/*
		 * Walk on the free list. Stop at the insertion point (when mem
		 * is between prev and prev->next)
		 */
		prev = h->FreeList;
		while (prev->next < (MemChunk *)mem && prev->next)
			prev = prev->next;

		/* Make sure mem is not *within* prev */
		ASSERT((uint8_t*)mem >= (uint8_t*)prev + prev->size);

		/* Should it be merged with previous block? */
		if (((uint8_t *)prev) + prev->size == ((uint8_t *)mem))
		{
			/* Yes */
			prev->size += size;
		}
		else /* not merged with previous chunk */
		{
			MemChunk *curr = (MemChunk*)mem;

			/* insert it after the previous node
			 * and move the 'prev' pointer forward
			 * for the following operations
			 */
			curr->next = prev->next;
			curr->size = size;
			prev->next = curr;

			/* Adjust for the following test */
			prev = curr;
		}
	}

	/* Also merge with next chunk? */
	if (((uint8_t *)prev) + prev->size == ((uint8_t *)prev->next))
	{
		prev->size += prev->next->size;
		prev->next = prev->next->next;

		/* There should be only one merge opportunity, becuase we always merge on free */
		ASSERT((uint8_t*)prev + prev->size != (uint8_t*)prev->next);
	}
}

#if CONFIG_HEAP_MALLOC

void *heap_malloc(struct Heap* h, size_t size)
{
	size_t *mem;

	size += sizeof(size_t);
	if ((mem = (size_t*)heap_allocmem(h, size)))
		*mem++ = size;

	return mem;
}

void *heap_calloc(struct Heap* h, size_t size)
{
	void *mem;

	if ((mem = heap_malloc(h, size)))
		memset(mem, 0, size);

	return mem;
}

/**
 * Free a block of memory, determining its size automatically.
 *
 * \param h    Heap from which the block was allocated.
 * \param mem  Pointer to a block of memory previously allocated with
 *             either heap_malloc() or heap_calloc().
 *
 * \note If \a mem is a NULL pointer, no operation is performed.
 *
 * \note Freeing the same memory block twice has undefined behavior.
 *
 * \note This function works like the ANSI C free().
 */
void heap_free(struct Heap *h, void *mem)
{
	size_t *_mem = (size_t *)mem;

	if (_mem)
	{
		--_mem;
		heap_freemem(h, _mem, *_mem);
	}
}

#endif /* CONFIG_HEAP_MALLOC */

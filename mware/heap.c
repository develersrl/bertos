/*!
 * \file
 * <!--
 * Copyright 2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 1999,2000,2001 Bernardo Innocenti <bernie@develer.com>
 * This file is part of DevLib - See devlib/README for information.
 * -->
 *
 * \brief Heap subsystem (public interface).
 *
 * \version $Id$
 *
 * \author Bernardo Innocenti <bernie@develer.com>
 */

/*
 * $Log$
 * Revision 1.3  2004/08/14 19:37:57  rasky
 * Merge da SC: macros.h, pool.h, BIT_CHANGE, nome dei processi, etc.
 *
 * Revision 1.2  2004/08/04 15:54:18  rasky
 * Merge da SC: prima versione veramente funzionante
 *
 * Revision 1.1  2004/07/31 16:33:58  rasky
 * Spostato lo heap da kern/ a mware/
 *
 * Revision 1.2  2004/06/03 11:27:09  bernie
 * Add dual-license information.
 *
 * Revision 1.1  2004/05/23 17:27:00  bernie
 * Import kern/ subdirectory.
 *
 */

#include "heap.h"
#include <string.h>           // memset()
#include <macros.h>           // IS_POW2()
#include <drv/kdebug.h>       // ASSERT()

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

void heap_free(struct Heap* h, void *mem_)
{
	size_t* mem = (size_t*)mem_;
	--mem;
	heap_freemem(h, mem, *mem);
}

#endif /* CONFIG_HEAP_MALLOC */

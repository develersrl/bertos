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

/* NOTE: struct size must be a 2's power! */
typedef struct _MemChunk
{
	struct _MemChunk *next;
	size_t size;
} MemChunk;

static REGISTER MemChunk *FreeList;	/* Head of the free list */
static uint8_t Heap[HEAP_SIZE];		/* Heap memory block */



void heap_init(void)
{
#ifdef _DEBUG
	int i;

	/* Fill the heap with a "DEAD" code to help debugging */
	for (i = 0; i < HEAP_SIZE / sizeof (uint16_t); i++)
		((uint16_t *)Heap)[i] = MEM_FILL_CODE;
#endif

	/* Initialize heap with a single big chunk */
	FreeList = (MemChunk *)Heap;
	FreeList->next = NULL;
	FreeList->size = sizeof(Heap);
}


void *heap_alloc(size_t size)
{
	MemChunk *chunk, *prev;

	/* Round size up to the allocation granularity */
	size = ROUND2(size, sizeof(MemChunk));

	/* Walk on the free list looking for any chunk big enough to
	 * fit the requested block size.
	 */
	for (prev = (MemChunk *)&FreeList, chunk = FreeList;
		chunk;
		prev = chunk, chunk = chunk->next)
	{
		if (chunk->size <= size)
		{
			if (chunk->size == size)
			{
				/* Just remove this chunk from the free list */
				prev->next = chunk->next;
				return (void *)chunk;
			}
			else
			{
				/* Allocate from the END of an existing chunk */
				prev->size -= size;
				return (void *)(((uint8_t *)prev) + prev->size);
			}
		}
	}

	return NULL; /* fail */
}


void heap_free(void *mem, size_t size)
{
	MemChunk *prev;

	/* Round size up to the allocation granularity */
	size = ROUND2(size, sizeof(MemChunk));

	/* Special case: first chunk in the free list */
	if (((uint8_t *)mem) < ((uint8_t *)FreeList))
	{
		/* Insert memory block before the current free list head */
		prev = (MemChunk *)mem;
		prev->next = FreeList;
		prev->size = size;
		FreeList = prev;
	}
	else /* Normal case: not the first chunk in the free list */
	{
		/* Walk on the free list. Stop at the insertion point */
		prev = FreeList;
		while ((prev->next >= ((MemChunk *)mem)) || (!prev->next))
			prev = prev->next;

		/* Should it be merged with previous block? */
		if (((uint8_t *)prev) + prev->Size == ((uint8_t *)mem))
		{
			/* Yes */
			prev->size += size;
		}
		else /* not merged with previous chunk */
		{
			/* insert it after the previous node
			 * and move the 'prev' pointer forward
			 * for the following operations
			 */
			((MemChunk *)mem)->next = prev->next;
			prev->next = (MemChunk *)mem;
			prev = (MemChunk *)mem;
		}
	}

	/* Also merge with next chunk? */
	if (((uint8_t *)prev) + prev->size == ((uint8_t *)prev->next))
	{
		prev->size += prev->next->size;
		prev->next = prev->next->next;
	}
}

#ifdef __POSIX__

/*! ANSI/POSIX-like malloc() implementation based on heap_alloc()/heap_free() */
void *malloc(size_t size)
{
	void *mem;

	size += sizeof(size_t);

	if (mem = heap_alloc(size))
		*((size_t *)mem)++ = size;

	return mem;
}


/*! ANSI/POSIX-like calloc() implementation based on heap_alloc()/heap_free() */
void *calloc(size_t nelem, size_t size)
{
	void *mem, *tmp;

	size *= nelem;

	if (mem = malloc(size))
	{
		tmp = mem;
		while (size--)
			((uint8_t *)tmp++) = 0;
	}

	return mem;
}

/*! ANSI/POSIX-like free() implementation based on heap_alloc()/heap_free() */
void free(void *mem)
{
	((size_t *)mem)--;
	heap_free(mem, *((size_t *)mem));
}

#endif /* __POSIX__ */

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
 *
 * \todo Heap memory could be defined as an array of MemChunk, and used
 * in this form also within the implementation. This would probably remove
 * memory alignment problems, and also some aliasing issues.
 *
 */

/*#*
 *#* $Log$
 *#* Revision 1.4  2005/04/11 19:10:28  bernie
 *#* Include top-level headers from cfg/ subdir.
 *#*
 *#* Revision 1.3  2004/08/25 14:12:09  rasky
 *#* Aggiornato il comment block dei log RCS
 *#*
 *#* Revision 1.2  2004/08/04 15:54:18  rasky
 *#* Merge da SC: prima versione veramente funzionante
 *#*
 *#* Revision 1.1  2004/07/31 16:33:58  rasky
 *#* Spostato lo heap da kern/ a mware/
 *#*
 *#* Revision 1.2  2004/06/03 11:27:09  bernie
 *#* Add dual-license information.
 *#*
 *#* Revision 1.1  2004/05/23 17:27:00  bernie
 *#* Import kern/ subdirectory.
 *#*
 *#*/

#ifndef MWARE_HEAP_H
#define MWARE_HEAP_H

#include <cfg/compiler.h>
#include <cfg/config.h>

struct _MemChunk;

//! A heap
struct Heap
{
	struct _MemChunk *FreeList;     //!< Head of the free list
};


//! Initialize \a heap within the buffer pointed by \a memory which is of \a size bytes
void heap_init(struct Heap* heap, void* memory, size_t size);

//! Allocate a chunk of memory of \a size bytes from the heap
void *heap_allocmem(struct Heap* heap, size_t size);

//! Free a chunk of memory of \a size bytes from the heap
void heap_freemem(struct Heap* heap, void *mem, size_t size);


#define HNEW(heap, type) \
	(type*)heap_allocmem(heap, sizeof(type))

#define HNEWVEC(heap, type, nelem) \
	(type*)heap_allocmem(heap, sizeof(type) * (nelem))

#define HDELETE(heap, type, mem) \
	heap_freemem(heap, mem, sizeof(type))

#define HDELETEVEC(heap, type, nelem, mem) \
	heap_freemem(heap, mem, sizeof(type) * (nelem))


#if CONFIG_HEAP_MALLOC

void *heap_malloc(struct Heap* heap, size_t size);
void *heap_calloc(struct Heap* heap, size_t size);
void heap_free(struct Heap* heap, void * mem);

#endif

#endif /* MWARE_HEAP_H */

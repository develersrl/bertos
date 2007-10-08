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
 * Copyright 1999,2000,2001 Bernardo Innocenti <bernie@develer.com>
 * This file is part of DevLib - See README.devlib for information.
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
 *#* Revision 1.7  2006/07/19 12:56:27  bernie
 *#* Convert to new Doxygen style.
 *#*
 *#* Revision 1.6  2005/11/27 23:36:19  bernie
 *#* Use appconfig.h instead of cfg/config.h.
 *#*
 *#* Revision 1.5  2005/11/04 16:20:02  bernie
 *#* Fix reference to README.devlib in header.
 *#*
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
#include <appconfig.h>

struct _MemChunk;

/// A heap
struct Heap
{
	struct _MemChunk *FreeList;     ///< Head of the free list
};


/// Initialize \a heap within the buffer pointed by \a memory which is of \a size bytes
void heap_init(struct Heap* heap, void* memory, size_t size);

/// Allocate a chunk of memory of \a size bytes from the heap
void *heap_allocmem(struct Heap* heap, size_t size);

/// Free a chunk of memory of \a size bytes from the heap
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

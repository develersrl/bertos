/*!
 * \file
 * <!--
 * Copyright 2003, 2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2001 Bernardo Innocenti <bernie@develer.com>
 * This file is part of DevLib - See devlib/README for information.
 * -->
 *
 * \version $Id$
 *
 * \author Bernardo Innocenti <bernie@develer.com>
 *
 * \brief General pourpose FIFO buffer implemented with a ring buffer
 *
 * \li \c begin points to the first buffer element;
 * \li \c end points to the last buffer element (unlike the STL convention);
 * \li \c head points to the element to be extracted next;
 * \li \c tail points to the location following the last insertion;
 * \li when any of the pointers advances beyond \c end, it is reset
 *     back to \c begin.
 *
 * \code
 *
 *  +-----------------------------------+
 *  |  empty  |   valid data   |  empty |
 *  +-----------------------------------+
 *  ^         ^                ^        ^
 *  begin    head             tail     end
 *
 * \endcode
 *
 * The buffer is EMPTY when \c head and \c tail point to the same location:
 *		\code head == tail \endcode
 *
 * The buffer is FULL when \c tail points to the location immediately
 * after \c head:
 *		\code tail == head - 1 \endcode
 *
 * The buffer is also FULL when \c tail points to the last buffer
 * location and head points to the first one:
 *		\code head == begin && tail == end \endcode
 */

/*#*
 *#* $Log$
 *#* Revision 1.15  2004/08/29 22:05:16  bernie
 *#* Rename BITS_PER_PTR to CPU_BITS_PER_PTR.
 *#*
 *#* Revision 1.14  2004/08/25 14:12:09  rasky
 *#* Aggiornato il comment block dei log RCS
 *#*
 *#* Revision 1.13  2004/08/24 13:16:11  bernie
 *#* Add type-size definitions for preprocessor.
 *#*
 *#* Revision 1.12  2004/08/02 20:20:29  aleph
 *#* Merge from project_ks
 *#*
 *#* Revision 1.11  2004/07/30 14:15:53  rasky
 *#* Nuovo supporto unificato per detect della CPU
 *#*
 *#* Revision 1.10  2004/07/29 22:57:09  bernie
 *#* Doxygen fix.
 *#*
 *#* Revision 1.9  2004/07/20 23:54:27  bernie
 *#* fifo_flush_locked(): New function;
 *#* Revamp documentation.
 *#*
 *#* Revision 1.8  2004/07/20 23:47:39  bernie
 *#* Finally remove redundant protos.
 *#*
 *#* Revision 1.7  2004/07/20 23:46:29  bernie
 *#* Finally remove redundant protos.
 *#*
 *#* Revision 1.6  2004/06/06 17:18:04  bernie
 *#* Remove redundant declaration of fifo_isempty_locked().
 *#*
 *#* Revision 1.5  2004/06/06 16:50:35  bernie
 *#* Import fixes for race conditions from project_ks.
 *#*
 *#* Revision 1.4  2004/06/06 16:11:17  bernie
 *#* Protect MetroWerks specific pragmas with #ifdef's
 *#*/

#ifndef MWARE_FIFO_H
#define MWARE_FIFO_H

#include "cpu.h"

typedef struct FIFOBuffer
{
	unsigned char * volatile head;
	unsigned char * volatile tail;
	unsigned char *begin;
	unsigned char *end;
} FIFOBuffer;


/*!
 * Check whether the fifo is empty
 *
 * \note Calling fifo_isempty() is safe while a concurrent
 *       execution context is calling fifo_push() or fifo_pop()
 *       only if the CPU can atomically update a pointer
 *       (which the AVR and other 8-bit processors can't do).
 *
 * \sa fifo_isempty_locked
 */
INLINE bool fifo_isempty(const FIFOBuffer *fb)
{
	return fb->head == fb->tail;
}


/*!
 * Check whether the fifo is full
 *
 * \note Calling fifo_isfull() is safe while a concurrent
 *       execution context is calling fifo_pop() and the
 *       CPU can update a pointer atomically.
 *       It is NOT safe when the other context calls
 *       fifo_push().
 *       This limitation is not usually problematic in a
 *       consumer/producer scenario because the
 *       fifo_isfull() and fifo_push() are usually called
 *       in the producer context.
 */
INLINE bool fifo_isfull(const FIFOBuffer *fb)
{
	return
		((fb->head == fb->begin) && (fb->tail == fb->end))
		|| (fb->tail == fb->head - 1);
}


/*!
 * Pop a character from the fifo buffer.
 *
 * \note Calling \c fifo_push() on a full buffer is undefined.
 *       The caller must make sure the buffer has at least
 *       one free slot before calling this function.
 *
 * \note It is safe to call fifo_pop() and fifo_push() from
 *       concurrent contexts, unless the CPU can't update
 *       a pointer atomically (which the AVR and other 8-bit
 *       processors can't do).
 *
 * \sa fifo_push_locked
 */
INLINE void fifo_push(FIFOBuffer *fb, unsigned char c)
{
#ifdef __MWERKS__
#pragma interrupt called
#endif
	/* Write at tail position */
	*(fb->tail) = c;

	if (fb->tail == fb->end)
		/* wrap tail around */
		fb->tail = fb->begin;
	else
		/* Move tail forward */
		fb->tail++;
}


/*!
 * Pop a character from the fifo buffer.
 *
 * \note Calling \c fifo_pop() on an empty buffer is undefined.
 *       The caller must make sure the buffer contains at least
 *       one character before calling this function.
 *
 * \note It is safe to call fifo_pop() and fifo_push() from
 *       concurrent contexts.
 */
INLINE unsigned char fifo_pop(FIFOBuffer *fb)
{
#ifdef __MWERKS__
#pragma interrupt called
#endif
	if (fb->head == fb->end)
	{
		/* wrap head around */
		fb->head = fb->begin;
		return *(fb->end);
	}
	else
		/* move head forward */
		return *(fb->head++);
}


/*!
 * Make the fifo empty, discarding all its current contents.
 */
INLINE void fifo_flush(FIFOBuffer *fb)
{
	fb->head = fb->tail;
}


#if CPU_REG_BITS >= CPU_BITS_PER_PTR

	/*
	 * 16/32bit CPUs that can update a pointer with a single write
	 * operation, no need to disable interrupts.
	 */
	#define fifo_isempty_locked(fb) fifo_isempty((fb))
	#define fifo_push_locked(fb, c) fifo_push((fb), (c))
	#define fifo_flush_locked(fb) fifo_flush((fb))

#else /* CPU_REG_BITS < CPU_BITS_PER_PTR */

	/*!
	 * Similar to fifo_isempty(), but with stronger guarantees for
	 * concurrent access between user and interrupt code.
	 *
	 * \note This is actually only needed for 8-bit processors.
	 *
	 * \sa fifo_isempty()
	 */
	INLINE bool fifo_isempty_locked(const FIFOBuffer *fb)
	{
		bool result;
		cpuflags_t flags;

		DISABLE_IRQSAVE(flags);
		result = fifo_isempty(fb);
		ENABLE_IRQRESTORE(flags);

		return result;
	}


	/*!
	 * Similar to fifo_push(), but with stronger guarantees for
	 * concurrent access between user and interrupt code.
	 *
	 * \note This is actually only needed for 8-bit processors.
	 *
	 * \sa fifo_push()
	 */
	INLINE void fifo_push_locked(FIFOBuffer *fb, unsigned char c)
	{
		cpuflags_t flags;
		DISABLE_IRQSAVE(flags);
		fifo_push(fb, c);
		ENABLE_IRQRESTORE(flags);
	}


	/*!
	 * Similar to fifo_flush(), but with stronger guarantees for
	 * concurrent access between user and interrupt code.
	 *
	 * \note This is actually only needed for 8-bit processors.
	 *
	 * \sa fifo_flush()
	 */
	INLINE void fifo_flush_locked(FIFOBuffer *fb)
	{
		cpuflags_t flags;
		DISABLE_IRQSAVE(flags);
		fifo_flush(fb);
		ENABLE_IRQRESTORE(flags);
	}

#endif /* CPU_REG_BITS < BITS_PER_PTR */


/*!
 * Thread safe version of fifo_isfull()
 */
INLINE bool fifo_isfull_locked(const FIFOBuffer *_fb)
{
	bool _result;
	cpuflags_t _flags;

	DISABLE_IRQSAVE(_flags);
	_result = fifo_isfull(_fb);
	ENABLE_IRQRESTORE(_flags);

	return _result;
}


/*!
 * FIFO Initialization.
 */
INLINE void fifo_init(FIFOBuffer *fb, unsigned char *buf, size_t size)
{
	fb->head = fb->tail = fb->begin = buf;
	fb->end = buf + size - 1;
}


#if 0

/*
 * UNTESTED: if uncommented, to be moved in fifobuf.c
 */
void fifo_pushblock(FIFOBuffer *fb, unsigned char *block, size_t len)
{
	size_t freelen;

	/* Se c'e' spazio da tail alla fine del buffer */
	if (fb->tail >= fb->head)
	{
		freelen = fb->end - fb->tail + 1;

		/* C'e' abbastanza spazio per scrivere tutto il blocco? */
		if (freelen < len)
		{
			/* Scrivi quello che entra fino alla fine del buffer */
			memcpy(fb->tail, block, freelen);
			block += freelen;
			len -= freelen;
			fb->tail = fb->begin;
		}
		else
		{
			/* Scrivi tutto il blocco */
			memcpy(fb->tail, block, len);
			fb->tail += len;
			return;
		}
	}

	for(;;)
	{
		while (!(freelen = fb->head - fb->tail - 1))
			Delay(FIFO_POLLDELAY);

		/* C'e' abbastanza spazio per scrivere tutto il blocco? */
		if (freelen < len)
		{
			/* Scrivi quello che entra fino alla fine del buffer */
			memcpy(fb->tail, block, freelen);
			block += freelen;
			len -= freelen;
			fb->tail += freelen;
		}
		else
		{
			/* Scrivi tutto il blocco */
			memcpy(fb->tail, block, len);
			fb->tail += len;
			return;
		}
	}
}
#endif

#endif /* MWARE_FIFO_H */


/*!
 * \file
 * <!--
 * Copyright 2003,2004 Develer S.r.l. (http://www.develer.com/)
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
 * \li \c begin punta al primo elemento del buffer,
 * \li \c end punta all'ultimo elemento,
 * \li \c head punta al prossimo elemento che verra' estratto,
 * \li \c tail punta alla posizione successiva all'ultimo elemento inserito.
 * \li quando uno dei due puntatori raggiunge @c end, viene resettato a @c begin.
 *
 * \code
 *
 *  +-----------------------------------+
 *  |  vuoto  |   dati validi  |  vuoto |
 *  +-----------------------------------+
 *  ^         ^                ^        ^
 *  begin    head             tail     end
 *
 * \endcode
 *
 * Il buffer e' VUOTO quando head e tail coincidono:
 *		\code head == tail \endcode
 *
 * Il buffer e' PIENO quando tail si trova immediatamente dietro a head:
 *		\code tail == head - 1 \endcode
 *
 * Il buffer e' PIENO anche quando tail e' posizionato
 * sull'ultima posizione del buffer e head sulla prima:
 *		\code head == begin && tail == end \endcode
 */

/*
 * $Log$
 * Revision 1.4  2004/06/06 16:11:17  bernie
 * Protect MetroWerks specific pragmas with #ifdef's
 *
 * Revision 1.3  2004/06/03 15:04:10  aleph
 * Merge improvements from project_ks (mainly inlining)
 *
 * Revision 1.2  2004/06/03 11:27:09  bernie
 * Add dual-license information.
 *
 * Revision 1.1  2004/05/23 15:43:16  bernie
 * Import mware modules.
 *
 * Revision 1.3  2004/05/22 17:55:58  rasky
 * \samp non esiste in doxygen
 *
 * Revision 1.2  2004/04/27 11:13:29  rasky
 * Spostate tutte le definizioni CPU-specific da compiler.h nel nuovo file cpu.h
 *
 * Revision 1.1  2004/04/21 17:38:25  rasky
 * New application
 *
 * Revision 1.4  2004/03/24 15:37:03  bernie
 * Remove Copyright messages from Doxygen output
 *
 * Revision 1.3  2004/03/18 18:11:07  bernie
 * Add thread-safe FIFO handling macros
 *
 * Revision 1.2  2004/03/01 08:00:36  bernie
 * Fixes for Doxygen
 *
 * Revision 1.1  2003/12/07 04:04:20  bernie
 * Initial project_ks framework.
 *
 * Revision 1.1  2003/11/21 16:36:17  aleph
 * Rename from fifo to fifobuf to avoid conflict with BSP fifo.h header
 *
 * Revision 1.1  2003/11/20 22:17:41  aleph
 * Add fifo buffer used by serial
 *
 */

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


/* Public function prototypes */
INLINE bool fifo_isempty(const FIFOBuffer *fb);
INLINE bool fifo_isempty_locked(const FIFOBuffer *fb);
INLINE bool fifo_isfull(const FIFOBuffer *fb);
INLINE bool fifo_isfull_locked(const FIFOBuffer *fb);
#ifdef __MWERKS__
#pragma interrupt called
#endif
INLINE void fifo_push(FIFOBuffer *fb, unsigned char c);
#ifdef __MWERKS__
#pragma interrupt called
#endif
INLINE unsigned char fifo_pop(FIFOBuffer *fb);
INLINE void fifo_flush(FIFOBuffer *fb);
INLINE void fifo_init(FIFOBuffer *fb, unsigned char *buf, size_t size);


/*!
 * Check whether the fifo is empty
 *
 * \note Calling fifo_isempty() is safe while a concurrent
 *       execution context is calling fifo_push() or fifo_pop()
 *       only if the CPU can atomically update a pointer.
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


#if !defined(__AVR__)

	/* No tricks needed on 16/32bit CPUs */
#	define fifo_isempty_locked(fb) fifo_isempty((fb))

#else /* !__AVR__ */

	INLINE bool fifo_isempty_locked(const FIFOBuffer *fb)
	{
		bool result;
		cpuflags_t flags;

		DISABLE_IRQSAVE(flags);
		result = fifo_isempty(fb);
		ENABLE_IRQRESTORE(flags);

		return result;
	}

#endif /* !__AVR__ */


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
 * Pop a character from the fifo buffer.
 *
 * \note Calling \c fifo_push() on a full buffer is undefined.
 *       The caller must make sure the buffer has at least
 *       one free slot before calling this function.
 *
 * \note It is safe to call fifo_pop() and fifo_push() from
 *       concurrent contexts.
 */
INLINE void fifo_push(FIFOBuffer *fb, unsigned char c)
{
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


/*!
 * \file
 * <!--
 * Copyright (C) 2003,2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright (C) 2001 Bernardo Innocenti <bernie@develer.com>
 * All Rights Reserved.
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
 * <pre>
 *
 *  +-----------------------------------+
 *  |  vuoto  |   dati validi  |  vuoto |
 *  +-----------------------------------+
 *  ^         ^                ^        ^
 *  begin    head             tail     end
 *
 * </pre>
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
#ifndef COMPILER_H
#include "compiler.h"
#endif

typedef struct FIFOBuffer
{
	unsigned char *head;
	unsigned char *tail;
	unsigned char *begin;
	unsigned char *end;
} FIFOBuffer;

/* Public function prototypes */
void fifo_init(volatile FIFOBuffer *fb, unsigned char *buf, size_t size);

#pragma interrupt called
void fifo_push(volatile FIFOBuffer *fb, unsigned char c);

#pragma interrupt called
unsigned char fifo_pop(volatile FIFOBuffer *fb);


/*!
 * Check whether the fifo is empty
 *
 * \note Calling fifo_isempty() is safe while a concurrent
 *       execution context is calling fifo_push() or fifo_pop()
 *       only if the CPU can atomically update a pointer.
 */
#define fifo_isempty(fb)	((fb)->head == (fb)->tail)

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
#define fifo_isfull(fb) \
	((((fb)->head == (fb)->begin) && ((fb)->tail == (fb)->end)) \
	|| ((fb)->tail == (fb)->head - 1))

/*!
 * Make the fifo empty, discarding all its current contents.
 */
#define fifo_flush(fb)	((fb)->head = (fb)->tail)

#if !defined(__AVR__)

	/* No tricks needed on 16/32bit CPUs */
#	define fifo_isempty_locked(fb) fifo_isempty((fb))

#else /* !__AVR__ */

	INLINE bool fifo_isempty_locked(const volatile FIFOBuffer *_fb);
	INLINE bool fifo_isempty_locked(const volatile FIFOBuffer *_fb)
	{
		bool _result;
		cpuflags_t _flags;

		DISABLE_IRQSAVE(_flags);
		_result = fifo_isempty(_fb);
		ENABLE_IRQRESTORE(_flags);

		return _result;
	}

#endif /* !__AVR__ */

/*!
 * Thread safe version of fifo_isfull()
 */
INLINE bool fifo_isfull_locked(const volatile FIFOBuffer *_fb);
INLINE bool fifo_isfull_locked(const volatile FIFOBuffer *_fb)
{
	bool _result;
	cpuflags_t _flags;

	DISABLE_IRQSAVE(_flags);
	_result = fifo_isfull(_fb);
	ENABLE_IRQRESTORE(_flags);

	return _result;
}

#endif /* MWARE_FIFOBUF_H */


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
 * \brief FIFO buffer handling routines
 */

/*
 * $Log$
 * Revision 1.1  2004/05/23 15:43:16  bernie
 * Import mware modules.
 *
 */

#include "fifobuf.h"
#include "compiler.h"
#include <drv/kdebug.h>

void fifo_init(volatile FIFOBuffer *fb, unsigned char *buf, size_t size)
{
	fb->head = fb->tail = fb->begin = buf;
	fb->end = buf + size - 1;
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
void fifo_push(volatile FIFOBuffer *fb, unsigned char c)
{
#pragma interrupt called
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
unsigned char fifo_pop(volatile FIFOBuffer *fb)
{
#pragma interrupt called
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


#if 0

/* untested */
void fifo_pushblock(volatile FIFOBuffer *fb, unsigned char *block, size_t len)
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

#endif /* UNUSED */

/*!
 * \file
 * <!--
 * Copyright 2001,2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 1999,2000,2001 Bernardo Innocenti <bernie@develer.com>
 * All Rights Reserved.
 * -->
 *
 * \brief Semaphore based synchronization services.
 *
 * \version $Id$
 *
 * \author Bernardo Innocenti <bernie@develer.com>
 */

/*
 * $Log$
 * Revision 1.1  2004/05/23 17:27:00  bernie
 * Import kern/ subdirectory.
 *
 */

#include "sem.h"
#include "proc.h"
#include "proc_p.h"
#include "signal.h"
#include "hw.h"


/*!
 * \brief Initialize a Semaphore structure
 */
void sem_init(struct Semaphore *s)
{
	INITLIST(&s->wait_queue);
	s->owner = NULL;
	s->nest_count = 0;
}


/*!
 * \brief Attempt to lock a semaphore without waiting.
 *
 * \return true in case of success, false if the semaphore
 *         was already locked by someone else.
 *
 * \note   each call to sem_attempt() must be matched by a
 *         call to sem_release().
 */
bool sem_attempt(struct Semaphore *s)
{
	DISABLE_INTS;
	if ((!s->owner) || (s->owner == CurrentProcess))
	{
		s->owner = CurrentProcess;
		s->nest_count++;
		ENABLE_INTS;
		return true;
	}
	ENABLE_INTS;
	return false;
}


/*!
 * \brief Lock a semaphore.
 *
 * If the semaphore is already owned by another process, the caller
 * process will be enqueued into the waiting list and sleep until
 * the semaphore is available.
 *
 * \note Each call to sem_obtain() must be matched by a
 *       call to sem_release().
 *
 * \note This routine is optimized for highest speed in
 *       the most common case: the semaphore is free or locked
 *       by the calling process itself. Rearranging this code
 *       is probably a bad idea.
 */
void sem_obtain(struct Semaphore *s)
{
	DISABLE_INTS;

	/* Is the semaphore already locked by another process? */
	if (s->owner && (s->owner != CurrentProcess))
	{
		/* Append calling process to the wait queue */
		ADDTAIL(&s->wait_queue, (Node *)CurrentProcess);
		ENABLE_INTS;

		/* We will awake only when the current owner calls
		 * ReleaseSemaphore(). Then, the semaphore will already
		 * be locked for us.
		 */
		proc_schedule();
	}
	else
	{
		/* The semaphore is free: lock it */
		s->owner = CurrentProcess;
		s->nest_count++;
		ENABLE_INTS;
	}
}


/*!
 * \brief Releases a lock on a previously locked semaphore.
 *
 * If the nesting count of the semaphore reaches zero,
 * the next process waiting for it will be awaken.
 *
 * \note This routine is optimized for highest speed in
 *       the most common case: the semaphore has been locked just
 *       once and nobody else was waiting for it. Rearranging
 *       this code is probably a bad idea.
 */
void sem_release(struct Semaphore *s)
{
	DISABLE_INTS;

	/* Decremement nesting count and check if the semaphore
	 * has been fully unlocked
	 */
	if (--s->nest_count == 0)
	{
		/* Disown semaphore */
		s->owner = NULL;

		/* Anybody still waiting for this semaphore? */
		if (!ISLISTEMPTY(&s->wait_queue))
		{
			/* Give semaphore to the first applicant */
			Process *proc = (Process *)s->wait_queue.head;
			REMOVE((Node *)proc);
			s->nest_count = 1;
			s->owner = proc;
			SCHED_ENQUEUE(proc);
		}
	}

	ENABLE_INTS;
}


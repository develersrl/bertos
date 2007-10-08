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
 * Copyright 2001, 2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 1999, 2000, 2001 Bernardo Innocenti <bernie@develer.com>
 *
 * -->
 *
 * \brief Semaphore based synchronization services.
 *
 * \version $Id$
 *
 * \author Bernardo Innocenti <bernie@develer.com>
 */

/*#*
 *#* $Log$
 *#* Revision 1.12  2006/07/19 12:56:27  bernie
 *#* Convert to new Doxygen style.
 *#*
 *#* Revision 1.11  2006/02/24 01:17:05  bernie
 *#* Update for new emulator.
 *#*
 *#* Revision 1.10  2005/11/04 16:20:02  bernie
 *#* Fix reference to README.devlib in header.
 *#*
 *#* Revision 1.9  2005/04/11 19:10:28  bernie
 *#* Include top-level headers from cfg/ subdir.
 *#*
 *#* Revision 1.8  2005/01/22 04:20:42  bernie
 *#* Add integrity checks.
 *#*
 *#* Revision 1.7  2004/11/28 23:20:25  bernie
 *#* Remove obsolete INITLIST macro.
 *#*
 *#* Revision 1.6  2004/10/21 10:57:21  bernie
 *#* Use proc_forbid()/proc_permit().
 *#*
 *#* Revision 1.5  2004/10/21 10:48:57  bernie
 *#* sem_release(): Simplify (made by rasky on scfirm).
 *#*
 *#* Revision 1.4  2004/08/25 14:12:09  rasky
 *#* Aggiornato il comment block dei log RCS
 *#*
 *#* Revision 1.3  2004/08/08 05:53:23  bernie
 *#* Use DISABLE_IRQSAVE/ENABLE_IRQRESTORE; Cleanup documentation.
 *#*
 *#* Revision 1.2  2004/06/03 11:27:09  bernie
 *#* Add dual-license information.
 *#*
 *#* Revision 1.1  2004/05/23 17:27:00  bernie
 *#* Import kern/ subdirectory.
 *#*/

#include "sem.h"
#include <kern/proc.h>
#include <kern/proc_p.h>
#include <kern/signal.h>
#include <cfg/debug.h>

INLINE void sem_verify(struct Semaphore *s)
{
	LIST_ASSERT_VALID(&s->wait_queue);
	ASSERT(s->nest_count >= 0);
	ASSERT(s->nest_count < 128);   // heuristic max
}


/**
 * \brief Initialize a Semaphore structure.
 */
void sem_init(struct Semaphore *s)
{
	LIST_INIT(&s->wait_queue);
	s->owner = NULL;
	s->nest_count = 0;
}


/**
 * \brief Attempt to lock a semaphore without waiting.
 *
 * \return true in case of success, false if the semaphore
 *         was already locked by someone else.
 *
 * \note   each call to sem_attempt() must be matched by a
 *         call to sem_release().
 *
 * \see sem_obtain() sem_release()
 */
bool sem_attempt(struct Semaphore *s)
{
	bool result = false;

	proc_forbid();
	sem_verify(s);
	if ((!s->owner) || (s->owner == CurrentProcess))
	{
		s->owner = CurrentProcess;
		s->nest_count++;
		result = true;
	}
	proc_permit();

	return result;
}


/**
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
 *
 * \sa sem_release() sem_attempt()
 */
void sem_obtain(struct Semaphore *s)
{
	proc_forbid();
	sem_verify(s);

	/* Is the semaphore already locked by another process? */
	if (UNLIKELY(s->owner && (s->owner != CurrentProcess)))
	{
		/* Append calling process to the wait queue */
		ADDTAIL(&s->wait_queue, (Node *)CurrentProcess);

		/*
		 * We will wake up only when the current owner calls
		 * sem_release(). Then, the semaphore will already
		 * be locked for us.
		 */
		proc_permit();
		proc_schedule();
	}
	else
	{
		ASSERT(LIST_EMPTY(&s->wait_queue));

		/* The semaphore was free: lock it */
		s->owner = CurrentProcess;
		s->nest_count++;
		proc_permit();
	}
}


/**
 * \brief Release a lock on a previously locked semaphore.
 *
 * If the nesting count of the semaphore reaches zero,
 * the next process waiting for it will be awaken.
 *
 * \note This routine is optimized for highest speed in
 *       the most common case: the semaphore has been locked just
 *       once and nobody else was waiting for it. Rearranging
 *       this code is probably a bad idea.
 *
 * \sa sem_obtain() sem_attempt()
 */
void sem_release(struct Semaphore *s)
{
	proc_forbid();
	sem_verify(s);

	ASSERT(s->owner == CurrentProcess);

	/*
	 * Decrement nesting count and check if the semaphore
	 * has been fully unlocked.
	 */
	if (--s->nest_count == 0)
	{
		Process *proc;

		/* Disown semaphore */
		s->owner = NULL;

		/* Give semaphore to the first applicant, if any */
		if (UNLIKELY((proc = (Process *)list_remHead(&s->wait_queue))))
		{
			s->nest_count = 1;
			s->owner = proc;
			SCHED_ENQUEUE(proc);
		}
	}

	proc_permit();
}

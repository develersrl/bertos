/*!
 * \file
 * <!--
 * Copyright 2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 1999,2000,2001 Bernardo Innocenti <bernie@develer.com>
 * This file is part of DevLib - See devlib/README for information.
 * -->
 *
 * \brief IPC signals implementation.
 *
 * Each process can wait for just one signal.
 * Multiple processes can wait for the same signal. In this
 * case, each signal will wake-up one of them.
 *
 * \version $Id$
 *
 * \author Bernardo Innocenti <bernie@develer.com>
 */

/*
 * $Log$
 * Revision 1.2  2004/06/03 11:27:09  bernie
 * Add dual-license information.
 *
 * Revision 1.1  2004/05/23 17:27:00  bernie
 * Import kern/ subdirectory.
 *
 */

#include "signal.h"
#include "proc.h"
#include "proc_p.h"
#include "hw.h"

// FIXME
#if CONFIG_KERN_SIGNALS

/*!
 * Check if any of the signals in \a sigs has occurred and clear them.
 * Return the signals that have occurred.
 */
sigset_t sig_check(sigset_t sigs)
{
	sigset_t result;

	DISABLE_INTS;
	result = CurrentProcess->sig_recv & sigs;
	CurrentProcess->sig_recv &= ~sigs;
	ENABLE_INTS;
	return result;
}


/*!
 * Sleep until any of the signals in \a sigs occurs.
 * Return the signal(s) that have awaked the process.
 */
sigset_t sig_wait(sigset_t sigs)
{
	sigset_t result;

	DISABLE_INTS;

	for(;;)
	{
		/* Check if we got at least one of the signals */
		if ((result = CurrentProcess->sig_recv & sigs))
		{
			/* Yes, clear signals and return */
			CurrentProcess->sig_recv &= ~sigs;
			ENABLE_INTS;
			return result;
		}

		/* No, go to sleep and proc_schedule() another process */
		CurrentProcess->sig_wait = sigs;
		proc_schedule();
	}
}


/*!
 * Send the signals \a sigs to the process \a proc.
 * The process will be awaken if it was waiting for any of them.
 *
 * This call is interrupt safe (no \c DISABLE_INTS/ENABLE_INTS protection)
 */
void _sig_signal(Process *proc, sigset_t sigs)
{
	/* Set the signals */
	proc->sig_recv |= sigs;

	/* Check if process needs to be awaken */
	if (proc->sig_recv & proc->sig_wait)
	{
		/* Wake up process and enqueue in ready list */
		proc->sig_wait = 0;
		SCHED_ENQUEUE(proc);
	}
}


/*!
 * Same as _sig_signal() with interrupt protection.
 *
 * \note Inlined manually because some compilers are too
 *       stupid to it automatically.
 */
void sig_signal(Process *proc, sigset_t sigs)
{
	DISABLE_INTS;

	/* Set the signals */
	proc->sig_recv |= sigs;

	/* Check if process needs to be awaken */
	if (proc->sig_recv & proc->sig_wait)
	{
		/* Wake up process and enqueue in ready list */
		proc->sig_wait = 0;
		SCHED_ENQUEUE(proc);
	}

	ENABLE_INTS;
}

#endif /* CONFIG_KERN_SIGNALS */


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
 * Revision 1.4  2004/07/30 14:30:27  rasky
 * Resa la sig_signal interrupt safe (con il nuovo scheduler IRQ-safe)
 * Rimossa event_doIntr (ora inutile) e semplificata la logica delle macro con funzioni inline
 *
 * Revision 1.3  2004/07/30 14:24:16  rasky
 * Task switching con salvataggio perfetto stato di interrupt (SR)
 * Kernel monitor per dump informazioni su stack dei processi
 *
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
	cpuflags_t flags;

	DISABLE_IRQSAVE(flags);
	result = CurrentProcess->sig_recv & sigs;
	CurrentProcess->sig_recv &= ~sigs;
	ENABLE_IRQRESTORE(flags);
	return result;
}


/*!
 * Sleep until any of the signals in \a sigs occurs.
 * Return the signal(s) that have awaked the process.
 */
sigset_t sig_wait(sigset_t sigs)
{
	sigset_t result;
	cpuflags_t flags;

	DISABLE_IRQSAVE(flags);

	/* Loop until we get at least one of the signals */
	while (!(result = CurrentProcess->sig_recv & sigs))
	{
		/* go to sleep and proc_schedule() another process */
		CurrentProcess->sig_wait = sigs;
		proc_schedule();
	}

	/* Signals found: clear them and return */
	CurrentProcess->sig_recv &= ~sigs;
	ENABLE_IRQRESTORE(flags);
	return result;
}


/*!
 * Send the signals \a sigs to the process \a proc.
 * The process will be awaken if it was waiting for any of them.
 *
 * \note This call is interrupt safe.
 */
void sig_signal(Process *proc, sigset_t sigs)
{
	cpuflags_t flags;
	DISABLE_IRQSAVE(flags);

	/* Set the signals */
	proc->sig_recv |= sigs;

	/* Check if process needs to be awaken */
	if (proc->sig_recv & proc->sig_wait)
	{
		/* Wake up process and enqueue in ready list */
		proc->sig_wait = 0;
		SCHED_ENQUEUE(proc);
	}

	ENABLE_IRQRESTORE(flags);
}

#endif /* CONFIG_KERN_SIGNALS */


/*!
 * \file
 * <!--
 * Copyright 2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 1999, 2000, 2001 Bernardo Innocenti <bernie@develer.com>
 * This file is part of DevLib - See devlib/README for information.
 * -->
 *
 * \brief IPC signals implementation.
 *
 * Signals are a low-level IPC primitive.  A process receives a signal
 * when some external event has happened.  Like interrupt requests,
 * signals do not carry any additional information.  If processing a
 * specific event requires additional data, the process must obtain it
 * through some other mechanism.
 *
 * Despite the name, one shouldn't confuse these signals with POSIX
 * signals.  POSIX signals are usually executed synchronously, like
 * software interrupts.
 *
 * In this implementation, each process has a limited set of signal
 * bits (usually 32) and can wait for multiple signals at the same
 * time using sig_wait().  Signals can also be polled using sig_check(),
 * but a process spinning on its signals usually defeats their purpose
 * of providing a multitasking-friendly infrastructure for event-driven
 * applications.
 *
 * Signals are like flags: they are either active or inactive.  After an
 * external event has delivered a particular signal, it remains raised until
 * the process acknowledges it using either sig_wait() or sig_check().
 * Counting signals is not a reliable way to count how many times a
 * particular event has occurred, because the same signal may be
 * delivered twice before the process can notice.
 *
 * Any execution context, including an interrupt handler, can deliver
 * a signal to a process using sig_signal().  Multiple distinct signals
 * may be delivered at once with a single invocation of sig_signal(),
 * although this is rarely useful.
 *
 * There's no hardcoded mapping of specific events to signal bits.
 * The meaning of a particular signal bit is defined by an agreement
 * between the delivering entity and the receiving process.
 * For instance, a terminal driver may be written to deliver
 * a signal bit called SIG_INT when it reads the CTRL-C sequence
 * from the keyboard, and a process may react to it by quitting.
 *
 * The SIG_SINGLE bit is reserved for a special purpose (this is
 * more a suggestion than a constraint).  When a process wants
 * wait for a single event on the fly, it needs not allocate a
 * free signal from its pool.  Instead, SIG_SINGLE can be used
 *
 * The "event" module is a higher-level interface that can optionally
 * deliver signals to processes.  Messages provide even higher-level
 * IPC services built on signals.  Semaphore arbitration is also
 * implemented using signals.
 *
 * Signals are very low overhead.  Using them exclusively to wait
 * for multiple asynchronous events results in very simple dispatch
 * logic with low processor and resource usage.
 *
 *
 * \version $Id$
 *
 * \author Bernardo Innocenti <bernie@develer.com>
 */

/*#*
 *#* $Log$
 *#* Revision 1.8  2004/09/14 21:06:44  bernie
 *#* Use debug.h instead of kdebug.h.
 *#*
 *#* Revision 1.7  2004/08/25 14:12:09  rasky
 *#* Aggiornato il comment block dei log RCS
 *#*
 *#* Revision 1.6  2004/08/14 19:37:57  rasky
 *#* Merge da SC: macros.h, pool.h, BIT_CHANGE, nome dei processi, etc.
 *#*
 *#* Revision 1.5  2004/08/04 21:50:33  bernie
 *#* Add extensive documentation.
 *#*
 *#* Revision 1.4  2004/07/30 14:30:27  rasky
 *#* Resa la sig_signal interrupt safe (con il nuovo scheduler IRQ-safe)
 *#* Rimossa event_doIntr (ora inutile) e semplificata la logica delle macro con funzioni inline
 *#*
 *#* Revision 1.3  2004/07/30 14:24:16  rasky
 *#* Task switching con salvataggio perfetto stato di interrupt (SR)
 *#* Kernel monitor per dump informazioni su stack dei processi
 *#*
 *#* Revision 1.2  2004/06/03 11:27:09  bernie
 *#* Add dual-license information.
 *#*
 *#* Revision 1.1  2004/05/23 17:27:00  bernie
 *#* Import kern/ subdirectory.
 *#*
 *#*/

#include "signal.h"
#include "proc.h"
#include "proc_p.h"
#include "hw.h"
#include <debug.h>

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

		/* When we come back here, a signal must be arrived */
		ASSERT(!CurrentProcess->sig_wait);
		ASSERT(CurrentProcess->sig_recv);
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


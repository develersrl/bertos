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
 * Copyright 2008 Bernie Innocenti <bernie@codewiz.org>
 * -->
 *
 * \brief Process scheduler (public interface).
 *
 * \version $Id: proc.h 1646 2008-08-17 13:49:48Z bernie $
 * \author Bernie Innocenti <bernie@codewiz.org>
 */
#include "irq.h"

#include <cfg/module.h>
#include <kern/proc_p.h>

#include <cfg/cfg_kern.h>

#include <unistd.h> // FIXME: move POSIX stuff to irq_posix.h


MOD_DEFINE(irq)

// FIXME
static void (*irq_handlers[100])(void);

/* signal handler */
void irq_entry(int signum)
{
	Process * const old_process = CurrentProcess;

	irq_handlers[signum]();

#if CONFIG_KERN_PREEMPT
	if (!CurrentProcess)
	{
		TRACEMSG("no runnable processes!");
		IRQ_ENABLE;
		pause();
	}
	else
	{
		if (old_process != CurrentProcess)
		{
			TRACEMSG("switching from %p:%s to %p:%s",
				old_process, old_process ? old_process->monitor.name : "-",
				CurrentProcess, CurrentProcess->monitor.name);

			if (old_process)
				swapcontext(&old_process->context, &CurrentProcess->context);
			else
				setcontext(&CurrentProcess->context);

			// not reached
		}
		TRACEMSG("keeping %p:%s", CurrentProcess, CurrentProcess->monitor.name);
	}
#endif // CONFIG_KERN_PREEMPT
}

void irq_register(int irq, void (*callback)(void))
{
	irq_handlers[irq] = callback;
}

void irq_init(void)
{
	struct sigaction act;
	act.sa_handler = irq_entry;
	sigemptyset(&act.sa_mask);
	//sigaddset(&act.sa_mask, irq);
	act.sa_flags = SA_RESTART; // | SA_SIGINFO;

	sigaction(SIGUSR1, &act, NULL);
	#if !(ARCH & ARCH_QT)
		sigaction(SIGALRM, &act, NULL);
	#endif

	MOD_INIT(irq);
}

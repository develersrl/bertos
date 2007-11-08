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
 * Copyright 2004 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief Monitor to check for stack overflows
 *
 * \version $Id$
 *
 * \author Giovanni Bajo <rasky@develer.com>
 */


#include "monitor.h"

#if CONFIG_KERN_MONITOR

#include "proc_p.h"
#include <mware/list.h>
#include <drv/timer.h>
#include <kern/proc.h>
#include <cfg/macros.h>
#include <cfg/debug.h>


static List MonitorProcs;


void monitor_init(void)
{
	LIST_INIT(&MonitorProcs);
}


void monitor_add(Process* proc, const char* name, cpustack_t* stack_base, size_t stack_size)
{
	proc->monitor.name = name;
	proc->monitor.stack_base = stack_base;
	proc->monitor.stack_size = stack_size;

	ADDTAIL(&MonitorProcs, &proc->monitor.link);
}


void monitor_remove(Process* proc)
{
	REMOVE(&proc->monitor.link);
}

void monitor_rename(Process *proc, const char* name)
{
	proc->monitor.name = name;
}

/* TODO: use containerof() */
#define MONITOR_NODE_TO_PROCESS(node) \
	(struct Process *)((intptr_t)(node) - offsetof(struct Process, monitor.link))

size_t monitor_checkStack(cpustack_t* stack_base, size_t stack_size)
{
	cpustack_t* beg;
	cpustack_t* cur;
	cpustack_t* end;
	size_t sp_free;

	beg = stack_base;
	end = stack_base + stack_size / sizeof(cpustack_t) - 1;

	if (CPU_STACK_GROWS_UPWARD)
	{
		cur = beg;
		beg = end;
		end = cur;
	}

	cur = beg;
	while (cur != end)
	{
		if (*cur != CONFIG_KERN_STACKFILLCODE)
			break;

		if (CPU_STACK_GROWS_UPWARD)
			cur--;
		else
			cur++;
	}

	sp_free = ABS(cur - beg) * sizeof(cpustack_t);
	return sp_free;
}


void monitor_report(void)
{
	struct Process* p;
	int i;

	if (LIST_EMPTY(&MonitorProcs))
	{
		kputs("No stacks registered in the monitor\n");
		return;
	}

	kprintf("%-24s%-8s%-8s%-8s%-8s\n", "Process name", "TCB", "SPbase", "SPsize", "SPfree");
	for (i=0;i<56;i++)
		kputchar('-');
	kputchar('\n');

	for (p = MONITOR_NODE_TO_PROCESS(LIST_HEAD(&MonitorProcs));
		 p->monitor.link.succ;
		 p = MONITOR_NODE_TO_PROCESS(p->monitor.link.succ))
	{
		size_t free = monitor_checkStack(p->monitor.stack_base, p->monitor.stack_size);
		kprintf("%-24s%8p%8p%8zx%8zx\n",
			p->monitor.name, p, p->monitor.stack_base, p->monitor.stack_size, free);
	}
}


static void NORETURN monitor(void)
{
	struct Process *p;

	while (1)
	{
		for (p = MONITOR_NODE_TO_PROCESS(LIST_HEAD(&MonitorProcs));
			p->monitor.link.succ;
			p = MONITOR_NODE_TO_PROCESS(p->monitor.link.succ))
		{
			size_t free = monitor_checkStack(p->monitor.stack_base, p->monitor.stack_size);

			if (free < 0x20)
				kprintf("MONITOR: WARNING: Free stack for process '%s' is only %x chars\n",
						p->monitor.name, free);

			timer_delay(500);
		}
	}
}


void monitor_start(size_t stacksize, cpustack_t *stack)
{
	proc_new(monitor, NULL, stacksize, stack);
}

#endif /* CONFIG_KERN_MONITOR */

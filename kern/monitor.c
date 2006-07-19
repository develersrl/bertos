/**
 * \file
 * <!--
 * Copyright 2004 Develer S.r.l. (http://www.develer.com/)
 * This file is part of DevLib - See README.devlib for information.
 * -->
 *
 * \brief Monitor to check for stack overflows
 *
 * \version $Id$
 *
 * \author Giovanni Bajo <rasky@develer.com>
 */

/*#*
 *#* $Log$
 *#* Revision 1.9  2006/07/19 12:56:27  bernie
 *#* Convert to new Doxygen style.
 *#*
 *#* Revision 1.8  2006/03/22 09:49:22  bernie
 *#* Reduce memory usage.
 *#*
 *#* Revision 1.7  2006/03/13 02:07:14  bernie
 *#* Add TODO item.
 *#*
 *#* Revision 1.6  2006/02/24 01:17:05  bernie
 *#* Update for new emulator.
 *#*
 *#* Revision 1.5  2005/11/04 16:20:02  bernie
 *#* Fix reference to README.devlib in header.
 *#*
 *#* Revision 1.4  2005/04/11 19:10:28  bernie
 *#* Include top-level headers from cfg/ subdir.
 *#*
 *#* Revision 1.3  2004/11/28 23:20:25  bernie
 *#* Remove obsolete INITLIST macro.
 *#*
 *#* Revision 1.2  2004/10/26 09:01:15  bernie
 *#* monitor_rename(): New function.
 *#*
 *#* Revision 1.1  2004/10/03 20:39:03  bernie
 *#* Import in DevLib.
 *#*
 *#* Revision 1.2  2004/10/03 20:36:43  bernie
 *#* Use debug.h instead of drv/kdebug.h; Misc spacing/header fixes.
 *#*
 *#* Revision 1.1  2004/09/30 23:19:30  rasky
 *#* Estratto il monitor degli stack da proc.c in due file a parte: monitor.c/h
 *#* Rinominata monitor_debug_stacks in monitor_report
 *#*/

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
	(struct Process *)((char *)(node) - offsetof(struct Process, monitor.link))

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
		kprintf("%-24s%8p%8p%8lx%8lx\n",
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

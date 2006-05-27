/**
 * \file
 * <!--
 * Copyright 2006 Develer S.r.l. (http://www.develer.com/)
 * This file is part of DevLib - See README.devlib for information.
 * -->
 *
 * \brief Simple notifier for the observer/subject pattern (implementation)
 *
 * \version $Id$
 * \author Bernardo Innocenti <bernie@develer.com>
 */

#include "observer.h"
#include <cfg/cpu.h>  // IRQ_DISABLE/IRQ_ENABLE


void observer_SetEvent(Observer *observer, void (*event)(int event_id, void *param))
{
	observer->event = event;
}

void observer_InitSubject(Subject *subject)
{
	LIST_INIT(&subject->observers);
}

void observer_Subscribe(Subject *subject, Observer *observer)
{
	ATOMIC(ADDHEAD(&subject->observers, &observer->link));
}

void observer_Unsubscribe(UNUSED_ARG(Subject *,subject), Observer *observer)
{
	ATOMIC(REMOVE(&observer->link));
}

void observer_notify(Subject *subject, int event_id, void *param)
{
	Observer *observer;
	cpuflags_t irqstate;
	IRQ_SAVE_DISABLE(irqstate);

	/*
	 * Run over list with protection against other
	 * threads, but re-enable irqs in callbacks.
	 */
	FOREACH_NODE(observer, &subject->observers)
	{
		IRQ_RESTORE(irqstate);
		observer->event(event_id, param);
		IRQ_SAVE_DISABLE(irqstate);
	}

	IRQ_RESTORE(irqstate);
}

/*!
 * \file
 * <!--
 * Copyright 2003,2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 1999,2001,2003 Bernardo Innocenti <bernie@develer.com>
 * This file is part of DevLib - See devlib/README for information.
 * -->
 *
 * \brief Events handling
 *
 * This module implements a common system for executing
 * a user defined action calling a hook function.
 *
 * \version $Id$
 *
 * \author Bernardo Innocenti <bernie@develer.com>
 */

/*
 * $Log$
 * Revision 1.6  2004/08/14 19:37:57  rasky
 * Merge da SC: macros.h, pool.h, BIT_CHANGE, nome dei processi, etc.
 *
 * Revision 1.5  2004/07/30 14:30:27  rasky
 * Resa la sig_signal interrupt safe (con il nuovo scheduler IRQ-safe)
 * Rimossa event_doIntr (ora inutile) e semplificata la logica delle macro con funzioni inline
 *
 * Revision 1.4  2004/06/07 15:58:00  aleph
 * Add function prototypes
 *
 * Revision 1.3  2004/06/06 18:25:44  bernie
 * Rename event macros to look like regular functions.
 *
 * Revision 1.2  2004/06/03 11:27:09  bernie
 * Add dual-license information.
 *
 * Revision 1.1  2004/05/23 17:27:00  bernie
 * Import kern/ subdirectory.
 *
 */
#ifndef KERN_EVENT_H
#define KERN_EVENT_H

#include <config.h>

#ifdef CONFIG_KERNEL
	#include "config_kern.h"
	#if defined(CONFIG_KERN_SIGNALS) && CONFIG_KERN_SIGNALS
		#include "signal.h"
	#endif
#endif


/* Forward decl */
struct Process;


/*! Event types */
enum EventAction
{
	EVENT_IGNORE,	/*!< No-op event */
#if defined(CONFIG_KERN_SIGNALS) && CONFIG_KERN_SIGNALS
	EVENT_SIGNAL,	/*!< Send a signal to a process */
#endif
	EVENT_SOFTINT	/*!< Trigger a software interrupt (i.e.: call user hook) */
};

//! User defined callback type
typedef void (*Hook)(void *);

typedef struct Event
{
	enum EventAction action;
	union
	{
#if defined(CONFIG_KERN_SIGNALS) && CONFIG_KERN_SIGNALS
		struct
		{
			struct Process *sig_proc;  /* Process to be signalled */
			sig_t           sig_bit;   /* Signal to send */
		} Sig;
#endif
		struct
		{
			Hook  func;         /* Pointer to softint hook */
			void *user_data;    /* Data to be passed back to user hook */
		} Int;
	} Ev;
} Event;


/*! Initialize the event \a e as a no-op */
#define event_initNone(e) \
	((e)->action = EVENT_IGNORE)

/*! Same as event_initNone(), but returns the initialized event */
INLINE Event event_createNone(void);
INLINE Event event_createNone(void)
{
	Event e;
	e.action = EVENT_IGNORE;
	return e;
}

/*! Initialize the event \a e with a software interrupt (call function \a f, with parameter \a u) */
#define event_initSoftInt(e,f,u) \
	((e)->action = EVENT_SOFTINT,(e)->Ev.Int.func = (f), (e)->Ev.Int.user_data = (u))

/*! Same as event_initSoftInt(), but returns the initialized event */
INLINE Event event_createSoftInt(Hook func, void* user_data)
{
	Event e;
	e.action = EVENT_SOFTINT;
	e.Ev.Int.func = func;
	e.Ev.Int.user_data = user_data;
	return e;
}


#if defined(CONFIG_KERN_SIGNALS) && CONFIG_KERN_SIGNALS

/*! Initialize the event \a e with a signal (send signal \a s to process \a p) */
#define event_initSignal(e,p,s) \
	((e)->action = EVENT_SIGNAL,(e)->Ev.Sig.sig_proc = (p), (e)->Ev.Sig.sig_bit = (s))

/*! Same as event_initSignal(), but returns the initialized event */
INLINE Event event_createSignal(struct Process* proc, sig_t bit)
{
	Event e;
	e.action = EVENT_SIGNAL;
	e.Ev.Sig.sig_proc = proc;
	e.Ev.Sig.sig_bit = bit;
	return e;
}

#endif

/*! Trigger an event */
INLINE void event_do(struct Event* e)
{
	if ((e)->action == EVENT_SOFTINT)
		(e)->Ev.Int.func((e)->Ev.Int.user_data);
#if defined(CONFIG_KERN_SIGNALS) && CONFIG_KERN_SIGNALS
	else if ((e)->action == EVENT_SIGNAL)
		sig_signal((e)->Ev.Sig.sig_proc, (e)->Ev.Sig.sig_bit);
#endif
}

#ifdef CONFIG_KERN_OLDNAMES
	#define INITEVENT_SIG  event_initSignal
	#define INITEVENT_INT  event_initSoftInt
	#define DOEVENT        event_do
	#define DOEVENT_INTR   event_do
#endif

#endif /* KERN_EVENT_H */

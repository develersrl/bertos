/*!
 * \file
 * <!--
 * Copyright 2003,2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 1999,2001,2003 Bernardo Innocenti <bernie@develer.com>
 * All Rights Reserved.
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
 * Revision 1.1  2004/05/23 17:27:00  bernie
 * Import kern/ subdirectory.
 *
 */
#ifndef KERN_EVENT_H
#define KERN_EVENT_H

#include "config.h"

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


/*! Initialize an event with a softint */
#define INITEVENT_INT(e,f,u) \
	((e)->action = EVENT_SOFTINT,(e)->Ev.Int.func = (f), (e)->Ev.Int.user_data = (u))


#if defined(CONFIG_KERN_SIGNALS) && CONFIG_KERN_SIGNALS

/*! Initialize an event with a signal */
#define INITEVENT_SIG(e,p,s) \
	((e)->action = EVENT_SIGNAL,(e)->Ev.Sig.sig_proc = (p), (e)->Ev.Sig.sig_bit = (s))

/*! Trigger an event */
#define DOEVENT(e) \
do { \
	if ((e)->action == EVENT_SIGNAL) \
		sig_signal((e)->Ev.Sig.sig_proc, (e)->Sig.sig_bit); \
	else if ((e)->action == EVENT_SOFTINT) \
		(e)->Ev.Int.func((e)->Ev.Int.user_data); \
} while (0)

/*! Trigger an event (to be used inside interrupts) */
#define DOEVENT_INTR(e) \
do { \
	if ((e)->action == EVENT_SIGNAL) \
		_sig_signal((e)->Ev.Sig.sig_proc, (e)->Ev.Sig.sig_bit); \
	else if ((e)->action == EVENT_SOFTINT) \
		(e)->Ev.Int.func((e)->Ev.Int.user_data); \
} while (0)

#else /* !CONFIG_KERN_SIGNALS */

/*! Trigger an event */
#define DOEVENT(e) \
do { \
	if ((e)->action == EVENT_SOFTINT) \
		(e)->Ev.Int.func((e)->Ev.Int.user_data); \
} while (0)

/*! Trigger an event (to be used inside interrupts) */
#define DOEVENT_INTR(e) \
do { \
	if ((e)->action == EVENT_SOFTINT) \
		(e)->Ev.Int.func((e)->Ev.Int.user_data); \
} while (0)

#endif

#endif /* KERN_EVENT_H */

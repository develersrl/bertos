/*!
 * \file
 * <!--
 * Copyright 2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 1999,2001 Bernardo Innocenti <bernie@develer.com>
 * This file is part of DevLib - See devlib/README for information.
 * -->
 *
 * \brief Simple inter-process messaging system
 *
 * This module implements a common system for executing
 * a user defined action calling a hook function.
 *
 * \version $Id$
 *
 * \author Bernardo Innocenti <bernie@develer.com>
 */

/*#*
 *#* $Log$
 *#* Revision 1.5  2004/11/28 23:20:25  bernie
 *#* Remove obsolete INITLIST macro.
 *#*
 *#* Revision 1.4  2004/10/19 08:22:09  bernie
 *#* msg_peek(): New function.
 *#*
 *#* Revision 1.3  2004/08/25 14:12:09  rasky
 *#* Aggiornato il comment block dei log RCS
 *#*
 *#* Revision 1.2  2004/08/14 19:37:57  rasky
 *#* Merge da SC: macros.h, pool.h, BIT_CHANGE, nome dei processi, etc.
 *#*
 *#* Revision 1.1  2004/06/06 15:11:08  bernie
 *#* Import into DevLib.
 *#*
 *#*/
#ifndef KERN_MSG_H
#define KERN_MSG_H

#include "event.h"
#include <mware/list.h>


typedef struct MsgPort
{
	List  queue; /*!< Messages queued at this port */
	Event evn;   /*!< Event to trigger when a message arrives */
} MsgPort;


typedef struct Msg
{
	Node     link;      /*!< Link into message port queue */
	MsgPort *replyPort; /*!< Port to which the msg is to be replied */
	/* User data may follow */
} Msg;


/*! Initialize a message port */
INLINE void msg_initPort(MsgPort* port, Event event)
{
	LIST_INIT(&port->queue);
	port->evn = event;
}

/*! Queue \a msg into \a port, triggering the associated event */
INLINE void msg_put(MsgPort* port, Msg* msg)
{
	ADDTAIL(&port->queue, &msg->link);
	event_do(&port->evn);
}

/* Get the first message from the queue of \a port, or NULL if the port is empty */
INLINE Msg* msg_get(MsgPort* port)
{
	return (Msg*)REMHEAD(&port->queue);
}

/* Peek the first message in the queue of \a port, or NULL if the port is empty */
INLINE Msg *msg_peek(MsgPort *port)
{
	if (ISLISTEMPTY(&port->queue))
		return NULL;

	return (Msg *)port->queue.head;
}

/*! Send back (reply) \a msg to its sender */
INLINE void msg_reply(Msg* msg)
{
	msg_put(msg->replyPort, msg);
}

#endif /* KERN_MSG_H */

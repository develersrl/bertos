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

/*
 * $Log$
 * Revision 1.1  2004/06/06 15:11:08  bernie
 * Import into DevLib.
 *
 */
#ifndef KERN_MSG_H
#define KERN_MSG_H

#include "event.h"


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


/*! Initialize a messge port */
#define INITPORT(p)			INITLIST(&(p)->queue)

/*! Queue a message to a message port */
#define PUTMSG(p,m) (ADDTAIL(&(p)->queue,(Node *)(m)), DOEVENT(&(p)->evn))
#define PUTMSG_INTR(p,m) (ADDTAIL(&(p)->queue,(Node *)(m)), DOEVENT_INTR(&(p)->evn))

/*! Get first message from port's queue (returns NULL when the port is empty) */
#define GETMSG(p) ((Msg *)REMHEAD(&(p)->queue))

/*! Reply a message to its sender */
#define REPLYMSG(m) (PUTMSG((m)->replyPort,(m)))

#endif /* KERN_MSG_H */

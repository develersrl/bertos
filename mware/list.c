/*!
 * \file
 * <!--
 * Copyright 2003,2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2001 Bernardo Innocenti <bernie@develer.com>
 * This file is part of DevLib - See devlib/README for information.
 * -->
 *
 * \version $Id$
 *
 * \author Bernardo Innocenti <bernie@develer.com>
 *
 * \brief List handling functions
 */

/*
 * $Log$
 * Revision 1.2  2004/06/03 11:27:09  bernie
 * Add dual-license information.
 *
 * Revision 1.1  2004/05/23 15:43:16  bernie
 * Import mware modules.
 *
 */

#include "list.h"


Node *_list_rem_head(List *l)
{
	Node *n;

	if (ISLISTEMPTY(l))
		return (Node *)0;

	n = l->head;
	l->head = n->succ;
	n->succ->pred = (Node *)l;
	return n;
}


Node *_list_rem_tail(List *l)
{
	Node *n;

	if (ISLISTEMPTY(l))
		return (Node *)0;

	n = l->tail;
	l->tail = n->pred;
	n->pred->succ = (Node *)&l->null;
	return n;
}

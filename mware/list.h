/*!
 * \file
 * <!--
 * Copyright 2003, 2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2001 Bernardo Innocenti <bernie@develer.com>
 * This file is part of DevLib - See devlib/README for information.
 * -->
 *
 * \version $Id$
 *
 * \author Bernardo Innocenti <bernie@develer.com>
 *
 * \brief General pourpose double-linked lists
 */

/*#*
 *#* $Log$
 *#* Revision 1.12  2005/01/22 04:21:32  bernie
 *#* Add integrity checks.
 *#*
 *#* Revision 1.11  2004/12/31 16:44:11  bernie
 *#* list_remHead(), list_remTail(): Name like normal functions.
 *#*
 *#* Revision 1.10  2004/11/28 23:21:05  bernie
 *#* Remove obsolete INITLIST macro.
 *#*
 *#* Revision 1.9  2004/10/21 09:37:55  bernie
 *#* Revamp documentation.
 *#*
 *#* Revision 1.8  2004/10/19 08:46:34  bernie
 *#* Fix header.
 *#*
 *#* Revision 1.7  2004/08/25 14:12:09  rasky
 *#* Aggiornato il comment block dei log RCS
 *#*
 *#* Revision 1.6  2004/07/30 14:34:10  rasky
 *#* Vari fix per documentazione e commenti
 *#* Aggiunte PP_CATn e STATIC_ASSERT
 *#*
 *#* Revision 1.5  2004/07/20 23:45:01  bernie
 *#* Finally remove redundant protos.
 *#*
 *#* Revision 1.4  2004/07/18 22:12:53  bernie
 *#* Fix warnings with GCC 3.3.2.
 *#*
 *#* Revision 1.3  2004/07/18 22:01:43  bernie
 *#* REMHEAD(), REMTAIL(): Move to list.h as inline functions.
 *#*
 *#* Revision 1.2  2004/06/03 11:27:09  bernie
 *#* Add dual-license information.
 *#*
 *#* Revision 1.1  2004/05/23 15:43:16  bernie
 *#* Import mware modules.
 *#*
 *#*/
#ifndef MWARE_LIST_H
#define MWARE_LIST_H

#include <compiler.h> // INLINE

/*!
 * This structure represents a node for bidirectional lists.
 *
 * Data is usually appended to nodes by making them the first
 * field of another struture, as a poor-man's form of inheritance.
 */
typedef struct _Node
{
	struct _Node *succ;
	struct _Node *pred;
} Node;

/*!
 * Head of a doubly-linked list of \c Node structs.
 *
 * Lists must be initialized with LIST_INIT() prior to use.
 *
 * Nodes can be added and removed from either end of the list
 * with O(1) performance.  Iterating over these lists can be
 * tricky: use the FOREACHNODE() macro instead.
 */
typedef struct _List
{
	Node *head;
	Node *null;
	Node *tail;
} List;


/*! Template for a list of \a T structures. */
#define DECLARE_LIST(T) \
	struct { T *head; T *null; T *tail; }

/*! Template for a node in a list of \a T structures. */
#define DECLARE_NODE(T) \
	struct { T *succ; T *pred; }

/*! Template for a node in a list of \a T structures. */
#define DECLARE_NODE_ANON(T) \
	T *succ; T *pred;

/*!
 * Iterate over all nodes in a list.
 *
 * This macro generates a "for" statement using the following parameters:
 * \param n   Node pointer to be used in each iteration.
 * \param l   Pointer to list.
 */
#define FOREACHNODE(n,l) \
	for( \
		(n) = (typeof(n))((l)->head); \
		((Node *)(n))->succ; \
		(n) = (typeof(n))(((Node *)(n))->succ) \
	)

/*! Initialize a list. */
#define LIST_INIT(l) \
	do { \
		(l)->head = (Node *)(&(l)->null); \
		(l)->null = NULL; \
		(l)->tail = (Node *)(&(l)->head); \
	} while (0)

/* Make sure that a list is valid (it was initialized and is not corrupted) */
#ifdef _DEBUG
	#define LIST_ASSERT_VALID(l) \
		do { \
			Node *n, *pred; \
			ASSERT((l)->head != NULL); \
			ASSERT((l)->null == NULL); \
			ASSERT((l)->tail != NULL); \
			pred = (Node *)(&(l)->head); \
			FOREACHNODE(n, l) \
			{ \
				ASSERT(n->pred == pred); \
				pred = n; \
			} \
			ASSERT(n == (Node *)(&(l)->null)); \
		} while (0)
#else
	#define LIST_ASSERT_VALID(l) do {} while (0)
#endif

/*! Add node to list head. */
#define ADDHEAD(l,n) \
	do { \
		(n)->succ = (l)->head; \
		(n)->pred = (Node *)&(l)->head; \
		(n)->succ->pred = (n); \
		(l)->head = (n); \
	} while (0)

/*! Add node to list tail. */
#define ADDTAIL(l,n) \
	do { \
		(n)->succ = (Node *)(&(l)->null); \
		(n)->pred = (l)->tail; \
		(n)->pred->succ = (n); \
		(l)->tail = (n); \
	} while (0)

/*!
 * Insert node \a n before node \a ln.
 *
 * \note You can't pass in a list header as \a ln, but
 *       it is safe to pass list-\>head of an empty list.
 */
#define INSERTBEFORE(n,ln) \
	do { \
		(n)->succ = (ln); \
		(n)->pred = (ln)->pred; \
		(ln)->pred->succ = (n); \
		(ln)->pred = (n); \
	} while (0)

/*!
 * Remove \a n from whatever list it is in.
 *
 * \note Removing a node that has not previously been
 *       inserted into a list invokes undefined behavior.
 */
#define REMOVE(n) \
	do { \
		(n)->pred->succ = (n)->succ; \
		(n)->succ->pred = (n)->pred; \
	} while (0)

/*! Tell whether a list is empty. */
#define ISLISTEMPTY(l)  ( (l)->head == (Node *)(&(l)->null) )

/*!
 * Unlink a node from the head of the list \a l.
 *
 * \return Pointer to node, or NULL if the list was empty.
 */
INLINE Node *list_remHead(List *l)
{
	Node *n;

	if (ISLISTEMPTY(l))
		return (Node *)0;

	n = l->head;
	l->head = n->succ;
	n->succ->pred = (Node *)l;
	return n;
}

/*!
 * Unlink a node from the tail of the list \a l.
 *
 * \return Pointer to node, or NULL if the list was empty.
 */
INLINE Node *list_remTail(List *l)
{
	Node *n;

	if (ISLISTEMPTY(l))
		return (Node *)0;

	n = l->tail;
	l->tail = n->pred;
	n->pred->succ = (Node *)&l->null;
	return n;
}

/* OBSOLETE names */
#define REMHEAD list_remHead
#define REMTAIL list_remTail

#endif /* MWARE_LIST_H */

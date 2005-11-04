/*!
 * \file
 * <!--
 * Copyright 2003, 2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2001 Bernardo Innocenti <bernie@develer.com>
 * This file is part of DevLib - See README.devlib for information.
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
 *#* Revision 1.15  2005/11/04 16:20:02  bernie
 *#* Fix reference to README.devlib in header.
 *#*
 *#* Revision 1.14  2005/07/19 07:25:18  bernie
 *#* Refactor to remove type aliasing problems.
 *#*
 *#* Revision 1.13  2005/04/11 19:10:28  bernie
 *#* Include top-level headers from cfg/ subdir.
 *#*
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

#include <cfg/compiler.h> /* INLINE */
#include <cfg/debug.h> /* ASSERT() */

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
	Node head;
	Node tail;
} List;


/*!
 * Template for a naked node in a list of \a T structures.
 *
 * To be used as data member in other structures:
 *
 * \code
 *    struct Foo
 *    {
 *        DECLARE_NODE_ANON(struct Foo)
 *        int a;
 *        float b;
 *    }
 *
 *    DECLARE_LIST_TYPE(Foo);
 *
 *    void foo(void)
 *    {
 *        static LIST_TYPE(Foo) foo_list;
 *        static Foo foo1, foo2;
 *        Foo *fp;
 *
 *        LIST_INIT(&foo_list);
 *        LIST_ADDHEAD(&foo_list, &foo1);
 *        INSERTBEFORE(&foo_list, &foo2);
 *        FOREACHNODE(fp, &foo_list)
 *		fp->a = 10;
 *    }
 *
 * \endcode
 */
#define DECLARE_NODE_ANON(T) \
	T *succ; T *pred;

/*! Declare a typesafe node for structures of type \a T. */
#define DECLARE_NODE_TYPE(T) \
	typedef struct T##Node { T *succ; T *pred; } T##Node

/*! Template for a list of \a T structures. */
#define DECLARE_LIST_TYPE(T) \
	DECLARE_NODE_TYPE(T); \
	typedef struct T##List { \
		 T##Node head; \
		 T##Node tail; \
	} T##List

#define NODE_TYPE(T) T##Node
#define LIST_TYPE(T) T##List

/*!
 * Get a pointer to the first node in a list.
 *
 * If \a l is empty, result points to l->tail.
 */
#define LIST_HEAD(l) ((l)->head.succ)

/*!
 * Get a pointer to the last node in a list.
 *
 * If \a l is empty, result points to l->head.
 */
#define LIST_TAIL(l) ((l)->tail.pred)

/*!
 * Iterate over all nodes in a list.
 *
 * This macro generates a "for" statement using the following parameters:
 * \param n   Node pointer to be used in each iteration.
 * \param l   Pointer to list.
 */
#define FOREACHNODE(n, l) \
	for( \
		(n) = (typeof(n))LIST_HEAD(l); \
		((Node *)(n))->succ; \
		(n) = (typeof(n))(((Node *)(n))->succ) \
	)

/*! Initialize a list. */
#define LIST_INIT(l) \
	do { \
		(l)->head.succ = (typeof((l)->head.succ)) &(l)->tail; \
		(l)->head.pred = NULL; \
		(l)->tail.succ = NULL; \
		(l)->tail.pred = (typeof((l)->tail.pred)) &(l)->head; \
	} while (0)

#ifdef _DEBUG
	/*! Make sure that a list is valid (it was initialized and is not corrupted). */
	#define LIST_ASSERT_VALID(l) \
		do { \
			Node *n, *pred; \
			ASSERT((l)->head.succ != NULL); \
			ASSERT((l)->head.pred == NULL); \
			ASSERT((l)->tail.succ == NULL); \
			ASSERT((l)->tail.pred != NULL); \
			pred = &(l)->head; \
			FOREACHNODE(n, l) \
			{ \
				ASSERT(n->pred == pred); \
				pred = n; \
			} \
			ASSERT(n == &(l)->tail); \
		} while (0)

	#define INVALIDATE_NODE(n) ((n)->succ = (n)->pred = NULL)
#else
	#define LIST_ASSERT_VALID(l) do {} while (0)
	#define INVALIDATE_NODE(n) do {} while (0)
#endif

/*! Add node to list head. */
#define ADDHEAD(l,n) \
	do { \
		ASSERT(l); \
		ASSERT(n); \
		(n)->succ = (l)->head.succ; \
		(n)->pred = (l)->head.succ->pred; \
		(n)->succ->pred = (n); \
		(n)->pred->succ = (n); \
	} while (0)

/*! Add node to list tail. */
#define ADDTAIL(l,n) \
	do { \
		ASSERT(l); \
		ASSERT(n); \
		(n)->succ = &(l)->tail; \
		(n)->pred = (l)->tail.pred; \
		(n)->pred->succ = (n); \
		(l)->tail.pred = (n); \
	} while (0)

/*!
 * Insert node \a n before node \a ln.
 *
 * \note You can't pass in a list header as \a ln, but
 *       it is safe to pass list-\>head of an empty list.
 */
#define INSERT_BEFORE(n,ln) \
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
		INVALIDATE_NODE(n); \
	} while (0)

/*! Tell whether a list is empty. */
#define LIST_EMPTY(l)  ( (void *)((l)->head.succ) == (void *)(&(l)->tail) )

/*!
 * Unlink a node from the head of the list \a l.
 *
 * \return Pointer to node, or NULL if the list was empty.
 */
INLINE Node *list_remHead(List *l)
{
	Node *n;

	if (LIST_EMPTY(l))
		return (Node *)0;

	n = l->head.succ; /* Get first node. */
	l->head.succ = n->succ; /* Link list head to second node. */
	n->succ->pred = &l->head; /* Link second node to list head. */

	INVALIDATE_NODE(n);
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

	if (LIST_EMPTY(l))
		return (Node *)0;

	n = l->tail.pred; /* Get last node. */
	l->tail.pred = n->pred; /* Link list tail to second last node. */
	n->pred->succ = &l->tail; /* Link second last node to list tail. */

	INVALIDATE_NODE(n);
	return n;
}

/* OBSOLETE names */
#define REMHEAD list_remHead
#define REMTAIL list_remTail
#define INSERTBEFORE INSERT_BEFORE
#define ISLISTEMPTY LIST_EMPTY


#endif /* MWARE_LIST_H */

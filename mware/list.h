/*!
 * \file
 * Copyright (C) 2001 Bernardo Innocenti <bernie@develer.com>
 * Copyright (C) 2003 Develer S.r.l. (http://www.develer.com/)
 * All Rights Reserved.
 *
 * \version $Id$
 *
 * \author Bernardo Innocenti <bernie@develer.com>
 *
 * \brief General pourpose double-linked lists
 */

/*
 * $Log$
 * Revision 1.1  2004/05/23 15:43:16  bernie
 * Import mware modules.
 *
 */
#ifndef MWARE_LIST_H
#define MWARE_LIST_H

typedef struct _Node
{
	struct _Node *succ;
	struct _Node *pred;
} Node;

typedef struct _List
{
	Node *head;
	Node *null;
	Node *tail;
} List;


/*! Template for a list of \a T  structures */
#define DECLARE_LIST(T) \
	struct { T *head; T *null; T *tail; }

/*! Template for a node in a list of \a T structures */
#define DECLARE_NODE(T) \
	struct { T *succ; T *pred; }

/*! Template for a node in a list of \a T structures */
#define DECLARE_NODE_ANON(T) \
	T *succ; T *pred;

/*!
 * Iterate over all nodes in a list. This statement defines a for cicle
 * accepting the following parameters:
 * \param n   node pointer to be used in each iteration
 * \param l   pointer to list
 */
#define FOREACHNODE(n,l) \
	for( \
		(n) = (typeof(n))((l)->head); \
		((Node *)(n))->succ; \
		(n) = (typeof(n))(((Node *)(n))->succ) \
	)

/*! Initialize a list */
#define INITLIST(l) \
	do { \
		(l)->head = (Node *)(&(l)->null); \
		(l)->null = NULL; \
		(l)->tail = (Node *)(&(l)->head); \
	} while (0)

/*! Add node to list head */
#define ADDHEAD(l,n) \
	do { \
		(n)->succ = (l)->head; \
		(n)->pred = (Node *)&(l)->head; \
		(n)->succ->pred = (n); \
		(l)->head = (n); \
	} while (0)

/*! Add node to list tail */
#define ADDTAIL(l,n) \
	do { \
		(n)->succ = (Node *)(&(l)->null); \
		(n)->pred = (l)->tail; \
		(n)->pred->succ = (n); \
		(l)->tail = (n); \
	} while (0)

/*!
 * Insert node \a n before node \a ln
 * Note: you can't pass in a list header as \a ln, but
 * it is safe to pass list-\>head of an empty list.
 */
#define INSERTBEFORE(n,ln) \
	do { \
		(n)->succ = (ln); \
		(n)->pred = (ln)->pred; \
		(ln)->pred->succ = (n); \
		(ln)->pred = (n); \
	} while (0)

/*! Remove \a n from whatever list it is in */
#define REMOVE(n) \
	do { \
		(n)->pred->succ = (n)->succ; \
		(n)->succ->pred = (n)->pred; \
	} while (0)

/*! Tell whether a list is empty */
#define ISLISTEMPTY(l)  ( (l)->head == (Node *)(&(l)->null) )

/*! \name Extract an element from the head/tail of the list. If the list empty, return NULL. */
/*\{*/
#define REMHEAD(l) _list_rem_head(l)
#define REMTAIL(l) _list_rem_tail(l)
/*\}*/

/* Prototypes for out-of-line functions */
Node *_list_rem_head(List *l);
Node *_list_rem_tail(List *l);


#endif /* MWARE_LIST_H */

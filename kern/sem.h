/*!
 * \file
 * <!--
 * Copyright 2001,2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 1999,2000,2001 Bernardo Innocenti <bernie@develer.com>
 * This file is part of DevLib - See devlib/README for information.
 * -->
 *
 * \brief Mutually exclusive semaphores.
 *        Shared locking not supported in this implementation.
 *
 * \version $Id$
 *
 * \author Bernardo Innocenti <bernie@develer.com>
 */

/*
 * $Log$
 * Revision 1.2  2004/06/03 11:27:09  bernie
 * Add dual-license information.
 *
 * Revision 1.1  2004/05/23 17:27:00  bernie
 * Import kern/ subdirectory.
 *
 */

#ifndef KERN_SEM_H
#define KERN_SEM_H

#include "compiler.h"
#include <mware/list.h>

/* Fwd decl */
struct Process;


struct Semaphore
{
	struct Process *owner;
	List            wait_queue;
	int             nest_count;
};

/*!
 * \name Process synchronization services
 * \{
 */
void sem_init(struct Semaphore *s);
bool sem_attempt(struct Semaphore *s);
void sem_obtain(struct Semaphore *s);
void sem_release(struct Semaphore *s);
/* \} */

#endif /* KERN_SEM_H */

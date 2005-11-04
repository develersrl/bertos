/*!
 * \file
 * <!--
 * Copyright 2001,2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 1999,2000,2001 Bernardo Innocenti <bernie@develer.com>
 * This file is part of DevLib - See README.devlib for information.
 * -->
 *
 * \brief Mutually exclusive semaphores.
 *        Shared locking not supported in this implementation.
 *
 * \version $Id$
 *
 * \author Bernardo Innocenti <bernie@develer.com>
 */

/*#*
 *#* $Log$
 *#* Revision 1.6  2005/11/04 16:20:02  bernie
 *#* Fix reference to README.devlib in header.
 *#*
 *#* Revision 1.5  2005/04/11 19:10:28  bernie
 *#* Include top-level headers from cfg/ subdir.
 *#*
 *#* Revision 1.4  2005/01/22 04:21:20  bernie
 *#* Add handy typedef for struct Semaphore.
 *#*
 *#* Revision 1.3  2004/08/25 14:12:09  rasky
 *#* Aggiornato il comment block dei log RCS
 *#*
 *#* Revision 1.2  2004/06/03 11:27:09  bernie
 *#* Add dual-license information.
 *#*
 *#* Revision 1.1  2004/05/23 17:27:00  bernie
 *#* Import kern/ subdirectory.
 *#*
 *#*/

#ifndef KERN_SEM_H
#define KERN_SEM_H

#include <cfg/compiler.h>
#include <mware/list.h>

/* Fwd decl */
struct Process;


typedef struct Semaphore
{
	struct Process *owner;
	List            wait_queue;
	int             nest_count;
} Semaphore;

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

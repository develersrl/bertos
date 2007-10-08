/**
 * \file
 * <!--
 * Copyright 2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 1999,2000,2001 Bernardo Innocenti <bernie@develer.com>
 * This file is part of DevLib - See README.devlib for information.
 * -->
 *
 * \brief Signal module (public interface).
 *
 * \version $Id$
 *
 * \author Bernardo Innocenti <bernie@develer.com>
 */

/*#*
 *#* $Log$
 *#* Revision 1.8  2006/07/19 12:56:27  bernie
 *#* Convert to new Doxygen style.
 *#*
 *#* Revision 1.7  2005/11/04 16:20:02  bernie
 *#* Fix reference to README.devlib in header.
 *#*
 *#* Revision 1.6  2005/04/11 19:10:28  bernie
 *#* Include top-level headers from cfg/ subdir.
 *#*
 *#* Revision 1.5  2004/12/08 08:57:35  bernie
 *#* Rename sigset_t to sigmask_t.
 *#*
 *#* Revision 1.4  2004/08/25 14:12:09  rasky
 *#* Aggiornato il comment block dei log RCS
 *#*
 *#* Revision 1.3  2004/07/30 14:30:27  rasky
 *#* Resa la sig_signal interrupt safe (con il nuovo scheduler IRQ-safe)
 *#* Rimossa event_doIntr (ora inutile) e semplificata la logica delle macro con funzioni inline
 *#*
 *#* Revision 1.2  2004/06/03 11:27:09  bernie
 *#* Add dual-license information.
 *#*
 *#* Revision 1.1  2004/05/23 17:27:00  bernie
 *#* Import kern/ subdirectory.
 *#*/

#ifndef KERN_SIGNAL_H
#define KERN_SIGNAL_H

#include <cfg/compiler.h>


/* Fwd decl */
struct Process;

/* Inter-process Communication services */
sigmask_t sig_check(sigmask_t sigs);
void sig_signal(struct Process *proc, sigmask_t sig);
sigmask_t sig_wait(sigmask_t sigs);


/**
 * \name Signal definitions
 * \{
 */
#define SIG_USER0    BV(0)  /**< Free for user usage */
#define SIG_USER1    BV(1)  /**< Free for user usage */
#define SIG_USER2    BV(2)  /**< Free for user usage */
#define SIG_USER3    BV(3)  /**< Free for user usage */
#define SIG_SYSTEM4  BV(4)  /**< Reserved for system use */
#define SIG_SYSTEM5  BV(5)  /**< Reserved for system use */
#define SIG_SYSTEM6  BV(6)  /**< Reserved for system use */
#define SIG_SINGLE   BV(7)  /**< Used to wait for a single event */
/*\}*/

#endif /* KERN_SIGNAL_H */

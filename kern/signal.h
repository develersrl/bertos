/**
 * \file
 * <!--
 * This file is part of BeRTOS.
 *
 * Bertos is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * As a special exception, you may use this file as part of a free software
 * library without restriction.  Specifically, if other files instantiate
 * templates or use macros or inline functions from this file, or you compile
 * this file and link it with other files to produce an executable, this
 * file does not by itself cause the resulting executable to be covered by
 * the GNU General Public License.  This exception does not however
 * invalidate any other reasons why the executable file might be covered by
 * the GNU General Public License.
 *
 * Copyright 2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 1999,2000,2001 Bernardo Innocenti <bernie@develer.com>
 *
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

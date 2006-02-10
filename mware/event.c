/*!
 * \file
 * <!--
 * Copyright 2005 Develer S.r.l. (http://www.develer.com/)
 * This file is part of DevLib - See README.devlib for information.
 * -->
 *
 * \brief Events handling implementation
 *
 * \version $Id$
 *
 * \author Giovanni Bajo <rasky@develer.com>
 */

/*#*
 *#* $Log$
 *#* Revision 1.2  2006/02/10 12:24:42  bernie
 *#* Fix standalone build.
 *#*
 *#* Revision 1.1  2005/11/27 01:39:48  bernie
 *#* Move event.[ch] from kern/ to mware/.
 *#*
 *#* Revision 1.2  2005/11/04 16:20:02  bernie
 *#* Fix reference to README.devlib in header.
 *#*
 *#* Revision 1.1  2005/01/24 04:19:06  bernie
 *#* Function pointer based event dispatching.
 *#*
 *#* Revision 1.1  2005/01/18 19:39:42  rasky
 *#* Cambiata la logica di dispatching degli eventi: si utilizza puntatori a funzione invece di inlinare i controlli
 *#*
 *#*/

#include "event.h"

void event_hook_ignore(UNUSED_ARG(Event *, e))
{
}

#if defined(CONFIG_KERN_SIGNALS) && CONFIG_KERN_SIGNALS
void event_hook_signal(Event *e)
{
	sig_signal((e)->Ev.Sig.sig_proc, (e)->Ev.Sig.sig_bit);
}
#endif

void event_hook_softint(Event *e)
{
	e->Ev.Int.func(e->Ev.Int.user_data);
}

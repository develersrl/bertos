/*!
 * \file
 * <!--
 * Copyright 2005 Develer S.r.l. (http://www.develer.com/)
 * This file is part of DevLib - See devlib/README for information.
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
 *#* Revision 1.1  2005/01/24 04:19:06  bernie
 *#* Function pointer based event dispatching.
 *#*
 *#* Revision 1.1  2005/01/18 19:39:42  rasky
 *#* Cambiata la logica di dispatching degli eventi: si utilizza puntatori a funzione invece di inlinare i controlli
 *#*
 *#*/

#include "event.h"

void event_hook_ignore(Event *e)
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

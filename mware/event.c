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
 *#* Revision 1.3  2006/07/19 12:56:27  bernie
 *#* Convert to new Doxygen style.
 *#*
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

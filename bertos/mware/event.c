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
 *
 * -->
 *
 * \brief Events handling implementation
 *
 *
 * \author Giovanni Bajo <rasky@develer.com>
 */


#include "event.h"
#include "cfg/cfg_signal.h"
#include "cfg/cfg_timer.h"

void event_hook_ignore(UNUSED_ARG(Event *, e))
{
}

#if CONFIG_KERN_SIGNALS
void event_hook_signal(Event *e)
{
	sig_post((e)->Ev.Sig.sig_proc, (e)->Ev.Sig.sig_bit);
}
#endif

void event_hook_softint(Event *e)
{
	e->Ev.Int.func(e->Ev.Int.user_data);
}

void event_hook_generic(Event *e)
{
	e->Ev.Gen.completed = true;
	MEMORY_BARRIER;
}

#if CONFIG_TIMER_EVENTS
void event_hook_generic_timeout(Event *e)
{
	e->Ev.Gen.completed = true;
	MEMORY_BARRIER;
}
#endif

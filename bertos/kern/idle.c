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
 * Copyright 2008 Bernie Innocenti <bernie@codewiz.org>
 * -->
 *
 * \brief Idle loop for preemptive scheduling
 *
 * \version $Id: proc.c 1616 2008-08-10 19:41:26Z bernie $
 * \author Bernie Innocenti <bernie@codewiz.org>
 */

#include "proc.h"

#include <cfg/module.h>


static cpustack_t idle_stack[CONFIG_PROC_DEFSTACKSIZE / sizeof(cpustack_t)];

/**
 * The idle process
 *
 * This process never dies and never sleeps.  It's also quite lazy, apathic
 * and sometimes even a little antisocial.
 *
 * Having an idle process costs us some stack space, but simplifies the
 * interrupt-driven preemption logic because there is always a user
 * context to which we can return.
 *
 * The idle process is not required for cooperative task switching.
 */
static NORETURN void idle(void)
{
	for (;;)
	{
		TRACE;
		//monitor_report();
		proc_yield(); // FIXME: CPU_IDLE
	}
}

void idle_init(void)
{
	proc_new(idle, NULL, sizeof(idle_stack), idle_stack);
}

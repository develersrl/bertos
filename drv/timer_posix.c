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
 * Copyright 2005,2008 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \version $Id$
 *
 * \author Bernardo Innocenti <bernie@develer.com>
 *
 * \brief Low-level timer module for Qt emulator (implementation).
 */
#include <cfg/compiler.h> // hptime.t
#include <os/hptime.h>

#include <signal.h>       // sigaction()
#include <sys/time.h>     // setitimer()
#include <string.h>       // memset()


// Forward declaration for the user interrupt server routine.
void timer_isr(int);

/// HW dependent timer initialization.
static void timer_hw_init(void)
{
	struct sigaction sa;
	memset(&sa, 0, sizeof(sa));

	// Setup interrupt callback
	sa.sa_handler = timer_isr;
	sigemptyset(&sa.sa_mask);
	sigaddset(&sa.sa_mask, SIGALRM);
	sa.sa_flags = SA_RESTART;
	sigaction(SIGALRM, &sa, NULL);

	// Setup POSIX realtime timer to interrupt every 1/TIMER_TICKS_PER_SEC.
	static struct itimerval itv =
	{
		{ 0, 1000000 / TIMER_TICKS_PER_SEC }, /* it_interval */
		{ 0, 1000000 / TIMER_TICKS_PER_SEC }  /* it_value */
	};
	setitimer(ITIMER_REAL, &itv, NULL);
}

INLINE hptime_t timer_hw_hpread(void)
{
	return hptime_get();
}

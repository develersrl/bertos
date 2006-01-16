/*!
 * \file
 * <!--
 * Copyright 2005 Develer S.r.l. (http://www.develer.com/)
 * This file is part of DevLib - See README.devlib for information.
 * -->
 *
 * \version $Id$
 *
 * \author Bernardo Innocenti <bernie@develer.com>
 *
 * \brief Low-level timer module for Qt emulator (implementation).
 */

/*#*
 *#* $Log$
 *#* Revision 1.2  2006/01/16 03:30:21  bernie
 *#* Make header C++ friendly.
 *#*
 *#* Revision 1.1  2005/11/27 03:58:18  bernie
 *#* Add POSIX timer emulator.
 *#*
 *#* Revision 1.1  2005/11/27 03:06:36  bernie
 *#* Qt timer emulation.
 *#*
 *#*/

#include <cfg/compiler.h> // hptime.t
#include <os/hptime.h>

#include <signal.h>       // sigaction()
#include <sys/time.h>     // setitimer()
#include <string.h>       // memset()


// Forward declaration for the user interrupt server routine.
void timer_isr(int);

/// HW dependent timer initialization.
EXTERN_C static void timer_hw_init(void)
{
	struct sigaction sa;
	memset(&sa, 0, sizeof(sa));

	// Setup interrupt callback
	sa.sa_handler = timer_isr;
	sigemptyset(&sa.sa_mask);
	sigaddset(&sa.sa_mask, SIGALRM);
	sa.sa_flags = SA_RESTART;
	sigaction(SIGALRM, &sa, NULL);

	// Setup POSIX realtime timer to interrupt every 10ms.
	static struct itimerval itv =
	{
		{ 0, 1000 }, /* it_interval */
		{ 0, 1000 }  /* it_value */
	};
	setitimer(ITIMER_REAL, &itv, NULL);
}

INLINE hptime_t timer_hw_hpread(void)
{
	return hptime_get();
}


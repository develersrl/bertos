/**
 * \file
 * <!--
 * Copyright 2003, 2004 Develer S.r.l. (http://www.develer.com/)
 * This file is part of DevLib - See README.devlib for information.
 * -->
 *
 * \brief Portable abstraction for high-resolution time handling (implementation)
 *
 * \version $Id$
 * \author Bernardo Innocenti <bernie@develer.com>
 */

#include "hptime.h"

#if defined(_WIN32)

#include <windows.h>

hptime_t hptime_get(void)
{
	FILETIME ft;

	/*
	 * La precisione dei FileTime sarebbe 100ns, ma il
	 * valore viene ottenuto convertendo una struttura
	 * SYSTEMTIME, che ha precisione di 1ms. Il numero
	 * che otteniamo e' quindi sempre un multiplo di
	 * 100000.
	 */
	GetSystemTimeAsFileTime(&ft);

	/* Copy the upper/lower into a quadword. */
	return (((hptime_t)ft.dwHighDateTime) << 32) + (hptime_t)ft.dwLowDateTime;
}

#elif defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))

#include <sys/time.h> /* for gettimeofday() */
#include <stddef.h> /* for NULL */

hptime_t hptime_get(void)
{
	struct timeval tv;

	gettimeofday(&tv, NULL);
	return (hptime_t)tv.tv_sec * HPTIME_TICKS_PER_SECOND
		+ (hptime_t)tv.tv_usec;
}

#else /* !__unix__ */
	#error OS dependent support code missing for this OS
#endif /* !__unix__ */


/*!
 * \file
 * <!--
 * Copyright 2003, 2004 Develer S.r.l. (http://www.develer.com/)
 * This file is part of DevLib - See devlib/README for information.
 * -->
 *
 * \brief Portable abstraction for high-resolution time handling (implementation)
 *
 * \version $Id$
 * \author Bernardo Innocenti <bernie@develer.com>
 */

/*#*
 *#* $Log$
 *#* Revision 1.4  2004/08/25 14:12:09  rasky
 *#* Aggiornato il comment block dei log RCS
 *#*
 *#* Revision 1.3  2004/08/10 05:45:04  bernie
 *#* Fix spacing in header.
 *#*
 *#* Revision 1.2  2004/06/03 11:27:09  bernie
 *#* Add dual-license information.
 *#*
 *#* Revision 1.1  2004/06/03 09:01:06  bernie
 *#* Import into DevLib.
 *#*
 *#*/

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

#elif defined(__unix__)

#include <sys/time.h>

hptime_t hptime_get(void)
{
	struct timeval tv;

	gettimeofday(&tv, 0);
	return (hptime_t)tv.tv_sec * HPTIME_TICKS_PER_SECOND
		+ (hptime_t)tv.tv_usec;
}

#else /* !__unix__ */
	#error OS dependent support code missing for this OS
#endif /* !__unix__ */


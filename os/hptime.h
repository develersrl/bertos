/*!
 * \file
 * <!--
 * Copyright 2003,2004 Develer S.r.l. (http://www.develer.com/)
 * This file is part of DevLib - See devlib/README for information.
 * -->
 *
 * \brief Portable abstraction for high-resolution time handling (interface)
 *
 * \version $Id$
 * \author Bernardo Innocenti <bernie@develer.com>
 */

/*
 * $Log$
 * Revision 1.2  2004/06/03 11:27:09  bernie
 * Add dual-license information.
 *
 * Revision 1.1  2004/06/03 09:01:06  bernie
 * Import into DevLib.
 *
 */
#ifndef HPTIME_H
#define HPTIME_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef _WIN32

	/* type for time expressed in ms */
	typedef unsigned long mtime_t;

	/* our type for "high precision absolute time" */
	typedef unsigned __int64 hptime_t;

	#define HPTIME_TICKS_PER_SECOND		((hptime_t)10000000I64)
	#define HPTIME_TICKS_PER_MILLISEC	((hptime_t)10000I64)
	#define HPTIME_TICKS_PER_MICRO		((hptime_t)10I64)

#elif defined(__unix__)

	/* type for time expressed in ms */
	typedef long mtime_t;

	/* our type for "high precision absolute time" */
	typedef long long hptime_t;

	#define HPTIME_TICKS_PER_SECOND		((hptime_t)1000000LL)
	#define HPTIME_TICKS_PER_MILLISEC	((hptime_t)1000LL)
	#define HPTIME_TICKS_PER_MICRO		((hptime_t)1LL)

#else /* !__unix__ */
	#error OS dependent support code missing for this OS
#endif /* !__unix__ */

/*!
 * Return the current time with the maximum precision made available from the hosting OS
 */
extern hptime_t hptime_get(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* HPTIME_H */

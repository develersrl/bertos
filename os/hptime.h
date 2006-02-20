/*!
 * \file
 * <!--
 * Copyright 2003, 2004 Develer S.r.l. (http://www.develer.com/)
 * This file is part of DevLib - See README.devlib for information.
 * -->
 *
 * \brief Portable abstraction for high-resolution time handling (interface)
 *
 * \version $Id$
 * \author Bernardo Innocenti <bernie@develer.com>
 */

/*#*
 *#* $Log$
 *#* Revision 1.8  2006/02/20 01:46:46  bernie
 *#* Port to MacOSX.
 *#*
 *#* Revision 1.7  2006/01/16 03:28:25  bernie
 *#* Remove redundant mtime_t definition (cfg/compiler.h has one already).
 *#*
 *#* Revision 1.6  2005/11/27 03:57:22  bernie
 *#* Use C99 types to match cfg/compiler.h without depending on it.
 *#*
 *#* Revision 1.5  2005/11/04 16:20:02  bernie
 *#* Fix reference to README.devlib in header.
 *#*
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
#ifndef HPTIME_H
#define HPTIME_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef _WIN32

	/** our type for "high precision absolute time" */
	typedef __int64 hptime_t;

	#define HPTIME_TICKS_PER_SECOND		((hptime_t)10000000I64)
	#define HPTIME_TICKS_PER_MILLISEC	((hptime_t)10000I64)
	#define HPTIME_TICKS_PER_MICRO		((hptime_t)10I64)

#elif defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))

	#include <stdint.h>

	/** our type for "high precision absolute time" */
	typedef int64_t hptime_t;

	#define HPTIME_TICKS_PER_SECOND		1000000LL
	#define HPTIME_TICKS_PER_MILLISEC	1000LL
	#define HPTIME_TICKS_PER_MICRO		1LL

#else /* !__unix__ */
	#error OS dependent support code missing for this OS
#endif /* !__unix__ */

/**
 * Return the current time with the maximum precision made available from the hosting OS
 */
extern hptime_t hptime_get(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* HPTIME_H */

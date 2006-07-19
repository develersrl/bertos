/**
 * \file
 * <!--
 * Copyright 2005 Develer S.r.l. (http://www.develer.com/)
 * This file is part of DevLib - See README.devlib for information.
 * -->
 *
 * \brief Poor man's hex arrays (implementation).
 *
 * \version $Id$
 * \author Bernardo Innocenti <bernie@develer.com>
 */

/*#*
 *#* $Log$
 *#* Revision 1.3  2006/07/19 12:56:28  bernie
 *#* Convert to new Doxygen style.
 *#*
 *#* Revision 1.2  2005/04/11 19:10:28  bernie
 *#* Include top-level headers from cfg/ subdir.
 *#*
 *#* Revision 1.1  2005/03/15 00:06:30  bernie
 *#* Simpler, smaller, faster.
 *#*
 *#*/

#ifndef MWARE_STRTOL10_H
#define MWARE_STRTOL10_H

#include <cfg/compiler.h> /* bool */

bool strtoul10(const char *first, const char *last, unsigned long *val);
bool strtol10(const char *first, const char *last, long *val);

/**
 * Replacement for standard library function atol().
 */
INLINE long atol(const char *str)
{
	long val;
	strtol10(str, NULL, &val);
	return val;
}

/**
 * Replacement for standard library function atoi().
 */
INLINE int atoi(const char *str)
{
	return (int)atol(str);
}

#endif /* MWARE_STRTOL10_H */

/*!
 * \file
 * <!--
 * Copyright (C) 2002,2004 Develer S.r.l. (http://www.develer.com/)
 * All Rights Reserved.
 * -->
 *
 * \version $Id$
 *
 * \author Bernardo Innocenti <bernie@develer.com>
 *
 * \brief Simple sprintf() implementation based on _formatted_write()
 */

/*
 * $Log$
 * Revision 1.1  2004/05/23 15:43:16  bernie
 * Import mware modules.
 *
 * Revision 1.4  2004/04/03 20:42:27  aleph
 * Remove duplicated defines
 *
 * Revision 1.3  2004/03/24 15:48:53  bernie
 * Remove Copyright messages from Doxygen output
 *
 * Revision 1.2  2004/03/19 16:51:30  bernie
 * Add PROGMEM kludge.
 *
 * Revision 1.1  2004/02/23 09:45:09  aleph
 * Add missing library functions.
 *
 * Revision 1.1  2003/11/13 16:56:37  aleph
 * Add first implementation of dsp firmware
 *
 */

#include "compiler.h"
#include "formatwr.h"
#include <stdio.h>


static void __str_put_char(char c, void *ptr)
{
	/*
	 * This Does not work on Code Warrior. Hmm...
	 *	*(*((char **)ptr))++ = c;
	 */

	**((char **)ptr) = c;
	(*((char **)ptr))++;
}

static void __null_put_char(UNUSED(char c), UNUSED(void *ptr))
{
	/* nop */
}


int PGM_FUNC(vsprintf)(char * str, const char * PGM_ATTR fmt, va_list ap)
{
	int result;

	result = PGM_FUNC(_formatted_write)(
			fmt, (str ? __str_put_char : __null_put_char), &str, ap);

	/* terminate string */
	*str = '\0';

	return result;
}

/* FIXME: sprintf_P is incorrectly declared in <stdio.h> */
int PGM_FUNC(sprintf)(char *str, const char * fmt, ...)
{
	int result;
	va_list ap;

	va_start(ap, fmt);
	result = PGM_FUNC(vsprintf)(str, fmt, ap);
	va_end(ap);

	/* terminate string */
	*str = '\0';

	return result;
}

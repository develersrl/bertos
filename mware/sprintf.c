/*!
 * \file
 * <!--
 * Copyright (C) 2002,2004 Develer S.r.l. (http://www.develer.com/)
 * This file is part of DevLib - See devlib/README for information.
 * -->
 *
 * \brief sprintf() implementation based on _formatted_write()
 *
 * \version $Id$
 * \author Bernardo Innocenti <bernie@develer.com>
 */

/*#*
 *#* $Log$
 *#* Revision 1.7  2004/12/31 17:47:45  bernie
 *#* Rename UNUSED() to UNUSED_ARG().
 *#*
 *#* Revision 1.6  2004/11/16 21:15:19  bernie
 *#* Fix off-by-one bug in [v]snprintf().
 *#*
 *#* Revision 1.5  2004/10/03 18:54:36  bernie
 *#* sprintf(): Fix a serious bug; snprintf(): New function.
 *#*
 *#* Revision 1.4  2004/08/25 14:12:09  rasky
 *#* Aggiornato il comment block dei log RCS
 *#*
 *#* Revision 1.3  2004/06/27 15:20:26  aleph
 *#* Change UNUSED() macro to accept two arguments: type and name;
 *#* Add macro GNUC_PREREQ to detect GCC version during build;
 *#* Some spacing cleanups and typo fix
 *#*
 *#* Revision 1.2  2004/06/03 11:27:09  bernie
 *#* Add dual-license information.
 *#*/

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

static void __null_put_char(UNUSED_ARG(char, c), UNUSED_ARG(void *, ptr))
{
	/* nop */
}


int PGM_FUNC(vsprintf)(char * str, const char * PGM_ATTR fmt, va_list ap)
{
	int result;

	result = PGM_FUNC(_formatted_write)(
			fmt, (str ? __str_put_char : __null_put_char), &str, ap);

	/* Terminate string */
	*str = '\0';

	return result;
}


int PGM_FUNC(sprintf)(char *str, const char * fmt, ...)
{
	int result;
	va_list ap;

	va_start(ap, fmt);
	result = PGM_FUNC(vsprintf)(str, fmt, ap);
	va_end(ap);

	return result;
}

/*!
 * State information for __sn_put_char()
 */
struct __sn_state
{
	char *str;
	size_t len;
};

/*!
 * formatted_write() callback used [v]snprintf().
 */
static void __sn_put_char(char c, void *ptr)
{
	struct __sn_state *state = (struct __sn_state *)ptr;

	if (state->len)
	{
		--state->len;
		*state->str++ = c;
	}
}


int PGM_FUNC(vsnprintf)(char *str, size_t size, const char * PGM_ATTR fmt, va_list ap)
{
	int result = 0;

	/* Make room for traling '\0'. */
	if (size--)
	{
		struct __sn_state state;
		state.str = str;
		state.len = size;

		result = PGM_FUNC(_formatted_write)(
			fmt, (str ? __sn_put_char : __null_put_char), &state, ap);

		/* Terminate string. */
		*state.str = '\0';
	}

	return result;
}


int PGM_FUNC(snprintf)(char *str, size_t size, const char * fmt, ...)
{
	int result;
	va_list ap;

	va_start(ap, fmt);
	result = PGM_FUNC(vsnprintf)(str, size, fmt, ap);
	va_end(ap);

	return result;
}

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
 *#*
 *#* Revision 1.1  2004/05/23 15:43:16  bernie
 *#* Import mware modules.
 *#*
 *#* Revision 1.4  2004/04/03 20:42:27  aleph
 *#* Remove duplicated defines
 *#*
 *#* Revision 1.3  2004/03/24 15:48:53  bernie
 *#* Remove Copyright messages from Doxygen output
 *#*
 *#* Revision 1.2  2004/03/19 16:51:30  bernie
 *#* Add PROGMEM kludge.
 *#*
 *#* Revision 1.1  2004/02/23 09:45:09  aleph
 *#* Add missing library functions.
 *#*
 *#* Revision 1.1  2003/11/13 16:56:37  aleph
 *#* Add first implementation of dsp firmware
 *#*
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

static void __null_put_char(UNUSED(char, c), UNUSED(void *, ptr))
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

	if (state->len > 0)
	{
		--state->len;
		*state->str++ = c;
	}
}


int PGM_FUNC(vsnprintf)(char * str, size_t size, const char * PGM_ATTR fmt, va_list ap)
{
	int result;
	struct __sn_state state = { str, size };

	result = PGM_FUNC(_formatted_write)(
			fmt, (str ? __sn_put_char : __null_put_char), &state, ap);

	/* Terminate string */
	*state.str = '\0';

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

/**
 * \file
 * <!--
 * Copyright 2003, 2005 Develer S.r.l. (http://www.develer.com/)
 * This file is part of DevLib - See README.devlib for information.
 * -->
 *
 * \brief Definitions of constant string messages.
 *
 * \version $Id$
 *
 * \author Bernardo Innocenti <bernie@develer.com>
 * \author Stefano Fedrigo <aleph@develer.com>
 */

/*#*
 *#* $Log$
 *#* Revision 1.1  2005/03/01 23:31:02  bernie
 *#* String table and localization stuff.
 *#*
 *#*/
#ifndef MWARE_MSG_H
#define MWARE_MSG_H

enum
{
	MSG_NULL,

	// TODO: add your labels here.

	MSG_COUNT
};


#define MSG_BUFSIZE 6144 /* FIXME: how much? */

/* String tables */
extern const char *msg_strings[MSG_COUNT];
/* extern char msg_buf[MSG_BUFSIZE]; */


/* Macros to access translated messages */
#define MSG(x)  msg_strings[x]
#define PTRMSG(x)  ((unsigned int)(x) < 256 ? msg_strings[(unsigned int)(x)] : (const char *)(x))


#endif /* MWARE_MSG_H */

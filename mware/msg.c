/*!
 * \file
 * <!--
 * Copyright 2003, 2005 Develer S.r.l. (http://www.develer.com/)
 * This file is part of DevLib - See README.devlib for information.
 * -->
 *
 * \brief Messages for LCD.
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

#include "msg.h"

/*!
 * Array of pointers to localized strings. Should be filled
 * by localization stuff, but not for now.
 */
const char *msg_strings[MSG_COUNT] = {
	0,
	// TODO: add your strings here
};

/* Buffer for catalog file */
/* char msg_buf[MSG_BUFSIZE]; */


/* The following does not work (move string tables into the DMSG/CMSG segments)
 * #pragma memory=dataseg(DMSG)
 * #pragma memory=constseg(CMSG)
 */


/*!
 * Untranslated constant strings used more than once are
 * grouped here to save ROM space.
 */
const char
	str_empty[] = "";


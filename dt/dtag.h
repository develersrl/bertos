/**
 * \file
 * <!--
 * Copyright 2005 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 * \version $Id$
 *
 * \brief Tags interface.
 * This module contains the base message definitions and the list of all available tags.
 *
 * \version $Id$
 * \author Bernardo Innocenti <bernie@develer.com>
 * \author Francesco Sacchi <batt@develer.com>
 */

/*#*
 *#* $Log$
 *#* Revision 1.2  2006/07/19 12:56:26  bernie
 *#* Convert to new Doxygen style.
 *#*
 *#* Revision 1.1  2005/11/04 18:26:38  bernie
 *#* Import into DevLib.
 *#*
 *#* Revision 1.8  2005/06/07 15:22:29  batt
 *#* Add const_dval_t.
 *#*
 *#* Revision 1.7  2005/06/06 17:42:23  batt
 *#* Add error tag TAG_ERROR.
 *#*
 *#* Revision 1.6  2005/06/06 12:45:33  batt
 *#* Add TAG_NONE tag.
 *#*
 *#* Revision 1.5  2005/06/06 11:04:12  batt
 *#* Add some comments.
 *#*
 *#* Revision 1.4  2005/05/31 11:09:52  batt
 *#* Add some tags.
 *#*
 *#* Revision 1.3  2005/05/26 14:55:12  batt
 *#* Add form_processTime; change form_kbdProcess to form_processKey.
 *#*
 *#* Revision 1.2  2005/05/26 14:43:33  batt
 *#* Add new message filter interface.
 *#*
 *#* Revision 1.1  2005/05/26 08:32:53  batt
 *#* Add new Develer widget system :)
 *#*
 *#*/

#ifndef DT_DTAG_H
#define DT_DTAG_H

#include <cfg/macros.h>

/** Type for values associated with tags. */
typedef iptr_t dval_t;

/** Type for constant values associated with tags. */
typedef const_iptr_t const_dval_t;

/** Type for tag */
typedef enum dtag_t
{
	TAG_END = 0,
	TAG_NONE,
	TAG_ANY,
	TAG_SETVALUE,
	TAG_UP,
	TAG_DOWN,
	TAG_START,
	TAG_STOP,
	TAG_TOGGLE,
	TAG_KEY,
	TAG_TIME,
	TAG_ERROR,
} dtag_t;

/** This is the basic message structure used by all dnotifiers. */
typedef struct DTagItem
{
	dtag_t tag;
	dval_t val;
} DTagItem;

/** Marker to indicate the end of a map for message filtering/translating */
#define TAG_END_MARKER {{TAG_END, 0}, {TAG_END, 0}}
#endif

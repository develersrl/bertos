/*!
 * \file
 * <!--
 * Copyright 2003, 2004, 2005, 2006 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2000 Bernardo Innocenti
 * All Rights Reserved.
 * -->
 *
 * \brief Keyboard hardware-specific definitions
 *
 * \version $Id$
 *
 * \author Francesco Sacchi <batt@develer.com>
 * \author Stefano Fedrigo <a@develer.com>
 */

/*#*
 *#* $Log$
 *#* Revision 1.1  2006/02/10 12:31:03  bernie
 *#* Add templates for hw definitions.
 *#*
 *#*/

#ifndef HW_KBD_H
#define HW_KBD_H


#include "kbd_map.h"
#include <cfg/macros.h>
#include <cfg/debug.h>


 /*!
 * \name CPU ports <-> switches assignement
 * @{
 */

#define K_RPT_MASK (K_UP | K_DOWN | K_OK | K_CANCEL)
/*@}*/


#define KBD_HW_INIT \
	do { \
	} while (0)


/*!
 * Read the keyboard ports and return the mask of
 * depressed keys.
 */
INLINE keymask_t kbd_readkeys(void)
{
	keymask_t key = 0;

	uint32_t mask = 0xF;

	if (!(mask & 1))
		key |= K_OK;
	if (!(mask & 2))
		key |= K_CANCEL;
	if (!(mask & 4))
		key |= K_UP;
	if (!(mask & 8))
		key |= K_DOWN;

	return key;
}

#endif /* HW_KBD_H */

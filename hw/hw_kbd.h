/**
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
 *#* Revision 1.4  2006/07/19 12:56:27  bernie
 *#* Convert to new Doxygen style.
 *#*
 *#* Revision 1.3  2006/03/22 09:51:27  bernie
 *#* Wrap C++ stuff.
 *#*
 *#* Revision 1.2  2006/02/15 09:13:42  bernie
 *#* Add keyboard emulator.
 *#*
 *#* Revision 1.1  2006/02/10 12:31:03  bernie
 *#* Add templates for hw definitions.
 *#*
 *#*/

#ifndef HW_KBD_H
#define HW_KBD_H

#include "kbd_map.h"
#include <cfg/macros.h>

#define K_RPT_MASK (K_UP | K_DOWN | K_OK | K_CANCEL)

#define KBD_HW_INIT \
	do { \
	} while (0)

EXTERN_C int emul_kbdReadCols(void);

/**
 * Read the keyboard ports and return the mask of
 * depressed keys.
 */
INLINE keymask_t kbd_readkeys(void)
{
	return (keymask_t)emul_kbdReadCols();
}

#endif /* HW_KBD_H */

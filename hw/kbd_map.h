/**
 * \file
 * <!--
 * Copyright 2003, 2004, 2005, 2006 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2000 Bernardo Innocenti
 * All Rights Reserved.
 * -->
 *
 * \brief Keyboard map definitions.
 *
 * \version $Id$
 *
 * \author Francesco Sacchi <batt@develer.com>
 * \author Stefano Fedrigo <a@develer.com>
 */

/*#*
 *#* $Log$
 *#* Revision 1.2  2006/07/19 12:56:27  bernie
 *#* Convert to new Doxygen style.
 *#*
 *#* Revision 1.1  2006/02/10 12:31:03  bernie
 *#* Add templates for hw definitions.
 *#*
 *#*/

#ifndef HW_KBD_MAP_H
#define HW_KBD_MAP_H

#include <cfg/macros.h>


/**
 * Type for keyboard mask.
 */
typedef uint16_t keymask_t;

/**
 * \name Keycodes.
 */
/*@{*/
#define K_UP       BV(0)
#define K_DOWN     BV(1)
#define K_OK       BV(2)
#define K_CANCEL   BV(3)

#define K_REPEAT   BV(14) /**< This is a repeated keyevent. */
#define K_TIMEOUT  BV(15) /**< Fake key event for timeouts. */
/*@}*/


#endif /* HW_KBD_MAP_H */

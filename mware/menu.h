/*!
 * \file
 * <!--
 * Copyright 2003, 2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2000 Bernardo Innocenti <bernie@codewiz.org>
 * All Rights Reserved.
 * -->
 *
 * \version $Id$
 *
 * \author Bernardo Innocenti <bernie@develer.com>
 * \author Stefano Fedrigo <aleph@develer.com>
 *
 * \brief Common menu handling API
 */

/*#*
 *#* $Log$
 *#* Revision 1.3  2006/03/22 09:49:51  bernie
 *#* Simplifications from project_grl.
 *#*
 *#* Revision 1.2  2006/03/20 17:48:35  bernie
 *#* Implement support for ROM menus.
 *#*
 *#* Revision 1.1  2006/02/10 12:29:36  bernie
 *#* Add menu system.
 *#*
 *#* Revision 1.20  2005/11/16 18:10:19  bernie
 *#* Move top-level headers to cfg/ as in DevLib.
 *#*
 *#* Revision 1.19  2005/02/11 19:11:38  aleph
 *#* Move menu_displaymsg() in new displaymsg module
 *#*
 *#* Revision 1.18  2005/01/13 16:56:36  aleph
 *#* Fix progmem includes.
 *#*
 *#* Revision 1.17  2004/12/14 12:52:45  aleph
 *#* Add exclude menu flags
 *#*
 *#* Revision 1.16  2004/10/01 14:04:59  customer_pw
 *#* Add accessor functions for menu flags
 *#*
 *#* Revision 1.15  2004/09/09 08:31:36  customer_pw
 *#* Add disabled item type
 *#*
 *#* Revision 1.14  2004/08/29 21:46:12  bernie
 *#* CVSSILENT: Mark CVS log blocks.
 *#*
 *#* Revision 1.13  2004/08/25 15:35:23  customer_pw
 *#* IPTR -> iptr_t conversion.
 *#*
 *#* Revision 1.12  2004/08/25 13:23:45  bernie
 *#* IPTR -> iptr_t conversion.
 *#*/
#ifndef MWARE_MENU_H
#define MWARE_MENU_H

#include <cfg/compiler.h>

/* Fwd decl */
struct Bitmap;

/** Menu callback function */
typedef void (*MenuHook)(iptr_t userdata);

/**
 * Menu item description.
 */
typedef struct MenuItem
{
	const_iptr_t label;    /*!< Item label (ID or ptr to string, 0 to disable) */
	int          flags;    /*!< See MIF_#? definitions below */
	MenuHook     hook;     /*!< Callback function (NULL to terminate item list) */
	iptr_t       userdata; /*!< User data to be passed back to the hook */
} MenuItem;

/**
 * \name Flags for MenuItem.flags.
 * \{
 */
#define MIF_EXCLUDE_MASK    0x00FF /**< Mask for mutual exclusion map (shared with priority). */
#define MIF_PRI_MASK        0x00FF /**< Mask for priority value (shared with mutual exclusion). */
#define MIF_PRI(x)          ((x) & MIF_PRI_MASK) /**< Set menu item priority. */
#define MIF_EXCLUDE_0       BV(0)  /*!< Exclude item 0 when this item is checked */
#define MIF_EXCLUDE_1       BV(1)  /*!< Exclude item 1 when this item is checked */
#define MIF_EXCLUDE_2       BV(2)  /*!< Exclude item 2 when this item is checked */
#define MIF_EXCLUDE_3       BV(3)  /*!< Exclude item 3 when this item is checked */
#define MIF_EXCLUDE_4       BV(4)  /*!< Exclude item 4 when this item is checked */
#define MIF_EXCLUDE_5       BV(5)  /*!< Exclude item 5 when this item is checked */
#define MIF_EXCLUDE_6       BV(6)  /*!< Exclude item 6 when this item is checked */
#define MIF_EXCLUDE_7       BV(7)  /*!< Exclude item 7 when this item is checked */
#define MIF_CHECKED         BV(8)  /*!< Item is currently checked */
#define MIF_CHECKIT         BV(9)  /*!< Automatically check this item when selected */
#define MIF_TOGGLE          BV(10) /*!< Toggle MIF_CHECKED when item is selected */
#define MIF_HIDDEN          BV(11) /*!< This menu item is not visible */
#define MIF_DISABLED        BV(12) /*!< This menu item is not visible */
#define MIF_RAMLABEL        BV(13) /*!< Item label is stored in RAM, not in program memory */
/* \} */

/**
 * Menu description.
 */
typedef struct Menu
{
	MenuItem        *items;    /*!< Array of items (end with a NULL hook) */
	const_iptr_t     title;    /*!< Menu title (ID or ptr to string, 0 to disable) */
	int              flags;    /*!< See MF_#? definitions below */
	struct Bitmap   *bitmap;   /*!< Bitmap where the menu is rendered */
	int              startrow; /*!< Display starting row */
} Menu;

/**
 * \name Flags for Menu.flags.
 * \{
 */
#define MF_STICKY    BV(0)  /*!< Stay in the menu when the items called return */
#define MF_TOPLEVEL  BV(1)  /*!< Top-level menu (do not display "back" label) */
#define MF_ROMITEMS  BV(2)  /*!< Menu is in ROM (default is RAM) */
/* \} */

/* Function prototypes */
iptr_t menu_handle(const struct Menu *menu);
int menu_setFlags(struct Menu *menu, int idx, int flags);
int menu_clearFlags(struct Menu *menu, int idx, int flags);

#endif /* MWARE_MENU_H */

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
 * \brief General pourpose menu handling functions
 */

/*#*
 *#* $Log$
 *#* Revision 1.2  2006/02/15 09:10:51  bernie
 *#* Make title bold; Fix height when we have no menubar.
 *#*
 *#* Revision 1.1  2006/02/10 12:29:36  bernie
 *#* Add menu system.
 *#*
 *#* Revision 1.48  2005/11/27 23:02:55  bernie
 *#* Move graphics modules from mware/ to gfx/.
 *#*
 *#* Revision 1.47  2005/11/16 18:10:19  bernie
 *#* Move top-level headers to cfg/ as in DevLib.
 *#*
 *#* Revision 1.46  2005/02/17 03:49:21  bernie
 *#* Update to new PGM api.
 *#*
 *#* Revision 1.45  2005/02/11 19:11:32  aleph
 *#* Move menu_displaymsg() in new displaymsg module
 *#*
 *#* Revision 1.44  2005/01/21 20:05:57  aleph
 *#* Fix build warning with debug off
 *#*
 *#* Revision 1.43  2005/01/13 16:56:36  aleph
 *#* Fix progmem includes.
 *#*
 *#* Revision 1.42  2004/10/31 11:02:15  aleph
 *#* Rename functions with correct codying conventions; Simplify version display
 *#*
 *#* Revision 1.41  2004/10/15 17:34:33  customer_pw
 *#* Fix menuitem max length
 *#*
 *#* Revision 1.40  2004/10/06 12:55:08  customer_pw
 *#* Declare unused (if !_DEBUG) menu_count()
 *#*
 *#* Revision 1.39  2004/10/01 14:04:59  customer_pw
 *#* Add accessor functions for menu flags
 *#*
 *#* Revision 1.38  2004/09/27 12:05:46  customer_pw
 *#* Use sel label for toggle menus and remove it
 *#*
 *#* Revision 1.37  2004/09/27 10:05:33  customer_pw
 *#* Menu cosmetic fixes
 *#*
 *#* Revision 1.36  2004/09/14 22:18:03  bernie
 *#* Adapt to stricter casting rules.
 *#*
 *#* Revision 1.35  2004/09/12 17:56:03  aleph
 *#* Include debug.h instead of drv/kdebug.h
 *#*/

#include "menu.h"

#if CONFIG_MENU_MENUBAR
#include "menubar.h"
#endif

#include <gfx/gfx.h>
#include <gfx/font.h>
#include <gfx/text.h>
#include <drv/kbd.h>
#include <cfg/compiler.h>
#include <cfg/debug.h>

#if CPU_HARVARD
#include <avr/pgmspace.h> /* strncpy_P() */
#endif

#include <string.h> /* strcpy() */

#if defined(CONFIG_LOCALE) && (CONFIG_LOCALE == 1)
#include "msg.h"
#else
#define PTRMSG(x) ((const char *)x)
#endif


/* Temporary fake defines for ABORT stuff... */
#define abort_top  0
#define PUSH_ABORT false
#define POP_ABORT  do {} while(0)
#define DO_ABORT   do {} while(0)


#ifdef _DEBUG
/*!
 * Count the items present in a menu.
 */
static int UNUSED_FUNC menu_count(const struct Menu *menu)
{
	int cnt = 0;
	struct MenuItem *item;

	for (item = menu->items; item->label; ++item)
		cnt++;

	return cnt;
}
#endif /* _DEBUG */


#if CONFIG_MENU_MENUBAR

/*!
 * Update the menu bar according to the selected item
 * and redraw it.
 */
static void menu_update_menubar(
		const struct Menu *menu,
		struct MenuBar *mb,
		int selected)
{
	int item_flags = menu->items[selected].flags;
	const_iptr_t newlabel = (const_iptr_t)LABEL_OK;

	if (item_flags & MIF_DISABLED)
		newlabel = (const_iptr_t)LABEL_EMPTY;
	else if (item_flags & MIF_TOGGLE)
		newlabel = (const_iptr_t)LABEL_SEL;
	else if (item_flags & MIF_CHECKIT)
	{
		newlabel = (item_flags & MIF_CHECKED) ?
			(const_iptr_t)LABEL_EMPTY : (const_iptr_t)LABEL_SEL;
	}

	mb->labels[3] = newlabel;
	mbar_draw(mb);
}
#endif /* CONFIG_MENU_MENUBAR */


/*!
 * Show a menu on the LCD display.
 */
static void menu_layout(
		const struct Menu *menu,
		int first_item,
		int items_per_page,
		int selected)
{
	const MenuItem *item;
	int ypos, cnt;
	const char * PROGMEM title = PTRMSG(menu->title);

	ypos = menu->startrow;

	if (title)
		text_xprintf(menu->bitmap, ypos++, 0, STYLEF_BOLD | TEXT_FILL, title);

	for (
		cnt = 0, item = &menu->items[first_item];
		cnt < items_per_page;
		++cnt, ++item)
	{
		/* Check for end of menu */
		if (!(item->label || item->hook))
			break;

		/* Only print visible items */
		if (!(item->flags & MIF_HIDDEN))
		{
#if CPU_HARVARD
			text_xprintf_P
#else
			text_xprintf
#endif
			(
				menu->bitmap, ypos++, 0,
				(first_item + cnt == selected) ? (STYLEF_INVERT | TEXT_FILL) : TEXT_FILL,
				(item->flags & MIF_RAMLABEL) ? PSTR("%s%s") : PSTR("%S%s"),
				PTRMSG(item->label),
				(item->flags & MIF_TOGGLE) ?
					( (item->flags & MIF_CHECKED) ? PSTR(":ON") : PSTR(":OFF") )
					: ( (item->flags & MIF_CHECKED) ? PSTR("\04") : PSTR("") )
			);
		}
	}
}


/*!
 * Handle menu item selection
 */
static void menu_doselect(const struct Menu *menu, struct MenuItem *item)
{
	/* Exclude other items */
	int mask, i;
	for (mask = item->flags & MIF_EXCLUDE_MASK, i = 0; mask; mask >>= 1, ++i)
	{
		if (mask & 1)
			menu->items[i].flags &= ~MIF_CHECKED;
	}

	if (item->flags & MIF_DISABLED)
		return;

	/* Handle checkable items */
	if (item->flags & MIF_TOGGLE)
		item->flags ^= MIF_CHECKED;
	else if (item->flags & MIF_CHECKIT)
		item->flags |= MIF_CHECKED;

	/* Handle items with callback hooks */
	if (item->hook)
	{
		/* Push a jmp buffer to abort the operation with the STOP key */
		if (!PUSH_ABORT)
		{
			item->hook(item->userdata);
			POP_ABORT;
		}
	}
}


/*!
 * Return the previous visible item (rolls back to the last item)
 */
static int menu_next_visible_item(const struct Menu *menu, int index, int total)
{
	do
	{
		if (++index >= total)
		   index = 0;
	}
	while (menu->items[index].flags & MIF_HIDDEN);

	return index;
}


/*!
 * Return the next visible item (rolls back to the first item)
 */
static int menu_prev_visible_item(const struct Menu *menu, int index, int total)
{
	do
	{
		if (--index < 0)
			index = total - 1;
	}
	while (menu->items[index].flags & MIF_HIDDEN);

	return index;
}


/*!
 * Handle a menu and invoke hook functions for the selected menu items.
 */
iptr_t menu_handle(const struct Menu *menu)
{
	uint8_t entries, visible_entries, items_per_page;
	uint8_t first_item, selected;

#if CONFIG_MENU_MENUBAR
	struct MenuBar mb;
	const_iptr_t labels[] =
	{
		(const_iptr_t)LABEL_BACK,
		(const_iptr_t)LABEL_UPARROW,
		(const_iptr_t)LABEL_DOWNARROW,
		(const_iptr_t)0
	};

	/*
	 * Initialize menu bar
	 */
	if (menu->flags & MF_TOPLEVEL)
		labels[0] = (const_iptr_t)LABEL_EMPTY;

	mbar_init(&mb, menu->bitmap, labels, countof(labels));
#endif /* CONFIG_MENU_MENUBAR */


	/* Compute total number of items in menu (entries) and
	 * the number of visible entries, which excludes items
	 * without a label.
	 */
	{
		struct MenuItem *item;

		entries = 0;
		visible_entries = 0;

		for (item = menu->items; (item->label || item->hook); ++item)
		{
			++entries;
			if (!(item->flags & MIF_HIDDEN))
				++visible_entries;
		}
	}

	items_per_page =
		(menu->bitmap->height / menu->bitmap->font->height)
		- menu->startrow
#if CONFIG_MENU_MENUBAR
		- 1 /* menu bar labels */
#endif
		- (menu->title ? 1 : 0);

	/* Selected item should be a visible entry */
	first_item = selected = menu_next_visible_item(menu, -1, entries);

	for(;;)
	{
		keymask_t key;

		/*
		 * Keep selected item visible
		 */
		while (selected < first_item)
			first_item = menu_prev_visible_item(menu, first_item, entries);
		while (selected >= first_item + items_per_page)
			first_item = menu_next_visible_item(menu, first_item, entries);

		/* Clear screen */
		text_clear(menu->bitmap);
		menu_layout(menu, first_item, items_per_page, selected);

#if CONFIG_MENU_MENUBAR
		menu_update_menubar(menu, &mb, selected);
#endif

		key = kbd_get();

		if (key & K_OK)
		{
			struct MenuItem *item = &(menu->items[selected]);
			menu_doselect(menu, item);

			/* Return userdata as result */
			if (!menu->flags & MF_STICKY)
				return item->userdata;
		}
		else if (key & K_UP)
		{
			selected = menu_prev_visible_item(menu, selected, entries);
		}
		else if (key & K_DOWN)
		{
			selected = menu_next_visible_item(menu, selected, entries);
		}
		else if (key & K_CANCEL && !(menu->flags & MF_TOPLEVEL))
		{
			return 0;
		}
	}
}


/*!
 * Set flags on a menuitem.
 *
 * \param menu  Menu owner of the item to change.
 * \param idx   Index of the menu item.
 * \param flags Bit mask of the flags to set.
 *
 * \return Old flags.
 */
int menu_setFlags(struct Menu *menu, int idx, int flags)
{
	ASSERT(idx < menu_count(menu));

	int old = menu->items[idx].flags;
	menu->items[idx].flags |= flags;
	return old;
}


/*!
 * Clear flags on a menuitem.
 *
 * \param menu  Menu owner of the item to change.
 * \param idx   Index of the menu item.
 * \param flags Bit mask of the flags to clear.
 *
 * \return Old flags.
 */
int menu_clearFlags(struct Menu *menu, int idx, int flags)
{
	ASSERT(idx < menu_count(menu));

	int old = menu->items[idx].flags;
	menu->items[idx].flags &= ~flags;
	return old;
}

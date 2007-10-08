/**
 * \file
 * <!--
 * Copyright 2003, 2004, 2006 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2000 Bernardo Innocenti <bernie@codewiz.org>
 * This file is part of DevLib - See README.devlib for information.
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
 *#* Revision 1.8  2006/09/13 13:58:32  bernie
 *#* Add RenderHook support.
 *#*
 *#* Revision 1.7  2006/08/01 12:22:46  bernie
 *#* Mention DevLib license.
 *#*
 *#* Revision 1.6  2006/07/19 12:56:27  bernie
 *#* Convert to new Doxygen style.
 *#*
 *#* Revision 1.5  2006/06/03 13:58:01  bernie
 *#* Fix recursive timeout and add exit status information.
 *#*
 *#* Revision 1.4  2006/06/02 12:26:18  bernie
 *#* Draw graphical checkmarks.
 *#*
 *#* Revision 1.3  2006/05/28 15:03:31  bernie
 *#* Avoid unnecessary rendering.
 *#*
 *#* Revision 1.2  2006/05/25 23:34:38  bernie
 *#* Implement menu timeouts.
 *#*
 *#* Revision 1.1  2006/05/15 07:20:54  bernie
 *#* Move menu to gui/.
 *#*
 *#* Revision 1.7  2006/04/27 05:39:24  bernie
 *#* Enhance text rendering to arbitrary x,y coords.
 *#*
 *#* Revision 1.6  2006/04/11 00:07:32  bernie
 *#* Implemenent MF_SAVESEL flag.
 *#*
 *#* Revision 1.5  2006/03/22 09:49:51  bernie
 *#* Simplifications from project_grl.
 *#*
 *#* Revision 1.4  2006/03/20 17:48:35  bernie
 *#* Implement support for ROM menus.
 *#*
 *#* Revision 1.3  2006/02/20 14:34:32  bernie
 *#* Include appconfig.h before using its definitions.
 *#*
 *#* Revision 1.2  2006/02/15 09:10:51  bernie
 *#* Make title bold; Fix height when we have no menubar.
 *#*/

#include "menu.h"

#include <gfx/gfx.h>
#include <gfx/font.h>
#include <gfx/text.h>
#include <drv/kbd.h>
#include <cfg/compiler.h>
#include <cfg/debug.h>
#include <appconfig.h>
#include <string.h> /* strcpy() */

#if CPU_HARVARD
#include <avr/pgmspace.h> /* strncpy_P() */
#endif

#if CONFIG_MENU_SMOOTH
#include <drv/lcd_gfx.h>
#endif

#if (CONFIG_MENU_TIMEOUT != 0)
#include <drv/timer.h>
#endif

#if CONFIG_MENU_MENUBAR
#include "menubar.h"
#endif

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


/**
 * Return the total number of items in in a menu.
 */
static int menu_count(const struct Menu *menu)
{
	int cnt = 0;

	for (cnt = 0; /*NOP*/; ++cnt)
	{
		const MenuItem *item = &menu->items[cnt];
#if CPU_HARVARD
		MenuItem ram_item;
		if (menu->flags & MF_ROMITEMS)
		{
			memcpy_P(&ram_item, item, sizeof(ram_item));
			item = &ram_item;
		}
#endif
		if (!(item->label || item->hook))
			break;
	}

	return cnt;
}

#if CONFIG_MENU_MENUBAR

/**
 * Update the menu bar according to the selected item and redraw it.
 */
static void menu_update_menubar(
		const struct Menu *menu,
		struct MenuBar *mb,
		int selected)
{
	int item_flags;
#if CPU_HARVARD
	if (menu->flags & MF_ROMITEMS)
	{
		ASSERT(sizeof(menu->items[selected].flags) == sizeof(int));
		item_flags = pgm_read_int(&menu->items[selected].flags);
	}
	else
#endif
		item_flags = menu->items[selected].flags;

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


static void menu_defaultRenderHook(struct Bitmap *bm, int ypos, bool selected, const struct MenuItem *item)
{
	if (item->flags & MIF_CHECKIT)
	{
		gfx_rectClear(bm, 0, ypos,
				bm->font->height, ypos + bm->font->height);

		if (item->flags & MIF_TOGGLE)
			gfx_rectDraw(bm, 2, ypos + 2,
					bm->font->height - 2, ypos + bm->font->height - 2);
		if (item->flags & MIF_CHECKED)
		{
			gfx_line(bm,
					3, ypos + 3,
					bm->font->height - 3, ypos + bm->font->height - 3);
			gfx_line(bm,
					bm->font->height - 3, ypos + 3,
					3, ypos + bm->font->height - 3);
		}
	}

#if CPU_HARVARD
	((item->flags & MIF_RAMLABEL) ? text_xyprintf : text_xyprintf_P)
#else
	text_xyprintf
#endif
	(
		bm, (item->flags & MIF_CHECKIT) ? bm->font->height : 0, ypos,
		selected ? (STYLEF_INVERT | TEXT_FILL) : TEXT_FILL,
		PTRMSG(item->label)
	);
}

/**
 * Show a menu on the display.
 */
static void menu_layout(
		const struct Menu *menu,
		int first_item,
		int selected,
		bool redraw)
{
	coord_t ypos;
	int i;
	const char * PROGMEM title = PTRMSG(menu->title);
	Bitmap *bm = menu->bitmap;

	ypos = bm->cr.ymin;

#if 0
	if (redraw)
	{
		/* Clear screen */
		text_clear(menu->bitmap);
	}
#endif

	if (title)
	{
		if (redraw)
			text_xyprintf(bm, 0, ypos, STYLEF_UNDERLINE | STYLEF_BOLD | TEXT_CENTER | TEXT_FILL, title);
		ypos += bm->font->height;
	}

#if CONFIG_MENU_SMOOTH
	static coord_t yoffset = 0;
	static int old_first_item = 0;
	static int speed;
	coord_t old_ymin = bm->cr.ymin;

	/* Clip drawing inside menu items area */
	gfx_setClipRect(bm,
		bm->cr.xmin, bm->cr.ymin + ypos,
		bm->cr.xmax, bm->cr.ymax);

	if (old_first_item != first_item)
	{
		/* Speed proportional to distance */
		speed = ABS(old_first_item - first_item) * 3;

		if (old_first_item > first_item)
		{
			yoffset += speed;
			if (yoffset > bm->font->height)
			{
					yoffset = 0;
					--old_first_item;
			}
		}
		else
		{
			yoffset -= speed;
			if (yoffset < -bm->font->height)
			{
					yoffset = 0;
					++old_first_item;
			}
		}
		first_item = MIN(old_first_item, menu_count(menu));

		ypos += yoffset;
		redraw = true;
	}
#endif /* CONFIG_MENU_SMOOTH */

	if (redraw) for (i = first_item; /**/; ++i)
	{
		const MenuItem *item = &menu->items[i];
#if CPU_HARVARD
		MenuItem ram_item;
		if (menu->flags & MF_ROMITEMS)
		{
			memcpy_P(&ram_item, item, sizeof(ram_item));
			item = &ram_item;
		}
#endif /* CPU_HARVARD */

		/* Check for end of room */
		if (ypos > bm->cr.ymax)
			break;

		/* Check for end of menu */
		if (!(item->label || item->hook))
			break;

		/* Only print visible items */
		if (!(item->flags & MIF_HIDDEN))
		{
			/* Check if a special render function is supplied, otherwise use defaults */
			RenderHook renderhook = (item->flags & MIF_RENDERHOOK) ? (RenderHook)item->label : menu_defaultRenderHook;

			/* Render menuitem */
			renderhook(menu->bitmap, ypos++, (i == selected), item);

			ypos += bm->font->height;
		}
	}

#if CONFIG_MENU_SMOOTH
	if (redraw)
	{
		/* Clear rest of area */
		gfx_rectClear(bm, bm->cr.xmin, ypos, bm->cr.xmax, bm->cr.ymax);

		lcd_blitBitmap(&lcd_bitmap);
	}

	/* Restore old cliprect */
	gfx_setClipRect(bm,
			bm->cr.xmin, old_ymin,
			bm->cr.xmax, bm->cr.ymax);

#endif /* CONFIG_MENU_SMOOTH */
}


/**
 * Handle menu item selection
 */
static iptr_t menu_doselect(const struct Menu *menu, struct MenuItem *item)
{
	iptr_t result = 0;

	/* Exclude other items */
	int mask, i;
	for (mask = item->flags & MIF_EXCLUDE_MASK, i = 0; mask; mask >>= 1, ++i)
	{
		if (mask & 1)
			menu->items[i].flags &= ~MIF_CHECKED;
	}

	if (item->flags & MIF_DISABLED)
		return MENU_DISABLED;

	/* Handle checkable items */
	if (item->flags & MIF_TOGGLE)
		item->flags ^= MIF_CHECKED;
	else if (item->flags & MIF_CHECKIT)
		item->flags |= MIF_CHECKED;

	/* Handle items with callback hooks */
	if (item->hook)
	{
		/* Push a jmp buffer to abort the operation with the STOP/CANCEL key */
		if (!PUSH_ABORT)
		{
			result = item->hook(item->userdata);
			POP_ABORT;
		}
	}
	else
		result = item->userdata;

	return result;
}


/**
 * Return the next visible item (rolls back to the first item)
 */
static int menu_next_visible_item(const struct Menu *menu, int index)
{
	int total = menu_count(menu);
	int item_flags;

	do
	{
		if (++index >= total)
		   index = 0;

#if CPU_HARVARD
		if (menu->flags & MF_ROMITEMS)
		{
			ASSERT(sizeof(menu->items[index].flags) == sizeof(int));
			item_flags = pgm_read_int(&menu->items[index].flags);
		}
		else
#endif
			item_flags = menu->items[index].flags;
	}
	while (item_flags & MIF_HIDDEN);

	return index;
}


/**
 * Return the previous visible item (rolls back to the last item)
 */
static int menu_prev_visible_item(const struct Menu *menu, int index)
{
	int total = menu_count(menu);
	int item_flags;

	do
	{
		if (--index < 0)
			index = total - 1;

#if CPU_HARVARD
		if (menu->flags & MF_ROMITEMS)
		{
			ASSERT(sizeof(menu->items[index].flags) == sizeof(int));
			item_flags = pgm_read_int(&menu->items[index].flags);
		}
		else
#endif
			item_flags = menu->items[index].flags;
	}
	while (item_flags & MIF_HIDDEN);

	return index;
}


/**
 * Handle a menu and invoke hook functions for the selected menu items.
 */
iptr_t menu_handle(const struct Menu *menu)
{
	uint8_t items_per_page;
	uint8_t first_item = 0;
	uint8_t selected;
	iptr_t result = 0;
	bool redraw = true;

#if (CONFIG_MENU_TIMEOUT != 0)
	ticks_t now, menu_idle_time = timer_clock();
#endif

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


	items_per_page =
		(menu->bitmap->height / menu->bitmap->font->height)
#if CONFIG_MENU_MENUBAR
		- 1 /* menu bar labels */
#endif
		- (menu->title ? 1 : 0);

	/* Selected item should be a visible entry */
	//first_item = selected = menu_next_visible_item(menu, menu->selected - 1);
	selected = menu->selected;
	first_item = 0;

	for(;;)
	{
		keymask_t key;

		/*
		 * Keep selected item visible
		 */
		while (selected < first_item)
			first_item = menu_prev_visible_item(menu, first_item);
		while (selected >= first_item + items_per_page)
			first_item = menu_next_visible_item(menu, first_item);

		menu_layout(menu, first_item, selected, redraw);
		redraw = false;

		#if CONFIG_MENU_MENUBAR
			menu_update_menubar(menu, &mb, selected);
		#endif

		#if CONFIG_MENU_SMOOTH || (CONFIG_MENU_TIMEOUT != 0)
			key = kbd_peek();
		#else
			key = kbd_get();
		#endif

		#if (CONFIG_MENU_TIMEOUT != 0)
			/* Reset idle timer on key press. */
			now = timer_clock();
			if (key)
				menu_idle_time = now;
		#endif

		if (key & K_OK)
		{
			struct MenuItem *item = &(menu->items[selected]);
#if CPU_HARVARD
			MenuItem ram_item;
			if (menu->flags & MF_ROMITEMS)
			{
				memcpy_P(&ram_item, item, sizeof(ram_item));
				item = &ram_item;
			}
#endif
			result = menu_doselect(menu, item);
			redraw = true;

			/* Return immediately */
			if (!(menu->flags & MF_STICKY))
				break;

			#if (CONFIG_MENU_TIMEOUT != 0)
				/* Chain timeout */
				if ((result == MENU_TIMEOUT) && !(menu->flags & MF_TOPLEVEL))
					break;

				/* Reset timeout */
				menu_idle_time = timer_clock();
			#endif
		}
		else if (key & K_UP)
		{
			selected = menu_prev_visible_item(menu, selected);
			redraw = true;
		}
		else if (key & K_DOWN)
		{
			selected = menu_next_visible_item(menu, selected);
			redraw = true;
		}
		else if (!(menu->flags & MF_TOPLEVEL))
		{
			if (key & K_CANCEL)
			{
				result = MENU_CANCEL;
				break;
			}

			#if CONFIG_MENU_TIMEOUT != 0
				if (now - menu_idle_time > ms_to_ticks(CONFIG_MENU_TIMEOUT))
				{
					result = MENU_TIMEOUT;
					break;
				}
			#endif
		}
	}

	/* Store currently selected item before leaving. */
	if (menu->flags & MF_SAVESEL)
		CONST_CAST(struct Menu *, menu)->selected = selected;

	return result;
}


/**
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
	ASSERT(!(menu->flags & MF_ROMITEMS));

	int old = menu->items[idx].flags;
	menu->items[idx].flags |= flags;
	return old;
}


/**
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
	ASSERT(!(menu->flags & MF_ROMITEMS));

	int old = menu->items[idx].flags;
	menu->items[idx].flags &= ~flags;
	return old;
}

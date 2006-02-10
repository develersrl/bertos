/*!
 * \file
 * <!--
 * Copyright 2006 Develer S.r.l. (http://www.develer.com/)
 * This file is part of DevLib - See README.devlib for information.
 * -->
 *
 * \version $Id$
 *
 * \author Bernardo Innocenti <bernie@develer.com>
 *
 * \brief Very simple hierarchical windowing system.
 *
 * All functions in this module are to be intended as methods
 * of the Window class.  Please see its documentation
 * for a module-wise introduction.
 *
 * \see struct Window
 */

/*#*
 *#* $Log$
 *#* Revision 1.3  2006/02/10 12:25:41  bernie
 *#* Add missing header.
 *#*
 *#* Revision 1.2  2006/01/26 00:36:48  bernie
 *#* Const correctness for some new functions.
 *#*
 *#* Revision 1.1  2006/01/23 23:14:29  bernie
 *#* Implement simple, but impressive windowing system.
 *#*
 *#*/

#include "win.h"
#include <mware/list.h>

/**
 * Map the contents of all child-windows into the bitmap of \a w.
 *
 * Please note that recursively draw children into their parent
 * effectively damages the parent buffer.
 */
void win_compose(Window *w)
{
	Window *child;

	/*
	 * Walk over all children, in back to front order and tell them
	 * to compose into us.
	 */
	REVERSE_FOREACH_NODE(child, &w->children)
	{
		/* Recursively compose child first. */
		win_compose(child);

		/* Draw child into our bitmap. */
		if (w->bitmap)
			gfx_blit(w->bitmap, &child->geom, child->bitmap, 0, 0);
	}
}

/**
 * Map window \a w into \a parent.
 *
 * The new window becomes the topmost window.
 *
 * \note Opening a window twice is illegal.
 *
 * \see win_close()
 */
void win_open(Window *w, Window *parent)
{
	ASSERT(!w->parent);
	w->parent = parent;
	ADDHEAD(&parent->children, &w->link);
}

/**
 * Detach window from its parent.
 *
 * Closing a window causes it to become orphan of its
 * parent.  Its content will no longer appear in its
 * parent after the next refresh cycle.
 *
 * \note Closing a window that has not been previously
 *       opened is illegal.
 *
 * \see win_open()
 */
void win_close(Window *w)
{
	ASSERT(w->parent);
	REMOVE(&w->link);
	w->parent = NULL;
}

/**
 * Move window to the topmost position relative to its sibling.
 *
 * \see win_move(), win_resize(), win_setGeometry()
 */
void win_raise(Window *w)
{
	ASSERT(w->parent);
	REMOVE(&w->link);
	ADDHEAD(&w->parent->children, &w->link);
}

/**
 * Set window position and size at the same time.
 *
 * This function is equivalent to subsequent calls to win_move()
 * and win_resize() using the coordinates provided by the
 * \a new_geom rectangle.
 *
 * \note The xmax and ymax members of \a new_geom are non-inclusive,
 *       as usual for the Rect interface.
 *
 * \see win_move()
 * \see win_resize()
 */
void win_setGeometry(Window *w, const Rect *new_geom)
{
	// requires C99?
	// memcpy(&w->geom, new_geom, sizeof(w->geom));
	w->geom = *new_geom;
}

/**
 * Move window to specified position.
 *
 * Move the window top-left corner to the pixel coordinates
 * \a left and \a top, which are relative to the parent window.
 *
 * \note A window can also be moved outside the borders
 *       of its parent, or at negative coordinates.
 *
 * \note It is allowed to move an orphan window.
 */
void win_move(Window *w, coord_t left, coord_t top)
{
	Rect r;

	r.xmin = left;
	r.ymin = top;
	r.xmax = r.xmin + RECT_WIDTH(&w->geom);
	r.ymax = r.ymin + RECT_WIDTH(&w->geom);

	win_setGeometry(w, &r);
}

/**
 * Resize the rectangle of a window.
 *
 * The window shrinks or grows to the specified size.
 *
 * \note Growing a window beyond the size of its
 *       backing bitmap results in unspecified behavior.
 *
 * \note It is allowed to resize an orphan window.
 */
void win_resize(Window *w, coord_t width, coord_t height)
{
	Rect r;

	r.xmin = w->geom.xmin;
	r.ymin = w->geom.ymin;
	r.xmax = r.xmin + width;
	r.ymax = r.ymin + height;

	win_setGeometry(w, &r);
}

/**
 * Initialize a new window structure.
 *
 *
 * The new window initial position is set to (0,0).
 * The size is set to the size of the installed bitmap,
 * or (0,0) if there's no backing store.
 *
 * \arg bm  The bitmap to install as backing store
 *          for drawing into the window, or NULL if
 *          the window is not drawable.
 */
void win_create(Window *w, Bitmap *bm)
{
	w->parent = NULL;
	w->bitmap = bm;
	w->geom.xmin = 0;
	w->geom.ymin = 0;
	if (bm)
	{
		w->geom.xmax = bm->width;
		w->geom.ymax = bm->height;
	}
	LIST_INIT(&w->children);
}


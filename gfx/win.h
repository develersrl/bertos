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
 *#* Revision 1.1  2006/01/23 23:14:29  bernie
 *#* Implement simple, but impressive windowing system.
 *#*
 *#*/

#ifndef GFX_WIN_H
#define GFX_WIN_H

#include <mware/list.h> /* Node, List */
#include <gfx/gfx.h>    /* coord_t */


EXTERN_C_BEGIN

/**
 * Window handle and context structure.
 *
 * A window is a small rectangular area on the
 * screen backed by its own bitmap where you
 * can draw.
 *
 * A window can contain any number of children
 * sub-windows that can be depth arranged with
 * respect to their siblings.
 *
 * At any time, a window and all its children
 * can be drawn into another bitmap to display
 * a complete screen, taking depth and
 * overlapping into account.
 *
 * This rendering model is commonly referred to as
 * screen composition, and is quite popular among
 * modern windowing systems.
 */
typedef struct Window
{
	Node    link;      /**< Link us with other siblings into our parent.  */
	struct Window *parent;  /**< Our parent window.  NULL for the root window. */

	Bitmap *bitmap;    /**< Pixel storage for window contents. */
	Rect    geom;      /**< [px] Window size and position relative to parent. */

	/**
	 * List of child windows, arranged by depth (front to back).
	 *
	 * Child top/left coordinates are relative to us.
	 */
	List    children;

} Window;

/*
 * Public function prototypes
 */
void win_compose(Window *w);
void win_open(Window *w, Window *parent);
void win_close(Window *w);
void win_raise(Window *w);
void win_setGeometry(Window *w, Rect *new_geom);
void win_move(Window *w, coord_t left, coord_t top);
void win_resize(Window *w, coord_t width, coord_t height);
void win_create(Window *w, Bitmap *bm);

EXTERN_C_END

#endif /* GFX_WIN_H */


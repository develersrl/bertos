/*!
 * \file
 * Copyright (C) 1999 Bernardo Innocenti <bernie@develer.com>
 * Copyright (C) 2003 Develer S.r.l. (http://www.develer.com/)
 * All Rights Reserved.
 *
 * \version $Id$
 *
 * \author Bernardo Innocenti <bernie@develer.com>
 * \author Stefano Fedrigo <aleph@develer.com>
 *
 * \brief General pourpose graphics routines
 */

/*
 * $Log$
 * Revision 1.1  2004/05/23 15:43:16  bernie
 * Import mware modules.
 *
 * Revision 1.4  2004/02/09 00:21:28  aleph
 * Various gfx fixes
 *
 * Revision 1.3  2004/01/27 23:24:19  aleph
 * Add new graphics primitives
 *
 * Revision 1.2  2004/01/07 23:33:01  aleph
 * Change copyright email
 *
 * Revision 1.1  2004/01/07 19:05:31  aleph
 * Add graphics routines
 *
 */

#ifndef GFX_H
#define GFX_H

#include "compiler.h"
#include <avr/pgmspace.h>


/*! Common type for coordinates expressed in pixel units */
typedef int coord_t;

#ifdef CONFIG_LCD_VCOORDS
/*! Common type for coordinates expressed in logical units */
typedef float vcoord_t;
#endif /* CONFIG_LCD_VCOORDS */


typedef struct Rect { coord_t xmin, ymin, xmax, ymax; } Rect;


/*! Control structure to draw in a bitmap */

typedef struct Bitmap
{
	uint8_t *raster;        /*!< Pointer to byte array to hold the data */
	coord_t width, height;  /*!< Width/Height in pixels */
	coord_t penX, penY;     /*!< Current pen position MoveTo()/LineTo() */

	Rect cr;                /*!< Clip drawing inside this rectangle */

#ifdef CONFIG_LCD_VCOORDS
	/*! Logical coordinate system */
	vcoord_t orgX, orgY;
	vcoord_t scaleX, scaleY;
#endif /* CONFIG_LCD_VCOORDS */

} Bitmap;


/* Function prototypes */
extern void gfx_InitBitmap(Bitmap *bm, uint8_t *raster, coord_t w, coord_t h);
extern void gfx_ClearBitmap(Bitmap *bm);
extern void gfx_blitBitmap_P(Bitmap *bm, const prog_uchar *raster);
extern void gfx_DrawLine(Bitmap *bm, coord_t x1, coord_t y1, coord_t x2, coord_t y2);
extern void gfx_FillRect(Bitmap *bm, coord_t x1, coord_t y1, coord_t x2, coord_t y2);
extern void gfx_DrawRect(Bitmap *bm, coord_t x1, coord_t y1, coord_t x2, coord_t y2);
extern void gfx_ClearRect(Bitmap *bm, coord_t x1, coord_t y1, coord_t x2, coord_t y2);
extern void gfx_MoveTo(Bitmap *bm, coord_t x, coord_t y);
extern void gfx_LineTo(Bitmap *bm, coord_t x, coord_t y);
extern void gfx_SetClipRect(Bitmap *bm, coord_t xmin, coord_t ymin, coord_t xmax, coord_t ymax);

#ifdef CONFIG_LCD_VCOORDS
extern void gfx_SetViewRect(Bitmap *bm, vcoord_t x1, vcoord_t y1, vcoord_t x2, vcoord_t y2);
extern coord_t gfx_TransformX(Bitmap *bm, vcoord_t x);
extern coord_t gfx_TransformY(Bitmap *bm, vcoord_t y);
extern void gfx_VDrawLine(Bitmap *bm, vcoord_t x1, vcoord_t y1, vcoord_t x2, vcoord_t y2);
#endif /* CONFIG_LCD_VCOORDS */

#endif /* GFX_H */

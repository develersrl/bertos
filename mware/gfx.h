/*!
 * \file
 * Copyright 2003, 2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 1999 Bernardo Innocenti <bernie@develer.com>
 * This file is part of DevLib - See devlib/README for information.
 *
 * \version $Id$
 *
 * \author Bernardo Innocenti <bernie@develer.com>
 * \author Stefano Fedrigo <aleph@develer.com>
 *
 * \brief General pourpose graphics routines
 */

/*#*
 *#* $Log$
 *#* Revision 1.8  2004/11/01 15:14:07  bernie
 *#* Update to current coding conventions.
 *#*
 *#* Revision 1.7  2004/09/20 03:29:06  bernie
 *#* Conditionalize AVR-specific code.
 *#*
 *#* Revision 1.6  2004/09/14 21:01:08  bernie
 *#* Rename rectangle drawing functions; Unify filled/cleared implementations.
 *#*
 *#* Revision 1.4  2004/08/10 07:00:16  bernie
 *#* Add missing header.
 *#*
 *#* Revision 1.3  2004/08/04 03:16:59  bernie
 *#* Switch to new DevLib CONFIG_ convention.
 *#*
 *#* Revision 1.2  2004/06/03 11:27:09  bernie
 *#* Add dual-license information.
 *#*
 *#* Revision 1.1  2004/05/23 15:43:16  bernie
 *#* Import mware modules.
 *#*/

#ifndef MWARE_GFX_H
#define MWARE_GFX_H

#include <compiler.h>
#include <config.h>


/*! Common type for coordinates expressed in pixel units */
typedef int coord_t;

#if CONFIG_GFX_VCOORDS
/*! Common type for coordinates expressed in logical units */
typedef float vcoord_t;
#endif /* CONFIG_GFX_VCOORDS */


typedef struct Rect { coord_t xmin, ymin, xmax, ymax; } Rect;


/*!
 * Control structure to draw in a bitmap
 */
typedef struct Bitmap
{
	uint8_t *raster;        /*!< Pointer to byte array to hold the data */
	coord_t width, height;  /*!< Width/Height in pixels */
	coord_t penX, penY;     /*!< Current pen position MoveTo()/LineTo() */

	Rect cr;                /*!< Clip drawing inside this rectangle */

#if CONFIG_GFX_VCOORDS
	/*!
	 * \name Logical coordinate system
	 * \{
	 */
	vcoord_t orgX, orgY;
	vcoord_t scaleX, scaleY;
	/*\}*/
#endif /* CONFIG_GFX_VCOORDS */

} Bitmap;


/* Function prototypes */
extern void gfx_bitmapInit (Bitmap *bm, uint8_t *raster, coord_t w, coord_t h);
extern void gfx_bitmapClear(Bitmap *bm);
extern void gfx_line       (Bitmap *bm, coord_t x1, coord_t y1, coord_t x2, coord_t y2);
extern void gfx_rectDraw   (Bitmap *bm, coord_t x1, coord_t y1, coord_t x2, coord_t y2);
extern void gfx_rectFillC  (Bitmap *bm, coord_t x1, coord_t y1, coord_t x2, coord_t y2, uint8_t color);
extern void gfx_rectFill   (Bitmap *bm, coord_t x1, coord_t y1, coord_t x2, coord_t y2);
extern void gfx_rectClear  (Bitmap *bm, coord_t x1, coord_t y1, coord_t x2, coord_t y2);
extern void gfx_moveTo     (Bitmap *bm, coord_t x,  coord_t y);
extern void gfx_lineTo     (Bitmap *bm, coord_t x,  coord_t y);
extern void gfx_setClipRect(Bitmap *bm, coord_t xmin, coord_t ymin, coord_t xmax, coord_t ymax);

#if CPU_AVR
	#include <avr/pgmspace.h>
	extern void gfx_blit_P(Bitmap *bm, const prog_uchar *raster);
#endif /* CPU_AVR */


#if CONFIG_GFX_VCOORDS
extern void gfx_setViewRect(Bitmap *bm, vcoord_t x1, vcoord_t y1, vcoord_t x2, vcoord_t y2);
extern coord_t gfx_transformX(Bitmap *bm, vcoord_t x);
extern coord_t gfx_transformY(Bitmap *bm, vcoord_t y);
extern void gfx_vline(Bitmap *bm, vcoord_t x1, vcoord_t y1, vcoord_t x2, vcoord_t y2);
#endif /* CONFIG_GFX_VCOORDS */

#endif /* MWARE_GFX_H */

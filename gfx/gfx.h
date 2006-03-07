/*!
 * \file
 * Copyright 2003, 2004, 2005, 2006 Develer S.r.l. (http://www.develer.com/)
 * Copyright 1999 Bernardo Innocenti <bernie@develer.com>
 * This file is part of DevLib - See README.devlib for information.
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
 *#* Revision 1.11  2006/03/07 22:18:04  bernie
 *#* Correctly compute text width for prop fonts; Make styles a per-bitmap attribute.
 *#*
 *#* Revision 1.10  2006/02/17 22:24:40  bernie
 *#* Fix undefined preprocessor symbol.
 *#*
 *#* Revision 1.9  2006/02/15 09:10:15  bernie
 *#* Implement prop fonts; Fix algo styles.
 *#*
 *#* Revision 1.8  2006/02/10 12:28:33  bernie
 *#* Add font support in bitmaps; Make bitmap formats public.
 *#*
 *#* Revision 1.7  2006/01/26 00:36:48  bernie
 *#* Const correctness for some new functions.
 *#*
 *#* Revision 1.6  2006/01/23 23:13:04  bernie
 *#* RECT_WIDTH(), RECT_HEIGHT(), RASTER_SIZE(): New macros.
 *#*
 *#* Revision 1.5  2006/01/17 02:31:29  bernie
 *#* Add bitmap format support; Improve some comments.
 *#*/

#ifndef GFX_GFX_H
#define GFX_GFX_H

#include <cfg/compiler.h>
#include <cfg/cpu.h>   /* CPU_HARVARD */

#include <appconfig.h> /* CONFIG_GFX_* */

/**
 * \name Known pixel formats for bitmap representation.
 * \{
 */
#define BITMAP_FMT_PLANAR_H_MSB  1  /**< Planar pixels, horizontal bytes, MSB left. */
#define BITMAP_FMT_PLANAR_V_LSB  2  /**< Planar pixels, vertical bytes, LSB top. */
/* \} */

#if !defined(CONFIG_BITMAP_FMT) || (CONFIG_BITMAP_FMT != BITMAP_FMT_PLANAR_H_MSB && CONFIG_BITMAP_FMT != BITMAP_FMT_PLANAR_V_LSB)
	#error CONFIG_BITMAP_FMT must be defined to either BITMAP_FMT_PLANAR_H_LSB or BITMAP_FMT_PLANAR_V_LSB
#endif
#if !defined(CONFIG_GFX_CLIPPING) || (CONFIG_GFX_CLIPPING != 0 && CONFIG_GFX_CLIPPING != 1)
	#error CONFIG_GFX_CLIPPING must be defined to either 0 or 1
#endif
#if !defined(CONFIG_GFX_TEXT) || (CONFIG_GFX_TEXT != 0 && CONFIG_GFX_TEXT != 1)
	#error CONFIG_GFX_TEXT must be defined to either 0 or 1
#endif

EXTERN_C_BEGIN

/*! Common type for coordinates expressed in pixel units */
typedef int coord_t;

#if CONFIG_GFX_VCOORDS
/*! Common type for coordinates expressed in logical units */
typedef float vcoord_t;
#endif /* CONFIG_GFX_VCOORDS */


/**
 * Describe a rectangular area with coordinates expressed in pixels.
 *
 * The rectangle is represented in terms of its top/left and
 * right/bottom borders.
 *
 * In some cases, rectangles are assumed to obey to the
 * following invariants:
 *
 *    xmin <= xmax
 *    ymin <= ymax
 *
 * Oddly, the xmin and ymin coordinates are inclusive, while the
 * xmax and ymax coordinates are non-inclusive.  This design
 * decision makes several computations simpler and lets you
 * specify empty (0x0) rectangles without breaking the
 * invariants.
 *
 * Computing the size of a rectangle can be done by simply
 * subtracting the maximum X or Y coordinate from the minimum
 * X or Y coordinate.
 */
typedef struct Rect { coord_t xmin, ymin, xmax, ymax; } Rect;

/**
 * Return the width of a rectangle in pixels.
 *
 * \note The argument \a r is evaluated twice.
 */
#define RECT_WIDTH(r)   ((r)->xmax - (r)->xmin)

/**
 * Return the height of a rectangle in pixels.
 *
 * \note The argument \a r is evaluated twice.
 */
#define RECT_HEIGHT(r)  ((r)->ymax - (r)->ymin)

/* Fwd decl */
struct Font;

/*!
 * Control structure to draw in a bitmap
 */
typedef struct Bitmap
{
	uint8_t *raster;        /*!< Pointer to byte array to hold the data */
	coord_t width, height;  /*!< Width/Height in pixels */
	coord_t stride;		/*!< Bytes per row. */
	coord_t penX, penY;     /*!< Current pen position MoveTo()/LineTo() */

	Rect cr;                /*!< Clip drawing inside this rectangle */

#if CONFIG_GFX_TEXT
	const struct Font *font;/**< Current font for text rendering. */

	/**
	 * Algorithmic text style flags.
	 *
	 * The text rendering routine can apply a few simple transformations
	 * to the current font in order to generate common styles such as
	 * bold, italic and underline from plain glyphs.
	 */
	uint8_t styles;
#endif
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

#if CONFIG_BITMAP_FMT == BITMAP_FMT_PLANAR_H_MSB
	/**
	 * Compute the size in bytes of a raster suitable for
	 * holding a bitmap of \a width x \a height pixels.
	 */
	#define RASTER_SIZE(width, height) ( ((width) + 7 / 8) * (height) )

#elif CONFIG_BITMAP_FMT == BITMAP_FMT_PLANAR_V_LSB
	/**
	 * Compute the size in bytes of a raster suitable for
	 * holding a bitmap of \a width x \a height pixels.
	 */
	#define RASTER_SIZE(width, height) ( (width) * (((height) + 7) / 8) )
#else
	#error Unknown value of CONFIG_BITMAP_FMT
#endif /* CONFIG_BITMAP_FMT */


/* Function prototypes */
void gfx_bitmapInit (Bitmap *bm, uint8_t *raster, coord_t w, coord_t h);
void gfx_bitmapClear(Bitmap *bm);
void gfx_blit       (Bitmap *dst, const Rect *rect, const Bitmap *src, coord_t srcx, coord_t srcy);
void gfx_blitRaster (Bitmap *dst, coord_t dx, coord_t dy, const uint8_t *raster, coord_t w, coord_t h, coord_t stride);
void gfx_line       (Bitmap *bm, coord_t x1, coord_t y1, coord_t x2, coord_t y2);
void gfx_rectDraw   (Bitmap *bm, coord_t x1, coord_t y1, coord_t x2, coord_t y2);
void gfx_rectFillC  (Bitmap *bm, coord_t x1, coord_t y1, coord_t x2, coord_t y2, uint8_t color);
void gfx_rectFill   (Bitmap *bm, coord_t x1, coord_t y1, coord_t x2, coord_t y2);
void gfx_rectClear  (Bitmap *bm, coord_t x1, coord_t y1, coord_t x2, coord_t y2);
void gfx_moveTo     (Bitmap *bm, coord_t x,  coord_t y);
void gfx_lineTo     (Bitmap *bm, coord_t x,  coord_t y);
void gfx_setClipRect(Bitmap *bm, coord_t xmin, coord_t ymin, coord_t xmax, coord_t ymax);

#if CPU_HARVARD
	#include <mware/pgm.h>
	void gfx_blit_P(Bitmap *bm, const pgm_uint8_t *raster);
#endif

#if CONFIG_GFX_TEXT
INLINE void gfx_setFont(Bitmap *bm, const struct Font *font)
{
	bm->font = font;
}
#endif

#if CONFIG_GFX_VCOORDS
void gfx_setViewRect(Bitmap *bm, vcoord_t x1, vcoord_t y1, vcoord_t x2, vcoord_t y2);
coord_t gfx_transformX(Bitmap *bm, vcoord_t x);
coord_t gfx_transformY(Bitmap *bm, vcoord_t y);
void gfx_vline(Bitmap *bm, vcoord_t x1, vcoord_t y1, vcoord_t x2, vcoord_t y2);
#endif /* CONFIG_GFX_VCOORDS */

EXTERN_C_END

#endif /* GFX_GFX_H */

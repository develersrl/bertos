/**
 * \file
 * Copyright 2003, 2004, 2005, 2006 Develer S.r.l. (http://www.develer.com/)
 * Copyright 1999 Bernardo Innocenti <bernie@develer.com>
 *
 *
 * \version $Id$
 *
 * \author Bernardo Innocenti <bernie@develer.com>
 * \author Stefano Fedrigo <aleph@develer.com>
 *
 * \brief General pourpose graphics routines
 */

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

/** Common type for coordinates expressed in pixel units */
typedef int coord_t;
typedef unsigned int ucoord_t;

#if CONFIG_GFX_VCOORDS
/** Common type for coordinates expressed in logical units */
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

/**
 * Control structure to draw in a bitmap
 *
 * \todo For better ortogonality, split this structure into
 *       an Image and a plain drawing context called Painter.
 */
typedef struct Bitmap
{
	uint8_t *raster;        /**< Pointer to byte array to hold the data */
	coord_t width, height;  /**< Width/Height in pixels */
	coord_t stride;         /**< Bytes per row. */
	coord_t penX, penY;     /**< Current pen position MoveTo()/LineTo() */

#if CONFIG_GFX_CLIPPING || CONFIG_GFX_VCOORDS
	Rect cr;                /**< Clip drawing inside this rectangle */
#endif

#if CONFIG_GFX_TEXT
	const struct Font *font;/**< Current font for text rendering. */

	/**
	 * Algorithmic text style flags.
	 *
	 * The text rendering routine can apply a few simple transformations
	 * to the current font in order to generate common styles such as
	 * bold, italic and underline from plain glyphs.
	 *
	 * \see text_style()
	 */
	uint8_t styles;
#endif /* CONFIG_GFX_TEXT */

#if CONFIG_GFX_VCOORDS
	/**
	 * \name Logical coordinate system
	 * \{
	 */
	vcoord_t orgX, orgY;
	vcoord_t scaleX, scaleY;
	/*\}*/
#endif /* CONFIG_GFX_VCOORDS */

} Bitmap;

/**
 * Hold image pixels.
 *
 * \todo Use this as Bitmap and change Bitmap to Drawable.
 */
typedef struct Image
{
	const uint8_t *raster;   /**< Pointer to byte array to hold the data. */
	coord_t width;     /**< Raster width in pixels. */
	coord_t height;    /**< Raster height in pixels. */
	coord_t stride;    /**< Bytes per row. */
} Image;

#if CONFIG_BITMAP_FMT == BITMAP_FMT_PLANAR_H_MSB
	/**
	 * Compute the size in bytes of a raster suitable for
	 * holding a bitmap of \a width x \a height pixels.
	 */
	#define RAST_SIZE(width, height) ( ((width) + 7 / 8) * (height) )

#elif CONFIG_BITMAP_FMT == BITMAP_FMT_PLANAR_V_LSB
	/**
	 * Compute the size in bytes of a raster suitable for
	 * holding a bitmap of \a width x \a height pixels.
	 */
	#define RAST_SIZE(width, height) ( (width) * (((height) + 7) / 8) )
#else
	#error Unknown value of CONFIG_BITMAP_FMT
#endif /* CONFIG_BITMAP_FMT */

/* Function prototypes */
void gfx_bitmapInit (Bitmap *bm, uint8_t *raster, coord_t w, coord_t h);
void gfx_bitmapClear(Bitmap *bm);
void gfx_blit       (Bitmap *dst, const Rect *rect, const Bitmap *src, coord_t srcx, coord_t srcy);
void gfx_blitRaster (Bitmap *dst, coord_t dx, coord_t dy, const uint8_t *raster, coord_t w, coord_t h, coord_t stride);
void gfx_blitImage  (Bitmap *dst, coord_t dx, coord_t dy, const Image *image);
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

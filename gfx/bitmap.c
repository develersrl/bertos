/*!
 * \file
 * <!--
 * Copyright 2003, 2004, 2005, 2006 Develer S.r.l. (http://www.develer.com/)
 * Copyright 1999 Bernardo Innocenti <bernie@develer.com>
 * This file is part of DevLib - See README.devlib for information.
 * -->
 *
 * \version $Id$
 *
 * \author Bernardo Innocenti <bernie@develer.com>
 * \author Stefano Fedrigo <aleph@develer.com>
 *
 * \brief Bitmap manipulation routines.
 */

/*#*
 *#* $Log$
 *#* Revision 1.7  2006/03/07 22:18:04  bernie
 *#* Correctly compute text width for prop fonts; Make styles a per-bitmap attribute.
 *#*
 *#* Revision 1.6  2006/02/23 11:17:16  bernie
 *#* Documentation fixes.
 *#*
 *#* Revision 1.5  2006/02/15 09:10:15  bernie
 *#* Implement prop fonts; Fix algo styles.
 *#*
 *#* Revision 1.4  2006/02/10 12:32:33  bernie
 *#* Add multiple font support in bitmaps; gfx_blitRaster(): New function.
 *#*
 *#* Revision 1.3  2006/01/26 00:36:48  bernie
 *#* Const correctness for some new functions.
 *#*
 *#* Revision 1.2  2006/01/24 21:55:43  aleph
 *#* gfx_blit_P(): use RASTER_SIZE() to calculate raster size
 *#*
 *#* Revision 1.1  2006/01/24 02:17:49  bernie
 *#* Split out gfx.c into bitmap.c and line.c.
 *#*
 *#*/

#include "gfx.h"
#include "gfx_p.h"

#include <cfg/debug.h>  /* ASSERT() */
#include <cfg/cpu.h>    /* CPU_HARVARD */
#include <cfg/macros.h> /* MIN() */
#include <appconfig.h>  /* CONFIG_GFX_CLIPPING */

#include <string.h>     /* memset() */

#if CONFIG_GFX_TEXT
#include <gfx/font.h>   /* default_font */
#endif


/*!
 * Initialize a Bitmap structure with the provided parameters.
 *
 * \note The pen position is reset to the origin.
 */
void gfx_bitmapInit(Bitmap *bm, uint8_t *raster, coord_t w, coord_t h)
{
	bm->raster = raster;
	bm->width = w;
	bm->height = h;
	#if (CONFIG_BITMAP_FMT == BITMAP_FMT_PLANAR_H_MSB)
		bm->stride = (w + 7) / 8;
	#elif CONFIG_BITMAP_FMT == BITMAP_FMT_PLANAR_V_LSB
		bm->stride = w;
	#else
		#error Unknown value of CONFIG_BITMAP_FMT
	#endif /* CONFIG_BITMAP_FMT */
	bm->penX = 0;
	bm->penY = 0;

#if CONFIG_GFX_TEXT
	gfx_setFont(bm, &default_font);
	bm->styles = 0;
#endif

#if CONFIG_GFX_CLIPPING
	bm->cr.xmin = 0;
	bm->cr.ymin = 0;
	bm->cr.xmax = w;
	bm->cr.ymax = h;
#endif /* CONFIG_GFX_CLIPPING */
}


/*!
 * Clear the whole bitmap surface to the background color.
 *
 * \note This function does \b not update the current pen position.
 */
void gfx_bitmapClear(Bitmap *bm)
{
	memset(bm->raster, 0, RASTER_SIZE(bm->width, bm->height));
}


#if CPU_HARVARD

#include <avr/pgmspace.h> /* FIXME: memcpy_P() */

/*!
 * Copy a raster picture located in program memory in the bitmap.
 * The size of the raster to copy *must* be the same of the raster bitmap.
 *
 * \note This function does \b not update the current pen position
 */
void gfx_blit_P(Bitmap *bm, const pgm_uint8_t *raster)
{
	memcpy_P(bm->raster, raster, RASTER_SIZE(bm->width, bm->height));
}
#endif /* CPU_HARVARD */

/**
 * Copy a rectangular area of a bitmap on another bitmap.
 *
 * Blitting is a common copy operation involving two bitmaps.
 * A rectangular area of the source bitmap is copied bit-wise
 * to a different position in the destination bitmap.
 *
 * \note Using the same bitmap for \a src and \a dst is unsupported.
 *
 * \param dst  Bitmap where the operation writes.
 * \param rect The (xmin;ymin) coordinates provide the top/left offset
 *             for drawing in the destination bitmap.  If the source
 *             bitmap is larger than the rectangle, drawing is clipped.
 * \param src  Bitmap containing the source pixels.
 * \param srcx Starting X offset in the source bitmap.
 * \param srcy Starting Y offset in the source bitmap.
 */
void gfx_blit(Bitmap *dst, const Rect *rect, const Bitmap *src, coord_t srcx, coord_t srcy)
{
	coord_t dxmin, dymin, dxmax, dymax;
	coord_t dx, dy, sx, sy;

	/*
	 * Clip coordinates inside dst->cr and src->width/height.
	 */
	dxmin = rect->xmin;
	if (dxmin < dst->cr.xmin)
	{
		srcx += dst->cr.xmin - dxmin;
		dxmin = dst->cr.xmin;
	}
	dymin = rect->ymin;
	if (dymin < dst->cr.ymin)
	{
		srcy += dst->cr.ymin - dymin;
		dymin = dst->cr.ymin;
	}
	dxmax = MIN(MIN(rect->xmax, rect->xmin + src->width), dst->cr.xmax);
	dymax = MIN(MIN(rect->ymax, rect->ymin + src->height), dst->cr.ymax);

	/* TODO: make it not as dog slow as this */
	for (dx = dxmin, sx = srcx; dx < dxmax; ++dx, ++sx)
		for (dy = dymin, sy = srcy; dy < dymax; ++dy, ++sy)
			BM_DRAWPIXEL(dst, dx, dy, BM_READPIXEL(src, sx, sy));
}


void gfx_blitRaster(Bitmap *dst, coord_t dxmin, coord_t dymin, const uint8_t *raster, coord_t w, coord_t h, coord_t stride)
{
	coord_t dxmax, dymax;
	coord_t sxmin = 0, symin = 0;
	coord_t dx, dy, sx, sy;

	/*
	 * Clip coordinates inside dst->cr.
	 */
	if (dxmin < dst->cr.xmin)
	{
		sxmin += dst->cr.xmin - dxmin;
		dxmin = dst->cr.xmin;
	}
	if (dymin < dst->cr.ymin)
	{
		symin += dst->cr.ymin - dymin;
		dymin = dst->cr.ymin;
	}
	dxmax = MIN(dxmin + w, dst->cr.xmax);
	dymax = MIN(dymin + h, dst->cr.ymax);

	/* TODO: make it not as dog slow as this */
	for (dx = dxmin, sx = sxmin; dx < dxmax; ++dx, ++sx)
		for (dy = dymin, sy = symin; dy < dymax; ++dy, ++sy)
			BM_DRAWPIXEL(dst, dx, dy, RAST_READPIXEL(raster, sx, sy, stride));
}


/*!
 * Set the bitmap clipping rectangle to the specified coordinates.
 *
 * All drawing performed on the bitmap will be clipped inside this
 * rectangle.
 *
 * \note Following the convention used in all other operations, the
 *       top-left pixels of the rectangle are included, while the
 *       bottom-right pixels are considered outside the clipping region.
 */
void gfx_setClipRect(Bitmap *bm, coord_t minx, coord_t miny, coord_t maxx, coord_t maxy)
{
	ASSERT(minx < maxx);
	ASSERT(miny < maxy);
	ASSERT(miny >= 0);
	ASSERT(minx >= 0);
	ASSERT(maxx <= bm->width);
	ASSERT(maxy <= bm->height);

	bm->cr.xmin = minx;
	bm->cr.ymin = miny;
	bm->cr.xmax = maxx;
	bm->cr.ymax = maxy;

/*	kprintf("cr.xmin = %d, cr.ymin = %d, cr.xmax = %d, cr.ymax = %d\n",
		bm->cr.xMin, bm->cr.ymin, bm->cr.xmax, bm->cr.ymax);
*/
}


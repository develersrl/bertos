/*!
 * \file
 * <!--
 * Copyright 2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 1999, 2000, 2001, 2003 Bernardo Innocenti <bernie@develer.com>
 * This file is part of DevLib - See devlib/README for information.
 * -->
 *
 * \brief Simple charts on top of mware/gfx routines (implementation).
 *
 * Sample usage:
 *
 * \code
 *	bm = chart_init(0, ymax, N_POINTS_CURVE, ymin);
 *
 *	chart_drawCurve(bm, curve_y, curve_points + 1);
 *	gfx_setViewRect(bm, xmin, ymax, xmax, ymin);
 *	chart_drawDots(bm, samples_x, samples_y, samples_cnt);
 *
 *	print_bitmap(bm);
 * \endcode
 *
 * \version $Id$
 * \author Bernardo Innocenti <bernie@develer.com>
 */

/*#*
 *#* $Log$
 *#* Revision 1.4  2004/08/25 14:12:09  rasky
 *#* Aggiornato il comment block dei log RCS
 *#*
 *#* Revision 1.3  2004/08/11 19:39:12  bernie
 *#* Use chart_x_t and chart_y_t for the input dataset.
 *#*
 *#* Revision 1.1  2004/08/04 03:16:30  bernie
 *#* Import simple chart drawing code.
 *#*
 *#*/

#include "charts.h"
#include <mware/gfx.h>


void chart_init(Bitmap *bm, vcoord_t xmin, vcoord_t ymin, vcoord_t xmax, vcoord_t ymax)
{
	gfx_ClearBitmap(bm);
	chart_drawAxis(bm);

	gfx_SetClipRect(bm, CHART_BORDERLEFT, CHART_BORDERTOP,
		bm->width - CHART_BORDERRIGHT - 1, bm->height - CHART_BORDERBOTTOM - 1);

	gfx_SetViewRect(bm, xmin, ymin, xmax, ymax);

	//CHECK_WALL(wall_before_raster, WALL_SIZE);
	//CHECK_WALL(wall_after_raster, WALL_SIZE);
}


/*!
 * Draw the chart axes
 */
void chart_drawAxis(Bitmap *bm)
{
	/* Draw axis */
	gfx_MoveTo(bm, CHART_BORDERLEFT, 4);
	gfx_LineTo(bm, CHART_BORDERLEFT, CHART_BORDERTOP + CHART_HEIGHT - 1);
	gfx_LineTo(bm, CHART_BORDERLEFT + CHART_WIDTH - 5, CHART_BORDERTOP + CHART_HEIGHT - 1);

	/* Draw up arrow */
	gfx_MoveTo(bm, CHART_BORDERLEFT - 2, 3);
	gfx_LineTo(bm, CHART_BORDERLEFT + 2, 3);
	gfx_LineTo(bm, CHART_BORDERLEFT, 0);
	gfx_LineTo(bm, CHART_BORDERLEFT - 2, 3);

	/* Draw right arrow */
	gfx_MoveTo(bm, CHART_BORDERLEFT + CHART_WIDTH - 4, CHART_BORDERTOP + CHART_HEIGHT - 3);
	gfx_LineTo(bm, CHART_BORDERLEFT + CHART_WIDTH - 4, CHART_BORDERTOP + CHART_HEIGHT + 1);
	gfx_LineTo(bm, CHART_BORDERLEFT + CHART_WIDTH - 1, CHART_BORDERTOP + CHART_HEIGHT - 1);
	gfx_LineTo(bm, CHART_BORDERLEFT + CHART_WIDTH - 4, CHART_BORDERTOP + CHART_HEIGHT - 3);

	//CHECK_WALL(wall_before_raster, WALL_SIZE);
	//CHECK_WALL(wall_after_raster, WALL_SIZE);
}


/*!
 * Draw a set of \a curve_cnt connected segments, whose Y coordinates
 * are identified by the \a curve_y array and X-coordinates are
 * are evenly spaced by one virtual unit.
 */
void chart_drawCurve(Bitmap *bm, const chart_y_t *curve_y, int curve_cnt)
{
	int i;

	gfx_MoveTo(bm, gfx_TransformX(bm, 0), gfx_TransformY(bm, curve_y[0]));

	for (i = 1; i < curve_cnt; i++)
		gfx_LineTo(bm, gfx_TransformX(bm, i), gfx_TransformY(bm, curve_y[i]));

	//CHECK_WALL(wall_before_raster, WALL_SIZE);
	//CHECK_WALL(wall_after_raster, WALL_SIZE);
}


/*!
 * Disegna dei dot in corrispondenza delle coppie (dotsx[i];dotsy[i])
 * Se dotsx e' NULL, i punti vengono disegnati ad intervalli regolari.
 */
void chart_drawDots(Bitmap *bm, const chart_x_t *dots_x, const chart_y_t *dots_y, int cnt)
{
	int i;
	coord_t x, y;

	for (i = 0; i < cnt; i++)
	{
		if (dots_x)
			x = gfx_TransformX(bm, dots_x[i]);
		else
			x = gfx_TransformX(bm, i);

		y = gfx_TransformY(bm, dots_y[i]);

		gfx_DrawRect(bm, x - 1, y - 1, x + 1, y + 1);

		/* Disegna ticks sull'asse X */
		gfx_DrawLine(bm, x, bm->height - 1, x, CHART_HEIGHT - 1);
	}

	//CHECK_WALL(wall_before_raster, WALL_SIZE);
	//CHECK_WALL(wall_after_raster, WALL_SIZE);
}


/*!
 * \file
 * <!--
 * Copyright 2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 1999, 2000, 2001, 2003 Bernardo Innocenti <bernie@develer.com>
 * This file is part of DevLib - See devlib/README for information.
 * -->
 *
 * \brief Simple charts on top of mware/gfx routines (interface).
 *
 * \version $Id$
 * \author Bernardo Innocenti <bernie@develer.com>
 */

/*
 * $Log$
 * Revision 1.2  2004/08/11 07:32:54  bernie
 * Refactor after the new mware/gfx API.
 *
 * Revision 1.1  2004/08/04 03:16:30  bernie
 * Import simple chart drawing code.
 *
 */
#ifndef MWARE_CHARTS_H
#define MWARE_CHARTS_H

#include <mware/gfx.h> /* vcoord_t */

/*!
 * \name Width/height of the small ticks drawn over the axes
 * \{
 */
#define TICKS_HEIGHT     2
#define TICKS_WIDTH      3
/*\}*/

/*!
 * \name Chart frame dimensions
 * \{
 */
#define CHART_BORDERTOP       0
#define CHART_BORDERBOTTOM    TICKS_HEIGHT
#define CHART_BORDERLEFT      TICKS_WIDTH
#define CHART_BORDERRIGHT     0
/*\}*/

/*!
 * \name Chart size in pixels
 * \{
 */
#define CHART_WIDTH     (bm->width - CHART_BORDERLEFT - CHART_BORDERRIGHT)
#define CHART_HEIGHT    (bm->height  - CHART_BORDERTOP - CHART_BORDERBOTTOM)
/*\}*/


/* Public function protos */
extern void chart_init(Bitmap *bm, vcoord_t xmin, vcoord_t ymin, vcoord_t xmax, vcoord_t ymax);
extern void chart_drawAxis(Bitmap *bm);
extern void chart_drawCurve(Bitmap *bm, const vcoord_t *curve_y, int curve_cnt);
extern void chart_drawDots(Bitmap *bm, const vcoord_t *dotsx, const vcoord_t *dotsy, int cnt);

#endif /* MWARE_CHARTS_H */

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
 * Revision 1.1  2004/08/04 03:16:30  bernie
 * Import simple chart drawing code.
 *
 */
#ifndef MWARE_CHARTS_H
#define MWARE_CHARTS_H

#include <gfx.h> /* vcoord_t */

/*!
 * \name Width/height of the chart bitmap in pixels
 * \{
 */
#define BM_WIDTH       PRT_HRES
#define BM_HEIGHT      120
/*\}*/

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
#define CHART_WIDTH     (BM_WIDTH - CHART_BORDERLEFT - CHART_BORDERRIGHT)
#define CHART_HEIGHT    (BM_HEIGHT  - CHART_BORDERTOP - CHART_BORDERBOTTOM)
/*\}*/


/* Public function protos */
extern BitMap *chart_init(vcoord_t xmin, vcoord_t ymin, float xmax, float ymax);
extern void chart_drawAxis(BitMap *bm);
extern void chart_drawCurve(BitMap *bm, const vcoord_t *curve_y, int curve_cnt);
extern void chart_drawDots(BitMap *bm, const vcoord_t *dotsx, const float *dotsy, int cnt);

#endif /* MWARE_CHARTS_H */

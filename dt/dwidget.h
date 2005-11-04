/*!
 * \file
 * <!--
 * Copyright 2005 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 * \version $Id$
 *
 * \brief Widget (interface).
 *
 * \version $Id$
 * \author Bernardo Innocenti <bernie@develer.com>
 * \author Francesco Sacchi <batt@develer.com>
 */

/*#*
 *#* $Log$
 *#* Revision 1.1  2005/11/04 18:26:38  bernie
 *#* Import into DevLib.
 *#*
 *#* Revision 1.3  2005/06/06 11:04:12  batt
 *#* Add some comments.
 *#*
 *#* Revision 1.2  2005/05/26 14:44:10  batt
 *#* Abstract widget from layer: use context.
 *#*
 *#* Revision 1.1  2005/05/26 08:32:53  batt
 *#* Add new Develer widget system :)
 *#*
 *#*/

#ifndef DT_DWIDGET_H
#define DT_DWIDGET_H

#include <dt/dnotifier.h>
#include <cfg/compiler.h>

/*! Widget position. */
typedef uint8_t dpos_t;

/*! Widget drawing context. */
typedef iptr_t dcontext_t;

/*! Widget definition. */
typedef struct DWidget
{
	DNotifier notifier;

	dpos_t pos;
	dpos_t size;
	dcontext_t *context;
} DWidget;


void widget_init(DWidget *w, dpos_t pos, dpos_t size, dcontext_t *context);

#endif

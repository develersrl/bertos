/*!
 * \file
 * <!--
 * Copyright 2005 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 * \version $Id$
 *
 * \brief Widget (implementation).
 * A widget is typically a graphic object on a device.
 * Its proproperties are the position, the size and a context on which the widget is drawn.
 *
 * \version $Id$
 * \author Bernardo Innocenti <bernie@develer.com>
 * \author Francesco Sacchi <batt@ðeveler.com>
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

#include <dt/dwidget.h>
#include <dt/dnotifier.h>

/*!
 * Init the widget of \a pos and \a size on the drawing \a context.
 */
void widget_init(DWidget *w, dpos_t pos, dpos_t size, dcontext_t *context)
{
	// Init superclass
	notifier_init(&w->notifier);

	// Init instance
	w->pos = pos;
	w->size = size;
	w->context = context;
}

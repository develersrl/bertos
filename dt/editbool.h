/**
 * \file
 * <!--
 * Copyright 2005 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 * \version $Id$
 *
 * \brief Edit bool widget (interface).
 *
 * \version $Id$
 * \author Francesco Sacchi <batt@develer.com>
 */

/*#*
 *#* $Log$
 *#* Revision 1.2  2006/07/19 12:56:26  bernie
 *#* Convert to new Doxygen style.
 *#*
 *#* Revision 1.1  2005/11/04 18:26:38  bernie
 *#* Import into DevLib.
 *#*
 *#* Revision 1.2  2005/06/06 11:04:12  batt
 *#* Add some comments.
 *#*
 *#* Revision 1.1  2005/05/31 11:11:37  batt
 *#* Edit bool: first release.
 *#*
 *#*/

#ifndef DT_EDITBOOL_H
#define DT_EDITBOOL_H

#include <dt/dwidget.h>
#include <dt/dtag.h>

typedef struct DEditBool
{
	DWidget widget;
	bool *value;
	const char *true_string;
	const char *false_string;
	void (*draw)(struct DEditBool *);
} DEditBool;

void editbool_init(DEditBool *e, dpos_t pos, dpos_t size, dcontext_t *context, bool *val, const char *true_str, const char *false_str);
void editbool_update(DEditBool *e, dtag_t tag, dval_t val);
void editbool_draw(DEditBool *e);


#endif

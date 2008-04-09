/**
 * \file
 * Copyright 2004, 2005 Develer S.r.l. (http://www.develer.com/)
 * All Rights Reserved.
 *
 * \version $Id$
 *
 * \author Stefano Fedrigo <aleph@develer.com>
 *
 * \brief Graphic menu bar widget.
 */

/*#*
 *#* $Log: menubar.h,v $
 *#* Revision 1.17  2006/06/16 16:18:49  batt
 *#* Fix doxygen docs.
 *#*
 *#* Revision 1.16  2005/11/16 18:10:19  bernie
 *#* Move top-level headers to cfg/ as in DevLib.
 *#*
 *#* Revision 1.15  2005/02/17 03:49:21  bernie
 *#* Update to new PGM api.
 *#*
 *#* Revision 1.14  2004/10/31 11:02:15  aleph
 *#* Rename functions with correct codying conventions; Simplify version display
 *#*
 *#* Revision 1.13  2004/09/27 12:05:46  powersoft
 *#* Use sel label for toggle menus and remove it
 *#*
 *#* Revision 1.12  2004/09/27 10:05:33  powersoft
 *#* Menu cosmetic fixes
 *#*/
#ifndef MWARE_MENUBAR_H
#define MWARE_MENUBAR_H

#include <appconfig.h>
#include <cfg/compiler.h>
#include <brand.h>

/** Predefined labels ids */
enum LabelId
{
	LABEL_EMPTY,  /* empty label */
	LABEL_MUTE,
	LABEL_MENU,
	LABEL_BACK,
	LABEL_OK,
	LABEL_CH_1,
	LABEL_CH_2,
	LABEL_C1PLUS2,
	LABEL_UPARROW,
	LABEL_DOWNARROW,
	LABEL_MINUS,
	LABEL_PLUS,
	LABEL_SEL,
	#if OEM_BRAND == OEM_CLAIRBROS
	LABEL_GAIN,
	#else
	LABEL_LOCK,
	#endif
	LABEL_UNLOCK,
	LABEL_MORE,
	LABEL_EDIT,
	LABEL_FAST,
	LABEL_PREV,
	LABEL_NEXT,
	LABEL_SLOW,
	LABEL_YES,
	LABEL_NO,


	LABEL_CNT
};

#define UP_ARROW "\x18"
#define DOWN_ARROW  "\x19"
#define RIGHT_ARROW "\xC4\x1A"
#define LEFT_ARROW  "\x10\xC4"

/* Forward decl */
struct Bitmap;

typedef struct MenuBar
{
	struct Bitmap *bitmap;
	const_iptr_t  *labels;
	int            num_labels;
} MenuBar;

void mbar_init(
		struct MenuBar *mb,
		struct Bitmap *bmp,
		const_iptr_t *labels,
		int num_labels);
void mbar_draw(const struct MenuBar *mb);

#endif /* MWARE_MENUBAR_H */

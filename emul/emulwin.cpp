/**
 * \file
 * <!--
 * Copyright 2006 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2000, 2001 Bernardo Innocenti <bernie@codewiz.org>
 * This file is part of DevLib - See README.devlib for information.
 * -->
 *
 * \version $Id$
 *
 * \author Bernardo Innocenti <bernie@develer.com>
 *
 * \brief Main Qt window for embedded applications emulator (implementation)
 */

/*#*
 *#* $Log$
 *#* Revision 1.2  2006/01/16 03:51:51  bernie
 *#* Fix boilerplate.
 *#*
 *#* Revision 1.1  2006/01/16 03:37:12  bernie
 *#* Add emulator skeleton.
 *#*
 *#*/

#include "emulwin.h"

#include <drv/lcd_gfx_qt.h>
#include <emul/emul.h>

#include <cassert>
#include <qlayout.h>
#include <qlabel.h>
#include <qslider.h>
#include <qcheckbox.h>
#include <qpopupmenu.h>
#include <qmenubar.h>
#include <qmessagebox.h>
#include <qdatetime.h>
#include <qtimer.h>
#include <qapplication.h>

EmulWin::EmulWin(Emulator *e) : QMainWindow(0, "SarfEmul", WDestructiveClose)
{
	// "File" menu
	QPopupMenu * file = new QPopupMenu(this);
	file->insertItem("&Quit", static_cast<QObject *>(e->emulApp), SLOT(closeAllWindows()), CTRL+Key_Q);

	// "Help" menu
	QPopupMenu * help = new QPopupMenu(this);
	help->insertItem("&About", this, SLOT(about()), Key_F1);

	// Menu bar
	QMenuBar * menu = new QMenuBar(this);
	menu->insertItem("&File", file);
	menu->insertSeparator();
	menu->insertItem("&Help", help);

	// Make a central widget to contain the other widgets
	QWidget *central = new QWidget(this);
	setCentralWidget(central);

	// Create a layout to position the widgets
	QHBoxLayout *box_main = new QHBoxLayout(central, 4);

	// Main layout
	QVBoxLayout *box_right = new QVBoxLayout(box_main, 4);

		// LCD
		QHBoxLayout *lay_lcd = new QHBoxLayout(box_right, 4);
		lay_lcd->addWidget(e->emulLCD = new EmulLCD(central));
}


EmulWin::~EmulWin()
{
	emul->quit();
}


void EmulWin::closeEvent(QCloseEvent *ce)
{
	ce->accept();
}


void EmulWin::about()
{
    QMessageBox::about(this,
		"Embedded Application Emulator",
		"Version 0.1\n"
		"Copyright 2006 Develer S.r.l. (http://www.develer.com/)\n"
		"Copyright 2001, 2002, 2003, 2005 Bernardo Innocenti <bernie@codewiz.org>\n"
		"All rights reserved."
	);
}

#include "emulwin_moc.cpp"

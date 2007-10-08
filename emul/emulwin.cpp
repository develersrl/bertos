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
 *#* Revision 1.6  2006/09/19 17:49:04  bernie
 *#* Reindent.
 *#*
 *#* Revision 1.5  2006/05/28 12:17:56  bernie
 *#* Drop almost all the Qt3 cruft.
 *#*
 *#* Revision 1.4  2006/02/20 02:00:39  bernie
 *#* Port to Qt 4.1.
 *#*
 *#* Revision 1.3  2006/02/15 09:11:17  bernie
 *#* Add keyboard emulator.
 *#*
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
#include <emul/emulkbd.h>

#include <cassert>

#include <QtGui/QLayout>
#include <QtGui/QLabel>
#include <QtGui/QSlider>
#include <QtGui/QCheckBox>
#include <QtGui/QMenuBar>
#include <QtGui/QMessageBox>
#include <QtCore/QDateTime>
#include <QtCore/QTimer>
#include <QtGui/QApplication>
#include <QtGui/QCloseEvent>
using namespace Qt;

EmulWin::EmulWin(Emulator *e)
{
	setWindowTitle(tr("DevLib Emul Demo"));
	setAttribute(Qt::WA_DeleteOnClose);

	// Create the menu bar
	QMenu *file_menu = menuBar()->addMenu(tr("&File"));
	file_menu->addAction(tr("&Quit"),
		e->emulApp, SLOT(closeAllWindows()), CTRL+Key_Q);

	menuBar()->addSeparator();

	QMenu *help_menu = menuBar()->addMenu(tr("&Help"));
	help_menu->addAction(tr("&About"),
		this, SLOT(about()), Key_F1);

	// Make a central widget to contain the other widgets
	QWidget *central = new QWidget(this);
	setCentralWidget(central);

	// Create a layout to position the widgets
	QHBoxLayout *box_main = new QHBoxLayout(central);

	// Main layout
	QVBoxLayout *box_right = new QVBoxLayout();
	box_main->addLayout(box_right);

		// LCD
		QHBoxLayout *lay_lcd = new QHBoxLayout();
		box_right->addLayout(lay_lcd);
			lay_lcd->addStretch();
			lay_lcd->addWidget(e->emulLCD = new EmulLCD(central));
			lay_lcd->addStretch();

		// Keyboard
		box_right->addWidget(e->emulKbd = new EmulKbd(central));

	// Setup keyboard: Label   Keycode     Row Col MRow MCol
	e->emulKbd->addKey("^",    Key_Up,     0,  0,  0,   0);
	e->emulKbd->addKey("v",    Key_Down,   1,  0,  0,   1);
	e->emulKbd->addKey("OK",   Key_Return, 0,  1,  0,   2);
	e->emulKbd->addKey("ESC",  Key_Escape, 1,  1,  0,   3);
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

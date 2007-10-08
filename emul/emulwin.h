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
 * \brief Main Qt window for embedded applications emulator (interface)
 */

/*#*
 *#* $Log$
 *#* Revision 1.4  2006/05/28 12:17:56  bernie
 *#* Drop almost all the Qt3 cruft.
 *#*
 *#* Revision 1.3  2006/02/20 02:00:39  bernie
 *#* Port to Qt 4.1.
 *#*
 *#* Revision 1.2  2006/01/16 03:51:51  bernie
 *#* Fix boilerplate.
 *#*
 *#* Revision 1.1  2006/01/16 03:37:12  bernie
 *#* Add emulator skeleton.
 *#*
 *#*/

#ifndef EMUL_EMULWIN_H
#define EMUL_EMULWIN_H

#include <QtGui/QMainWindow>

// fwd decls
class Emulator;

class EmulWin : public QMainWindow
{
	Q_OBJECT

// construction
public:
	EmulWin(Emulator *emul);
	~EmulWin();

protected:
	void closeEvent(QCloseEvent *);

private slots:
	void about();
};

#endif // EMUL_EMULWIN_H


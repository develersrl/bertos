/**
 * \file
 * <!--
 * Copyright 2006 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2000, 2001 Bernardo Innocenti <bernie@codewiz.org>
 * All Rights Reserved.
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
 *#* Revision 1.1  2006/01/16 03:37:12  bernie
 *#* Add emulator skeleton.
 *#*
 *#*/

#ifndef EMUL_EMULWIN_H
#define EMUL_EMULWIN_H

#include <qmainwindow.h>

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


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
 * \brief Qt-based emulator framework for embedded applications (implementation)
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

#include "emul.h"
#include "emulwin.h"

#include <qapplication.h>
#include <cstdlib> // std::exit()

/// The global emulator instance.
Emulator *emul;

Emulator::Emulator(int &argc, char **argv) :
	emulApp(new QApplication(argc, argv)),
	emulWin(new EmulWin(this))
{
	emulApp->setMainWidget(emulWin);
	emulWin->show();
}


Emulator::~Emulator()
{
	// we don't delete emulWin because it automatically
	// deletes itself when closed
	delete emulApp;
}


void Emulator::quit()
{
	// WHAT A KLUDGE!
	this->~Emulator();
	emul = NULL;

	// do we have a better way to shut down the emulation?
	exit(0);
}


/// Main emulator entry point.
extern "C" void emul_init(int *argc, char *argv[])
{
	ASSERT(!emul);

	// setup global emulator pointer
	emul = new Emulator(*argc, argv);
}

extern "C" void emul_cleanup()
{
	ASSERT(emul);
	delete emul;
	emul = NULL;
}

extern "C" void emul_idle()
{
	// We process GUI events when the application is idle.
	emul->emulApp->processEvents();
}


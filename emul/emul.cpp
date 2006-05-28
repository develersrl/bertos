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
 *#* Revision 1.6  2006/05/28 12:17:57  bernie
 *#* Drop almost all the Qt3 cruft.
 *#*
 *#* Revision 1.5  2006/05/27 22:30:56  bernie
 *#* Add some delay to slow things down.
 *#*
 *#* Revision 1.4  2006/02/24 01:35:40  bernie
 *#* Update for new emulator.
 *#*
 *#* Revision 1.3  2006/02/20 02:00:40  bernie
 *#* Port to Qt 4.1.
 *#*
 *#* Revision 1.2  2006/01/16 03:51:51  bernie
 *#* Fix boilerplate.
 *#*
 *#* Revision 1.1  2006/01/16 03:37:12  bernie
 *#* Add emulator skeleton.
 *#*
 *#*/

#include "emul.h"
#include "emulwin.h"
#include <cfg/module.h>

#include <appconfig.h>
#if CONFIG_KERNEL
	#include <config_kern.h>
#endif


#include <cstdlib> // std::exit()

#include <QtGui/qapplication.h>


/// The global emulator instance.
Emulator *emul;

#if CONFIG_KERNEL
	#include <mware/list.h>

	/// List of process stacks
	List StackFreeList;

	// HACK: Reserve 64KB of stack space for kernel processes
	const int NPROC = 8;
	int stacks[NPROC][(64 * 1024) / sizeof(int)];
#endif

Emulator::Emulator(int &argc, char **argv) :
	emulApp(new QApplication(argc, argv)),
	emulWin(new EmulWin(this))
{
	emulWin->show();
}


Emulator::~Emulator()
{
	// we don't delete emulWin because it automatically
	// deletes itself when closed
	delete emulApp;
}


NORETURN void Emulator::quit()
{
	// WHAT A KLUDGE!
	this->~Emulator();
	emul = NULL;

	// do we have a better way to shut down the emulation?
	exit(0);
}

MOD_DEFINE(emul)

/// Main emulator entry point.
extern "C" void emul_init(int *argc, char *argv[])
{
	// setup global emulator pointer
	emul = new Emulator(*argc, argv);

#if CONFIG_KERNEL
	LIST_INIT(&StackFreeList);
	for (int i = 0; i < NPROC; i++)
		ADDTAIL(&StackFreeList, (Node *)stacks[i]);
#endif

	MOD_INIT(emul);
}

extern "C" void emul_cleanup()
{
	MOD_CLEANUP(emul);

	delete emul;
	emul = NULL;
}

extern "C" void emul_idle()
{
	// We process GUI events when the application is idle.
	emul->emulApp->processEvents();
	usleep(1000);
}


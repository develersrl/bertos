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
 * \brief Qt-based emulator framework for embedded applications (interface)
 */

/*#*
 *#* $Log$
 *#* Revision 1.3  2006/01/23 23:12:08  bernie
 *#* Let Doxygen see through C++ protected section.
 *#*
 *#* Revision 1.2  2006/01/16 03:51:51  bernie
 *#* Fix boilerplate.
 *#*
 *#* Revision 1.1  2006/01/16 03:37:12  bernie
 *#* Add emulator skeleton.
 *#*
 *#*/

#ifndef EMUL_EMUL_H
#define EMUL_EMUL_H

#include <cfg/compiler.h>

#if defined(__cplusplus) || defined(__doxygen__)

// fwd decls
class QApplication;
class EmulWin;
class EmulPRT;
class EmulLCD;
class EmulKBD;
class QCheckBox;
class QSlider;
class QLabel;

class Emulator
{
// data members
public:
	QApplication	*emulApp;	///< QT Application.
	EmulWin			*emulWin;	///< Main window.

	EmulLCD			*emulLCD;	///< Display emulator.
	EmulKBD			*emulKBD;	///< Keyboard emulator.

// construction
	Emulator(int &argc, char **argv);
	~Emulator();

// public methods
	int exec(void (*entry)(void));
	void quit();
};

extern Emulator *emul;

#endif /* __cplusplus */

EXTERN_C void emul_init(int *argc, char *argv[]);
EXTERN_C void emul_cleanup();
EXTERN_C void emul_idle();

#endif /* EMUL_EMUL_H */


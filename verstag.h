/*!
 * \file
 * <!--
 * Copyright 2003,2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2001,2002,2003 by Bernardo Innocenti <bernie@codewiz.org>
 * This file is part of DevLib - See devlib/README for information.
 * -->
 *
 * \version $Id$
 *
 * \author Bernardo Innocenti <bernie@develer.com>
 *
 * \brief Define version strings
 */
#ifndef VERSTAG_H
#define VERSTAG_H

#ifndef ARCH_CONFIG_H
	#include "arch_config.h"
#endif

#if (ARCH & ARCH_FOO)
	#define VERS_MAJOR 0
	#define VERS_MINOR 1
	#define VERS_REV   0
	#define VERS_LETTER ""
#elif (ARCH & ARCH_BAR)
	#define VERS_MAJOR 0
	#define VERS_MINOR 1
	#define VERS_REV   0
	#define VERS_LETTER ""
#else
	#error unknown architecture
#endif

#ifdef _DEBUG
	#define VERS_DBG "D"
#else
	#define VERS_DBG ""
#endif

#define _MAKE_VERS(maj,min,rev)	#maj "." #min "." #rev VERS_LETTER VERS_DBG
#define MAKE_VERS(maj,min,rev)	_MAKE_VERS(maj,min,rev)

#define VERSION_TAG MAKE_VERS(VERS_MAJOR,VERS_MINOR,VERS_REV)

#endif /* VERSTAG_H */

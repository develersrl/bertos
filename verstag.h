/*!
 * \file
 * <!--
 * Copyright 2003, 2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2001, 2002, 2003 by Bernardo Innocenti <bernie@codewiz.org>
 * This file is part of DevLib - See devlib/README for information.
 * -->
 *
 * \version $Id$
 *
 * \author Bernardo Innocenti <bernie@develer.com>
 *
 * \brief Define version strings
 */
#ifndef DEVLIB_VERSTAG_H
#define DEVLIB_VERSTAG_H

#ifndef ARCH_CONFIG_H
	#include "arch_config.h"
#endif

#define APP_NAME "Appname"
#define APP_DESCRIPTION "Long application name description"
#define APP_AUTHOR "Develer"
#define APP_COPYRIGHT "Copyright (C) 2004 by Develer (http://www.develer.com/)"

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

#define _STRINGIZE(a) #a
/*! Build application version string (i.e.: "1.7.0") */
#define MAKE_VERS(maj,min,rev) _STRINGIZE(maj) "." _STRINGIZE(min) "." _STRINGIZE(rev) VERS_LETTER VERS_DBG
#define VERSION_TAG MAKE_VERS(VERS_MAJOR,VERS_MINOR,VERS_REV)

/*! Build application version string suitable for MS windows resource files (i.e.: "1, 7, 0, 1") */
#define MAKE_RCVERS(maj,min,rev,bld) _STRINGIZE(maj) ", " _STRINGIZE(min) ", " _STRINGIZE(rev) ", " _STRINGIZE(bld)
#define RCVERSION_TAG MAKE_VERS(VERS_MAJOR,VERS_MINOR,VERS_REV)

#endif /* DEVLIB_VERSTAG_H */

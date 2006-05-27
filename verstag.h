/**
 * \file
 * <!--
 * Copyright 2003, 2004, 2005, 2006 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2001, 2002, 2003 by Bernardo Innocenti <bernie@codewiz.org>
 * This file is part of DevLib - See README.devlib for information.
 * -->
 *
 * \version $Id$
 *
 * \author Bernardo Innocenti <bernie@develer.com>
 *
 * \brief Declare application version strings
 */
#ifndef DEVLIB_VERSTAG_H
#define DEVLIB_VERSTAG_H

#ifndef ARCH_CONFIG_H
	#include "cfg/arch_config.h"
#endif

#define APP_NAME "Appname"
#define APP_DESCRIPTION "Long application name description"
#define APP_AUTHOR "Develer"
#define APP_COPYRIGHT "Copyright 2006 Develer (http://www.develer.com/)"

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

/*!
 * If _SNAPSHOT is defined, \c VERS_TAG contains the build date
 * date instead of a numeric version string.
 */
#define _SNAPSHOT

#ifdef _DEBUG
	#define VERS_DBG "D"
#else
	#define VERS_DBG ""
#endif

#define __STRINGIZE(x) #x
#define _STRINGIZE(x) __STRINGIZE(x)

/*! Build application version string (i.e.: "1.7.0") */
#define MAKE_VERS(maj,min,rev) _STRINGIZE(maj) "." _STRINGIZE(min) "." _STRINGIZE(rev) VERS_LETTER VERS_DBG
#ifdef _SNAPSHOT
	#define VERS_TAG "snapshot" " " __DATE__ " " __TIME__ " " VERS_LETTER " " VERS_DBG
#else
	#define VERS_TAG MAKE_VERS(VERS_MAJOR,VERS_MINOR,VERS_REV)
#endif

/*! Build application version string suitable for MS windows resource files (i.e.: "1, 7, 0, 1") */
#define MAKE_RCVERS(maj,min,rev,bld) _STRINGIZE(maj) ", " _STRINGIZE(min) ", " _STRINGIZE(rev) ", " _STRINGIZE(bld)
#define RCVERSION_TAG MAKE_VERS(VERS_MAJOR,VERS_MINOR,VERS_REV)

/*! The revision string (contains VERS_TAG) */
extern const char vers_tag[];

/*! Sequential build number (contains VERS_BUILD) */
extern const int vers_build_nr;
//extern const char vers_build_str[];

/*! Hostname of the machine used to build this binary (contains VERS_HOST) */
extern const char vers_host[];

#endif /* DEVLIB_VERSTAG_H */

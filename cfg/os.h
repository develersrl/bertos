/*!
 * \file
 * <!--
 * Copyright 2004 Develer S.r.l. (http://www.develer.com/)
 * This file is part of DevLib - See README.devlib for information.
 * -->
 *
 * \brief OS-specific definitions
 *
 * \version $Id$
 *
 * \author Bernardo Innocenti <bernie@develer.com>
 */

/*#*
 *#* $Log$
 *#* Revision 1.2  2005/11/04 16:20:01  bernie
 *#* Fix reference to README.devlib in header.
 *#*
 *#* Revision 1.1  2005/04/11 19:04:13  bernie
 *#* Move top-level headers to cfg/ subdir.
 *#*
 *#* Revision 1.1  2004/12/31 17:40:24  bernie
 *#* Add OS detection code.
 *#*
 *#*/
#ifndef DEVLIB_OS_H
#define DEVLIB_OS_H


/*! Macro to include OS-specific versions of the headers. */
#define OS_HEADER(module)  PP_STRINGIZE(PP_CAT3(module, _, OS_ID).h)

/*
 * OS autodetection (Some systems trigger multiple OS definitions)
 */
#ifdef _WIN32
	#define OS_WIN32  1
	#define OS_ID     win32
#else
	#define OS_WIN32  0
#endif

#ifdef __unix__
	#define OS_UNIX   1
	#define OS_POSIX  1  /* Not strictly UNIX, but no way to autodetect it. */
	#define OS_ID     unix
#else
	#define OS_UNIX   0
	#define OS_POSIX  0
#endif

#ifdef __linux__
	#define OS_LINUX  1
#else
	#define OS_LINUX  0
#endif

#if defined(__APPLE__) && defined(__MACH__)
	#define OS_DARWIN 1
#else
	#define OS_DARWIN 0
#endif

/*
 * Summarize hosted environments as OS_HOSTED.
 */
#if OS_WIN32 || OS_UNIX
	#define OS_HOSTED   1
#else
	#define OS_HOSTED   0
#endif

/*
 * Summarize embedded environments as OS_EMBEDDED.
 */
#if CPU_AVR || CPU_DSP56K || CPU_I196 || defined(__embedded__)
	#define OS_EMBEDDED  1
#else
	#define OS_EMBEDDED  0
#endif


/* Self-check for the detection */
#if CPU_I196 + CPU_X86 + CPU_DSP56K + CPU_AVR == 0
	#error Unknown CPU
#endif
#if !defined(OS_ID)
	#error OS_ID not defined
#endif
#if OS_HOSTED && OS_EMBEDDED
	#error Both hosted and embedded OS environment
#endif
#if !OS_HOSTED && !OS_EMBEDDED
	#error Neither hosted nor embedded OS environment
#endif

#endif /* DEVLIB_OS_H */

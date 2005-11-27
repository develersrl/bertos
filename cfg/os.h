/*!
 * \file
 * <!--
 * Copyright 2004, 2005 Develer S.r.l. (http://www.develer.com/)
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
 *#* Revision 1.4  2005/11/27 03:07:13  bernie
 *#* IRQ_SAVE_DISABLE(): Really block signals.
 *#*
 *#* Revision 1.3  2005/11/27 03:02:40  bernie
 *#* Add POSIX emulation for IRQ_* macros; Add Qt support.
 *#*
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
#define OS_CSOURCE(module) PP_STRINGIZE(PP_CAT3(module, _, OS_ID).c)

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
	#define OS_ID     posix

	/*
	 * The POSIX moral equivalent of disabling IRQs is disabling signals.
	 */
	#define _XOPEN_SOURCE 600 /* Avoid BSDish stuff */
	#undef _GNU_SOURCE /* This implies _BSD_SOURCE and is predefined on Linux. */
	#include <signal.h>
	typedef sigset_t cpuflags_t;
	#define IRQ_DISABLE \
	do { \
		sigset_t sigs; \
		sigfillset(&sigs); \
		sigprocmask(SIG_BLOCK, &sigs, NULL); \
	} while (0)

	#define IRQ_ENABLE \
	do { \
		sigset_t sigs; \
		sigemptyset(&sigs); \
		sigprocmask(SIG_UNBLOCK, &sigs, NULL); \
	} while (0)

	#define IRQ_SAVE_DISABLE(old_sigs) \
	do { \
		sigset_t sigs; \
		sigemptyset(&sigs); \
		sigprocmask(SIG_BLOCK, &sigs, &old_sigs); \
	} while (0)

	#define IRQ_RESTORE(old_sigs) \
	do { \
		sigprocmask(SIG_SETMASK, &old_sigs, NULL); \
	} while (0)
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
 * We want Qt and other frameworks to look like OSes because you would
 * tipically want their portable abstractions if you're using one of these.
 */
#if defined(_QT)
	#define OS_QT 1
	#undef  OS_ID
	#define OS_ID qt
#else
	#define OS_QT 0
#endif



/*
 * Summarize hosted environments as OS_HOSTED and embedded
 * environment with OS_EMBEDDED.
 */
#if OS_WIN32 || OS_UNIX
	#define OS_HOSTED   1
	#define OS_EMBEDDED 0
#else
	#define OS_HOSTED   0
	#define OS_EMBEDDED 1
#endif

/* Self-check for the detection */
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

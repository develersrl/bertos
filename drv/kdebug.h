/*!
 * \file
 * <!--
 * Copyright 2003,2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2000,2001,2002 Bernardo Innocenti <bernie@codewiz.org>
 * This file is part of DevLib - See devlib/README for information.
 * -->
 *
 * \brief Definition of handy debug macros.  These macros are no-ops
 *        when the preprocessor symbol _DEBUG isn't defined.
 *
 * \version $Id$
 *
 * \author Bernardo Innocenti <bernie@develer.com>
 */

/*
 * $Log$
 * Revision 1.4  2004/07/30 14:26:33  rasky
 * Semplificato l'output dell'ASSERT
 * Aggiunta ASSERT2 con stringa di help opzionalmente disattivabile
 *
 * Revision 1.3  2004/06/03 11:27:09  bernie
 * Add dual-license information.
 *
 * Revision 1.2  2004/05/23 18:21:53  bernie
 * Trim CVS logs and cleanup header info.
 *
 * Revision 1.1  2004/05/23 18:10:11  bernie
 * Import drv/ modules.
 *
 */
#ifndef KDEBUG_H
#define KDEBUG_H

#include "compiler.h"

/* Avoid name clashes with Win32 headers */
#ifdef ASSERT
#undef ASSERT
#endif

#if defined(_DEBUG)
	void kdbg_init(void);
	void kdump(const void *buf, size_t len);

	#ifdef __AVR__

		#include <avr/pgmspace.h>
		void kputs_P(const char *PROGMEM str);
		void kprintf_P(const char *PROGMEM fmt, ...) FORMAT(__printf__, 1, 2);
		int __assert_P(const char *PROGMEM cond, const char *PROGMEM file, int line);
		int __invalid_ptr_P(void *p, const char *PROGMEM name, const char *PROGMEM file, int line);
		#define kputs(str)  kputs_P(PSTR(str))
		#define kprintf(fmt, ...)  kprintf_P(PSTR(fmt) ,## __VA_ARGS__)
		#define __assert(cond, file, line)  __assert_P(PSTR(cond), PSTR(file), (line))
		#define __invalid_ptr(p, name, file, line)  __invalid_ptr_P((p), PSTR(name), PSTR(file), (line))

	#else
		void kputs(const char *str);
		void kprintf(const char * fmt, ...) FORMAT(__printf__, 1, 2);
		int __assert(const char *cond, const char *file, int line);
		int __invalid_ptr(void *p, const char *name, const char *file, int line);
	#endif

	void __init_wall(long *wall, int size);
	int __check_wall(long *wall, int size, const char *name, const char *file, int line);

	#define THIS_FILE                       __FILE__
	#ifndef CONFIG_KDEBUG_ASSERT_NO_TEXT
		#define ASSERT(x)                   ((x) ? 0 : __assert(#x, THIS_FILE, __LINE__))
		#define ASSERT2(x, help)            ((x) ? 0 : __assert(help " (" #x ")", THIS_FILE, __LINE__))
	#else
		#define ASSERT(x)                   ((x) ? 0 : __assert("", THIS_FILE, __LINE__))
		#define ASSERT2(x, help)            ASSERT(x)
	#endif

	#define ASSERT_VALID_PTR(p)             ((p >= 0x200) ? 0 : __invalid_ptr(p, #p, THIS_FILE, __LINE__))
	#define ASSERT_VALID_PTR_OR_NULL(p)     (((p == NULL) || (p >= 0x200)) ? 0 : __invalid_ptr(p, #p, THIS_FILE, __LINE__))
	#define TRACE                           kprintf("%s()\n", __FUNCTION__)
	#define TRACEMSG(msg,...)               kprintf("%s(): " msg "\n", __FUNCTION__, ## __VA_ARGS__)
	#define DB(x)                           x

	/* Walls to detect data corruption */
	#define WALL_SIZE                       8
	#define WALL_VALUE                      (long)0xABADCAFEL
	#define DECLARE_WALL(name,size)         static long name[(size) / sizeof(long)];
	#define INIT_WALL(name)                 __init_wall((name), countof(name))
	#define CHECK_WALL(name)                __check_wall((name), countof(name), #name, THIS_FILE, __LINE__)

#else /* !_DEBUG */

	#define ASSERT(x)
	#define ASSERT2(x, help)
	#define ASSERT_VALID_PTR(p)
	#define ASSERT_VALID_PTR_OR_NULL(p)
	#define TRACE                           do {} while(0)
	#define TRACEMSG(x,...)                 do {} while(0)
	#define DB(x)

	#define DECLARE_WALL(name, size)
	#define INIT_WALL(name)
	#define CHECK_WALL(name)

	INLINE void kdbg_init(void) {}
	INLINE void kprintf(UNUSED(const char*, fmt), ...) {}

#endif /* !_DEBUG */

#endif /* KDEBUG_H */

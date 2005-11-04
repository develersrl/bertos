/*!
 * \file
 * <!--
 * Copyright 2003, 2004, 2005 Develer S.r.l. (http://www.develer.com/)
 * This file is part of DevLib - See README.devlib for information.
 * -->
 *
 * \brief Simple debug facilities for hosted and embedded C/C++ applications.
 *
 * Debug output goes to stderr in hosted applications.
 * Freestanding (AKA embedded) applications use \c drv/kdebug.c to output
 * diagnostic messages to a serial terminal or a JTAG debugger.
 *
 * \version $Id$
 * \author Bernardo Innocenti <bernie@develer.com>
 */

/*#*
 *#* $Log$
 *#* Revision 1.5  2005/11/04 16:09:03  bernie
 *#* Doxygen workaround.
 *#*
 *#* Revision 1.4  2005/07/03 15:18:52  bernie
 *#* Typo.
 *#*
 *#* Revision 1.3  2005/06/27 21:23:55  bernie
 *#* Rename cfg/config.h to appconfig.h.
 *#*
 *#* Revision 1.2  2005/04/11 19:10:27  bernie
 *#* Include top-level headers from cfg/ subdir.
 *#*
 *#* Revision 1.1  2005/04/11 19:04:13  bernie
 *#* Move top-level headers to cfg/ subdir.
 *#*
 *#* Revision 1.13  2005/03/01 23:23:58  bernie
 *#* Provide defaults for CONFIG_KDEBUG_DISABLE_TRACE and CONFIG_KDEBUG_ASSERT_NO_TEXT.
 *#*
 *#* Revision 1.12  2005/02/18 11:18:33  bernie
 *#* Fixes for Harvard processors from project_ks.
 *#*
 *#* Revision 1.11  2005/02/16 20:29:48  bernie
 *#* TRACE(), TRACEMSG(): Reduce code and data footprint.
 *#*
 *#* Revision 1.10  2005/02/09 21:50:28  bernie
 *#* Declare dummy ASSERT* macros as ((void)0) to work around a warning I can't remember any more.
 *#*
 *#* Revision 1.9  2005/01/25 08:36:40  bernie
 *#* kputnum(): Export.
 *#*
 *#* Revision 1.8  2005/01/11 18:08:08  aleph
 *#* Add empty kdump definition for debug off
 *#*
 *#* Revision 1.7  2004/12/31 17:43:09  bernie
 *#* Use UNUSED_ARG instead of obsolete UNUSED macro.
 *#*
 *#* Revision 1.6  2004/12/08 08:52:00  bernie
 *#* Save some more RAM on AVR.
 *#*/
#ifndef DEVLIB_DEBUG_H
#define DEVLIB_DEBUG_H

/*
 * Try to guess whether we're running in a hosted or embedded environment.
 */
#ifndef OS_HOSTED
	#if defined(__unix__) || defined(_WIN32)
		#define OS_HOSTED  1
	#else
		#define OS_HOSTED  0
	#endif
#endif /* !OS_HOSTED */

#if OS_HOSTED
	/*
	 * For now, avoid dependency on compiler.h
	 */
	#ifndef FORMAT
	#define FORMAT(x,y,z) /* nothing */
	#endif
	#ifndef INLINE
	#define INLINE static inline
	#endif
	#ifndef UNUSED_ARG
	#ifdef __cplusplus
		#define UNUSED_ARG(type,name) type
	#else
		#define UNUSED_ARG(type,name) type name
	#endif
	#endif
#else /* !OS_HOSTED */
	#include <cfg/compiler.h>
#endif /* !OS_HOSTED */


/*
 * Defaults for rarely used config stuff.
 */
#ifndef CONFIG_KDEBUG_DISABLE_TRACE
#define CONFIG_KDEBUG_DISABLE_TRACE  0
#endif

#ifndef CONFIG_KDEBUG_ASSERT_NO_TEXT
#define CONFIG_KDEBUG_ASSERT_NO_TEXT  0
#endif


/*!
 * \def _DEBUG
 *
 * This preprocessor symbol is defined only in debug builds.
 *
 * The build infrastructure must arrange for _DEBUG to
 * be predefined for all source files being compiled.
 *
 * This is compatible with the Microsoft convention for
 * the default Debug and Release targets.
 */
#ifdef _DEBUG

	// STLport specific: enable extra checks
	#define __STL_DEBUG 1

	// MSVC specific: Enable memory allocation debug
	#if defined(_MSC_VER)
		#include <crtdbg.h>
	#endif

	/*
	 * On UNIX systems the extabilished practice is to define
	 * NDEBUG for release builds and nothing for debug builds.
	 */
	#ifdef NDEBUG
	#undef NDEBUG
	#endif

	/*!
	 * This macro duplicates the old MSVC trick of redefining
	 * THIS_FILE locally to avoid the overhead of many duplicate
	 * strings in ASSERT().
	 */
	#ifndef THIS_FILE
		#define THIS_FILE  __FILE__
	#endif

	/*!
	 * This macro can be used to conditionally exclude one or more
	 * statements conditioned on \c _DEBUG, avoiding the clutter
	 * of ifdef/endif pairs.
	 *
	 * \code
	 *     struct FooBar
	 *     {
	 *         int foo;
	 *         bool bar;
	 *         DB(int ref_count;) // Track number of users
	 *
	 *         void release()
	 *         {
         *             DB(--ref_count;)
	 *         }
	 *     };
	 * \endcode
	 */
	#define DB(x) x

	#if OS_HOSTED
		#include <stdio.h>
		#include <stdarg.h>

		INLINE void kdbg_init(void) { /* nop */ }
		INLINE void kputchar(char c)
		{
			putc(c, stderr);
		}
		INLINE void kputs(const char *str)
		{
			fputs(str, stderr);
		}
		/* G++ can't inline functions with variable arguments... */
		#define kprintf(fmt, ...) fprintf(stderr, fmt, ## __VA_ARGS__)
		void kdump(const void *buf, size_t len); /* UNIMPLEMENTED */

		#ifndef ASSERT
			#include <assert.h>
			#define ASSERT(x) assert(x)
		#endif /* ASSERT */

		/*!
		 * Check that the given pointer is not NULL or pointing to raw memory.
		 *
		 * The assumption here is that valid pointers never point to low
		 * memory regions.  This helps catching pointers taken from
		 * struct/class memebers when the struct pointer was NULL.
		 */
		#define ASSERT_VALID_PTR(p)  ASSERT((unsigned long)(p) > 0x200)

		#define ASSERT_VALID_PTR_OR_NULL(p)  ASSERT((((p) == NULL) || ((unsigned long)(p) >= 0x200)))
	#else /* !OS_HOSTED */

		#include <appconfig.h>  /* CONFIG_KDEBUG_ASSERT_NO_TEXT */
		#include <cfg/cpu.h>  /* CPU_HARVARD */

		/* These are implemented in drv/kdebug.c */
		void kdbg_init(void);
		void kputchar(char c);
		int kputnum(int num);
		void kdump(const void *buf, size_t len);
		void __init_wall(long *wall, int size);

		#if CPU_HARVARD
			#include <mware/pgm.h>
			void kputs_P(const char *PROGMEM str);
			void kprintf_P(const char *PROGMEM fmt, ...) FORMAT(__printf__, 1, 2);
			int __assert_P(const char *PROGMEM cond, const char *PROGMEM file, int line);
			void __trace_P(const char *func);
			void __tracemsg_P(const char *func, const char *PROGMEM fmt, ...);
			int __invalid_ptr_P(void *p, const char *PROGMEM name, const char *PROGMEM file, int line);
			int __check_wall_P(long *wall, int size, const char * PGM_ATTR name, const char * PGM_ATTR file, int line);
			#define kputs(str)  kputs_P(PSTR(str))
			#define kprintf(fmt, ...)  kprintf_P(PSTR(fmt) ,## __VA_ARGS__)
			#define __assert(cond, file, line)  __assert_P(PSTR(cond), PSTR(file), (line))
			#define __trace(func)  __trace_P(func)
			#define __tracemsg(func, fmt, ...)  __tracemsg_P(func, PSTR(fmt), ## __VA_ARGS__)
			#define __invalid_ptr(p, name, file, line)  __invalid_ptr_P((p), PSTR(name), PSTR(file), (line))
			#define __check_wall(wall, size, name, file, line)  __check_wall_P(wall, size, PSTR(name), PSTR(file), (line))
		#else /* !CPU_HARVARD */
			void kputs(const char *str);
			void kprintf(const char *fmt, ...) FORMAT(__printf__, 1, 2);
			int __assert(const char *cond, const char *file, int line);
			void __trace(const char *func);
			void __tracemsg(const char *func, const char *fmt, ...) FORMAT(__printf__, 2, 3);
			int __invalid_ptr(void *p, const char *name, const char *file, int line);
			int __check_wall(long *wall, int size, const char *name, const char *file, int line);
		#endif /* !CPU_HARVARD */

		#if !CONFIG_KDEBUG_ASSERT_NO_TEXT
			#define ASSERT(x)         ((void)(LIKELY(x) ? 0 : __assert(#x, THIS_FILE, __LINE__)))
			#define ASSERT2(x, help)  ((void)(LIKELY(x) ? 0 : __assert(help " (" #x ")", THIS_FILE, __LINE__)))
		#else
			#define ASSERT(x)         ((void)(LIKELY(x) ? 0 : __assert("", THIS_FILE, __LINE__)))
			#define ASSERT2(x, help)  ((void)ASSERT(x))
		#endif

		#define ASSERT_VALID_PTR(p)         ((void)(LIKELY((p) >= 0x200) ? 0 : __invalid_ptr(p, #p, THIS_FILE, __LINE__)))
		#define ASSERT_VALID_PTR_OR_NULL(p) ((void)(LIKELY((p == NULL) || ((p) >= 0x200)) ? 0 : __invalid_ptr((p), #p, THIS_FILE, __LINE__)))

		#if !CONFIG_KDEBUG_DISABLE_TRACE
			#define TRACE  __trace(__func__)
			#define TRACEMSG(msg,...) __tracemsg(__func__, msg, ## __VA_ARGS__)
		#else
			#define TRACE  do {} while(0)
			#define TRACEMSG(...)  do {} while(0)
		#endif

	#endif /* !OS_HOSTED */

	/*!
	 * \name Walls to detect data corruption
	 * \{
	 */
	#define WALL_SIZE                    8
	#define WALL_VALUE                   (long)0xABADCAFEL
	#define DECLARE_WALL(name,size)      long name[(size) / sizeof(long)];
	#define FWD_DECLARE_WALL(name,size)  extern long name[(size) / sizeof(long)];
	#define INIT_WALL(name)              __init_wall((name), countof(name))
	#define CHECK_WALL(name)             __check_wall((name), countof(name), #name, THIS_FILE, __LINE__)
	/*\}*/

	/*!
	 * Check that the given pointer actually points to an object
	 * of the specified type.
	 */
	#define ASSERT_VALID_OBJ(_t, _o) do { \
		ASSERT_VALID_PTR((_o)); \
		ASSERT(dynamic_cast<_t>((_o)) != NULL); \
	}

	/*!
	 * \name Debug object creation and destruction.
	 *
	 * These macros help track some kinds of leaks in C++ programs.
	 * Usage is as follows:
	 *
	 * \code
	 *   class Foo
	 *   {
	 *       DECLARE_INSTANCE_TRACKING(Foo)
	 *
	 *       Foo()
	 *       {
	 *           NEW_INSTANCE(Foo);
	 *           // ...
	 *       }
	 *
	 *       ~Foo()
	 *       {
	 *           DELETE_INSTANCE(Foo);
	 *           // ...
	 *       }
	 *   };
	 *
	 *   // Put this in the implementation file of the class
	 *   IMPLEMENT_INSTANCE_TRACKING(Foo)
	 *
	 *   // Client code
	 *   int main(void)
	 *   {
	 *        Foo *foo = new Foo;
	 *        cout << GET_INSTANCE_COUNT(Foo) << endl; // prints "1"
	 *        delete foo;
	 *        ASSERT_ZERO_INSTANCES(Foo); // OK
	 *   }
	 * \endcode
	 * \{
	 */
	#define NEW_INSTANCE(CLASS)                do { ++CLASS::__instances } while (0)
	#define DELETE_INSTANCE(CLASS)             do { --CLASS::__instances } while (0)
	#define ASSERT_ZERO_INSTANCES(CLASS)       ASSERT(CLASS::__instances == 0)
	#define GET_INSTANCE_COUNT(CLASS)          (CLASS::__instances)
	#define DECLARE_INSTANCE_TRACKING(CLASS)   static int __instances
	#define IMPLEMENT_INSTANCE_TRACKING(CLASS) int CLASS::__instances = 0
	/*\}*/

#else /* !_DEBUG */

	/*
	 * On UNIX systems the extabilished practice is to define
	 * NDEBUG for release builds and nothing for debug builds.
	 */
	#ifndef NDEBUG
	#define NDEBUG 1
	#endif

	#define DB(x)  /* nothing */
	#ifndef ASSERT
		#define ASSERT(x)  ((void)0)
	#endif /* ASSERT */
	#define ASSERT2(x, help)             ((void)0)
	#define ASSERT_VALID_PTR(p)          ((void)0)
	#define ASSERT_VALID_PTR_OR_NULL(p)  ((void)0)
	#define ASSERT_VALID_OBJ(_t, _o)     ((void)0)
	#define TRACE                        do {} while (0)
	#define TRACEMSG(x,...)              do {} while (0)

	#define DECLARE_WALL(name, size)     /* nothing */
	#define FWD_DECLARE_WALL(name, size) /* nothing */
	#define INIT_WALL(name)              do {} while (0)
	#define CHECK_WALL(name)             do {} while (0)

	#define NEW_INSTANCE(CLASS)                do {} while (0)
	#define DELETE_INSTANCE(CLASS)             do {} while (0)
	#define ASSERT_ZERO_INSTANCES(CLASS)       do {} while (0)
	#define GET_INSTANCE_COUNT(CLASS)          ERROR_ONLY_FOR_DEBUG
	#define DECLARE_INSTANCE_TRACKING(CLASS)
	#define IMPLEMENT_INSTANCE_TRACKING(CLASS)

	INLINE void kdbg_init(void) { /* nop */ }
	INLINE void kputchar(UNUSED_ARG(char, c)) { /* nop */ }
	INLINE int kputnum(UNUSED_ARG(int, num)) { return 0; }
	INLINE void kputs(UNUSED_ARG(const char *, str)) { /* nop */ }
	INLINE void kdump(UNUSED_ARG(const void *, buf), UNUSED_ARG(size_t, len)) { /* nop */ }

	#ifdef __cplusplus
		/* G++ can't inline functions with variable arguments... */
		#define kprintf(fmt,...) do { (void)(fmt); } while(0)
	#else
		/* ...but GCC can. */
		INLINE void kprintf(UNUSED_ARG(const char *, fmt), ...) { /* nop */ }
	#endif

#endif /* _DEBUG */

#endif /* DEVLIB_DEBUG_H */

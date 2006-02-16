/*!
 * \file
 * <!--
 * Copyright 2006 Develer S.r.l. (http://www.develer.com/)
 * This file is part of DevLib - See README.devlib for information.
 * -->
 *
 * \brief Debug macros for inter-module dependency checking.
 *
 * These macros expand to nothing in release builds.  In debug
 * builds, they perform run-time dependency checks for modules.
 *
 * The usage pattern looks like this:
 *
 * \code
 * MOD_DEFINE(phaser)
 *
 * void phaser_init(void)
 * {
 *     MOD_CHECK(computer);
 *     MOD_CHECK(warp_core);
 *
 *    [...charge weapons...]
 *
 *    MOD_INIT(phaser);
 * }
 *
 * void phaser_cleanup(void)
 * {
 *    MOD_CLEANUP(phaser);
 *
 *    [...disarm phaser...]
 * }
 * \endcode
 *
 * \version $Id$
 * \author Bernardo Innocenti <bernie@develer.com>
 */
#ifndef CFG_MODULE_H
#define CFG_MODULE_H

#include <cfg/debug.h>

/**
 * Declare a global variable for module dependency check.
 *
 * \see MOD_INIT(), MOD_CHECK()
 */
#define MOD_DEFINE(module)   DB(bool module ## _initialized;)

/**
 * Check that \a module was already initialized.
 *
 * Put this check just before accessing any facility
 * provided by a module that requires prior initialization.
 *
 * \see MOD_INIT()
 */

#define MOD_CHECK(module) \
do { \
	DB(extern bool module ## _initialized;) \
	ASSERT(module ## _initialized); \
} while (0)

/**
 * Mark module as initialized.
 *
 * Marking initialization requires the global data
 * previously defined by MOD_DEFINE().
 *
 * To prevent double initialization bugs, an initialized
 * module must first be cleaned up with MOD_CLEANUP() before
 * calling MOD_INIT() another time.
 *
 * \see MOD_CLEANUP(), MOD_CHECK(), MOD_DEFINE()
 */
#define MOD_INIT(module) \
do { \
	ASSERT(!module ## _initialized); \
	DB(module ## _initialized = true;) \
} while (0)

/**
 * Mark module as being no longer initialized.
 *
 * Marking initialization requires the global data
 * previously defined by MOD_DEFINE().
 *
 * \see MOD_INIT(), MOD_CHECK(), MOD_DEFINE()
 */
#define MOD_CLEANUP(module) \
do { \
	ASSERT(module ## _initialized); \
	DB(module ## _initialized = false;) \
} while (0)

#endif /* CFG_MODULE_H */


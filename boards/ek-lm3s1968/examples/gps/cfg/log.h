/**
 * \file
 * <!--
 * This file is part of BeRTOS.
 *
 * Bertos is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * As a special exception, you may use this file as part of a free software
 * library without restriction.  Specifically, if other files instantiate
 * templates or use macros or inline functions from this file, or you compile
 * this file and link it with other files to produce an executable, this
 * file does not by itself cause the resulting executable to be covered by
 * the GNU General Public License.  This exception does not however
 * invalidate any other reasons why the executable file might be covered by
 * the GNU General Public License.
 *
 * Copyright 2008 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief Logging system module.
 *
 * This module implement a simple interface to use the multi level logging system.
 * The log message have the priority order, like this:
 *
 *  - error message (highest)
 *  - warning message
 *  - info message (lowest)
 *
 * With this priority system we can log only the message that have egual or major
 * priority than log level that you has been configurate. Further you can have a
 * differ log level for each module that you want. To do this you just need to
 * define LOG_LEVEL in cfg of select module.
 * When you set a log level, the system logs only the message that have priority
 * egual or major that you have define, but the other logs function are not include
 * at compile time, so all used logs function are linked, but the other no.
 *
 * To use logging system you should include this module in your drive and use
 * a LOG_ERROR, LOG_WARNING and LOG_INFO macros to set the level log of the message.
 * Then you should define a LOG_LEVEL and LOG_VERBOSE costant in your
 * \c cfg/cfg_\<your_cfg_module_name\>.h using the follow policy:
 *
 * - in your file \c cfg/cfg_\<cfg_module_name\>.h, you define the logging
 *   level and verbosity mode for your specific module:
 *
 * \code
 *	/// Module logging level.
 *	#define <cfg_module_name>_LOG_LEVEL    LOG_LVL_INFO
 *
 *	/// Module logging format.
 *	#define <cfg_module_name>_LOG_FORMAT   LOG_FMT_VERBOSE
 * \endcode
 *
 * - then, in the module that you use a logging macros you should define
 *   a LOG_LEVEL and LOG_FORMAT using the previous value that you have define
 *   in cfg_<cfg_module_name>.h header. After this you should include the cfg/log.h
 *   module:
 *
 * \code
 *	// Define log settings for cfg/log.h.
 *	#define LOG_LEVEL   <cfg_module_name>_LOG_LEVEL
 *	#define LOG_FORMAT  <cfg_module_name>_LOG_FORMAT
 *	#include <cfg/log.h>
 * \endcode
 *
 * if you include a log.h module without define the LOG_LEVEL and LOG_VERBOSE
 * macros, the module use the default setting (see below).
 *
 * WARNING: when use the log.h module, and you want to set a your log level
 * make sure to include this module after a \c cfg_<cfg_module_name>.h, because the
 * LOG_LEVEL and LOG_VERBOSE macros must be defined before to include log module,
 * otherwise the log module use a default settings.
 *
 * \version $Id$
 * \author Daniele Basile <asterix@develer.com>
 *
 * $WIZ$
 */

#ifndef CFG_LOG_H
#define CFG_LOG_H

#include <cfg/debug.h>


// Use a default setting if nobody defined a log level
#ifndef LOG_LEVEL
#define LOG_LEVEL       LOG_LVL_WARN
#endif

// Use a default setting if nobody defined a log format
#ifndef LOG_FORMAT
#define LOG_FORMAT      LOG_FMT_TERSE
#endif

/**
 * \name Logging level definition
 *
 * When you choose a log level messages you choose
 * also which print function are linked.
 * If you choose a low level of log you link all log function (error, warning and info),
 * but if choose a hight level you link only that have the priority egual or hight.
 * The priority level go from error (highest) to info (lowest) (see cfg/debug.h
 * for more detail).
 *
 * $WIZ$ log_level = "LOG_LVL_NONE", "LOG_LVL_ERR", "LOG_LVL_WARN", "LOG_LVL_INFO"
 * }
 */
#define LOG_LVL_NONE      0
#define LOG_LVL_ERR       1
#define LOG_LVL_WARN      2
#define LOG_LVL_INFO      3

/**
 * \name Logging format
 *
 * There are two logging format: terse and verbose.  The latter prepends
 * function names and line number information to each log entry.
 *
 * $WIZ$ log_format = "LOG_FMT_VERBOSE", "LOG_FMT_TERSE"
 */
#define LOG_FMT_VERBOSE   1
#define LOG_FMT_TERSE     0

#if LOG_FORMAT == LOG_FMT_VERBOSE
	#define LOG_PRINT(str_level, str,...)    kprintf("%s():%d:%s: " str, __func__, __LINE__, str_level, ## __VA_ARGS__)
#elif LOG_FORMAT == LOG_FMT_TERSE
	#define LOG_PRINT(str_level, str,...)    kprintf("%s: " str, str_level, ## __VA_ARGS__)
#else
	#error No LOG_FORMAT defined
#endif

#if LOG_LEVEL >= LOG_LVL_ERR
	#define LOG_ERR(str,...)       LOG_PRINT("ERR", str, ## __VA_ARGS__)
	#define LOG_ERRB(x)            x
#else
	INLINE void LOG_ERR(UNUSED_ARG(const char *, fmt), ...) { /* nop */ }
	#define LOG_ERRB(x)            /* Nothing */
#endif

#if LOG_LEVEL >= LOG_LVL_WARN
	#define LOG_WARN(str,...)       LOG_PRINT("WARN", str, ## __VA_ARGS__)
	#define LOG_WARNB(x)            x
#else
	INLINE void LOG_WARN(UNUSED_ARG(const char *, fmt), ...) { /* nop */ }
	#define LOG_WARNB(x)            /* Nothing */
#endif

#if LOG_LEVEL >= LOG_LVL_INFO
	#define LOG_INFO(str,...)       LOG_PRINT("INFO", str, ## __VA_ARGS__)
	#define LOG_INFOB(x)            x
#else
	INLINE void LOG_INFO(UNUSED_ARG(const char *, fmt), ...) { /* nop */ }
	#define LOG_INFOB(x)            /* Nothing */
#endif


#endif /* CFG_LOG_H */

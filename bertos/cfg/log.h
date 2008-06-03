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
 * All Rights Reserved.
 * -->
 *
 * \brief Logging system module.
 *
 * This module implement a simple interface to use the multi level
 * logging system.
 * The log message have the priority order, like this:
 *
 * - error message (most hight)
 * - warning message 
 * - info message (most low)
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
 * cfg/cfg_<your_module_name>.h using the follow police:
 * 
 * \code
 *
 *	/**
 *	 * Logging level definition.
 *	 *
 *	 * Use 0 to log only the error messages
 *	 * Use 1 to log the error and warning messages
 *	 * Use 2 to log all messages
 *	 *
 *	#define <module_name>_LOG_LEVEL      2
 *
 *	/**
 *	 * Set logging verbosity.
 *	 *
 *	 * If verbose is zero print short log messages.
 *	 *
 *	#define <module_name>_LOG_VERBOSE    1
 *
 *	// Define logging setting (for cfg/log.h module).
 *	#define LOG_LEVEL       <module_name>_LOG_LEVEL
 *	#define LOG_VERBOSE     <module_name>_LOG_VERBOSE
 *
 * \endcode
 *
 * if you include a log.h module without define the LOG_LEVEL and LOG_VERBOSE
 * macros, the module use the default setting (see below).
 *
 * WARNING: when use the log.h module, and you want to set a your log level
 * make sure to include this module after a cfg_<module_name>.h, because the
 * LOG_LEVEL and LOG_VERBOSE macros must be define before to include log module,
 * otherwise the log module use a default settings.
 *
 * \version $Id$
 *
 * \author Daniele Basile <asterix@develer.com>
 */

#ifndef CFG_LOG_H
#define CFG_LOG_H

#include <cfg/debug.h>

// Use a default setting if nobody have define a log level
#ifndef LOG_LEVEL
#define LOG_LEVEL   DBG_LOG_ERROR
#endif

// Use a default setting if nobody have define a log verbosity
#ifndef LOG_VERBOSE
#define LOG_VERBOSE   DBG_LOG_SILENT
#endif

/**
 *  Error log message.
 *  Use this macro to print error messages. This log level have hight priority,
 *  that means the error messages are always printed, if debug is enable.
 */
#define LOG_ERROR(str,...)      DBG_ERROR(LOG_LEVEL, LOG_VERBOSE, str, ## __VA_ARGS__)

/**
 * Log message level select.
 * When you choose a log level messages you choose
 * also which print function are linked.
 * If you choose a low level of log you link all log function (error, warning and info),
 * but if choose a hight level you link only that have the priority egual or hight.
 * The priority level go from error (most hight) to info (most low) (see cfg/debug.h 
 * for more detail).
 *
 */
#if (LOG_LEVEL <= DBG_LOG_INFO)
	#define LOG_WARNING(str,...)    DBG_WARNING(LOG_LEVEL, LOG_VERBOSE,  str, ## __VA_ARGS__)
	#define LOG_INFO(str,...)       DBG_INFO(LOG_LEVEL, LOG_VERBOSE, str, ## __VA_ARGS__)

#elif (LOG_LEVEL <= DBG_LOG_WARNING)
	#define LOG_WARNING(str,...)    DBG_WARNING(LOG_LEVEL, LOG_VERBOSE,  str, ## __VA_ARGS__)
	#define LOG_INFO(str,...)       /* Nothing */

#else /* LOG_LEVEL <= DBG_LOG_ERROR */
	#define LOG_WARNING(str,...)    /* Nothing */
	#define LOG_INFO(str,...)       /* Nothing */

#endif

#endif /* CFG_LOG_H */


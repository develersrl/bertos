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
 * Copyright 2001, 2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 1999, 2000, 2001, 2008 Bernie Innocenti <bernie@codewiz.org>
 * -->
 *
 * \brief Kernel configuration parameters
 *
 * \version $Id$
 * \author Bernie Innocenti <bernie@codewiz.org>
 */

#ifndef CFG_KERN_H
#define CFG_KERN_H

/**
 * Enable the multithreading kernel.
 *
 * $WIZARD = { "type" : "boolean" }
 */
#define CONFIG_KERN  1

#define CONFIG_KERN_SCHED       1  ///< Process schedling.                       $WIZARD = { "type" : "boolean" }
#define CONFIG_KERN_SIGNALS     1  ///< Inter-process signals.                   $WIZARD = { "type" : "boolean" }
#define CONFIG_KERN_IRQ         0  ///< Interrupt supervisor.                    $WIZARD = { "type" : "boolean" }
#define CONFIG_KERN_HEAP        0  ///< Dynamic memory allocation.               $WIZARD = { "type" : "boolean" }
#define CONFIG_KERN_SEMAPHORES  1  ///< Re-entrant mutual exclusion primitives.  $WIZARD = { "type" : "boolean" }
#define CONFIG_KERN_MONITOR     1  ///< Process monitor.                         $WIZARD = { "type" : "boolean" }
#define CONFIG_KERN_PREEMPT     0  ///< Preemptive process scheduling.           $WIZARD = { "type" : "boolean" }
#define CONFIG_KERN_PRI         0  ///< Priority-based scheduling policy.        $WIZARD = { "type" : "boolean" }

/**
 * [ms] Time sharing quantum (a prime number prevents interference effects)
 *
 * $WIZARD = { "type" : "int", "min" : "0" }
 */
#define CONFIG_KERN_QUANTUM     47

/**
 * Module logging level.
 *
 * $WIZARD = { "type" : "enum", "value_list" : "log_level" }
 */
#define KERN_LOG_LEVEL      LOG_LVL_ERR

/**
 * Module logging level.
 *
 * $WIZARD = { "type" : "enum", "value_list" : "log_level" }
 */
#define KERN_LOG_FORMAT     LOG_FMT_VERBOSE

#endif /*  CFG_KERN_H */

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
 * Copyright 2010 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief Recurrent task module.
 *
 * This module is a convenient method to handle multiple recurrent low priority
 * tasks. It creates a process with the default priority and schedules all the
 * tasks internally.
 * You can execute all the operations you want in each callback, since they
 * are executed in a different thread from the caller.
 *
 * Your callback may return true if you want the task to be scheduled
 * again, or false if you want the task to end.
 *
 * Interval time for each task should be fairly high (>20 ms) to avoid
 * blocking the whole CPU on this low priority job.
 *
 * \note rtask_add() may block.
 *
 * \author Luca Ottaviano <lottaviano@develer.com>
 * \author Francesco Sacchi <batt@develer.com>
 *
 */

#ifndef KERNEL_RTASK_H
#define KERNEL_RTASK_H

#include <cfg/compiler.h>

struct RTask;
typedef bool (*rtask_cb_t)(void *user_data);
typedef struct RTask RTask;

struct RTask *rtask_add(rtask_cb_t cb, mtime_t interval, void *cb_data);

/* Test functions */
int rtask_testRun(void);
int rtask_testSetup(void);
int rtask_testTearDown(void);

#endif /* KERNEL_RTASK_H */

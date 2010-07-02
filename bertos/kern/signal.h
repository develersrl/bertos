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
 * Copyright 2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 1999,2000,2001 Bernie Innocenti <bernie@codewiz.org>
 *
 * -->
 *
 * \brief Signal module for IPC.
 *
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 *
 * $WIZ$ module_name = "signal"
 * $WIZ$ module_depends = "kernel", "timer"
 * $WIZ$ module_configuration = "bertos/cfg/cfg_signal.h"
 */

#ifndef KERN_SIGNAL_H
#define KERN_SIGNAL_H

#include <cfg/compiler.h>
#include <cfg/macros.h>    // BV()

/* Fwd decl */
struct Process;

/* Inter-process Communication services */
sigmask_t sig_check(sigmask_t sigs);
void sig_send(struct Process *proc, sigmask_t sig);
void sig_post(struct Process *proc, sigmask_t sig);
/*
 * XXX: this is provided for backword compatibility, consider to make this
 * deprecated for the future.
 */
INLINE void sig_signal(struct Process *proc, sigmask_t sig)
{
	sig_post(proc, sig);
}
sigmask_t sig_wait(sigmask_t sigs);
sigmask_t sig_waitTimeout(sigmask_t sigs, ticks_t timeout);

int signal_testRun(void);
int signal_testSetup(void);
int signal_testTearDown(void);

/**
 * \name Signal definitions
 * \{
 */
#define SIG_USER0    BV(0)  /**< Free for user usage */
#define SIG_USER1    BV(1)  /**< Free for user usage */
#define SIG_USER2    BV(2)  /**< Free for user usage */
#define SIG_USER3    BV(3)  /**< Free for user usage */
#define SIG_TIMEOUT  BV(4)  /**< Reserved for timeout use */
#define SIG_SYSTEM5  BV(5)  /**< Reserved for system use */
#define SIG_SYSTEM6  BV(6)  /**< Reserved for system use */
#define SIG_SINGLE   BV(7)  /**< Used to wait for a single event */
/*\}*/

#endif /* KERN_SIGNAL_H */

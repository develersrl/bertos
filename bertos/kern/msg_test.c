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
 * Copyright 2004, 2008 Develer S.r.l. (http://www.develer.com/)
 * Copyright 1999, 2000, 2001 Bernie Innocenti <bernie@codewiz.org>
 * -->
 *
 * \brief Message test.
 *
 * \version $Id$
 *
 * \author Daniele Basile <asterix@develer.com>
 */

#include <cfg/cfg_timer.h>
#include <cfg/debug.h>
#include <cfg/test.h>
#include <cfg/compiler.h>

#include <kern/msg.h>
#include <kern/proc.h>
#include <kern/irq.h>

#include <drv/timer.h>


// Our message port.
static MsgPort test_port;

// A test message with two parameters and a result.
typedef struct
{
	Msg msg;

	int x, y;
	int result;
} TestMsg;


// Receive messages and do something boring with them.
static void receiver_proc(void)
{
	TestMsg *rec_msg;
	for (;;)
	{
	    kprintf("Proc[%d]..\n", 1);
		sig_wait(SIG_SINGLE);
	    kprintf("Proc[%d] get message\n", 1);
        rec_msg = containerof(msg_get(&test_port), TestMsg, msg);
        // Do something with the message
        rec_msg->result = rec_msg->x + rec_msg->y;
	    kprintf("Proc[%d] process message x[%d],y[%d],res[%d]\n", 1, rec_msg->x, rec_msg->y, rec_msg->result);
        msg_reply(&rec_msg->msg);
	    kprintf("Proc[%d] reply\n", 1);
	}
}

static cpu_stack_t receiver_stack[CONFIG_KERN_MINSTACKSIZE / sizeof(cpu_stack_t)];

/**
 * Run signal test
 */
int msg_testRun(void)
{
	kprintf("Run Message test..\n");

	MsgPort test_reply_port;
	TestMsg msg1;
	TestMsg *reply;

    struct Process *recv = proc_new(receiver_proc, NULL, sizeof(receiver_stack), receiver_stack);
	msg_initPort(&test_port, event_createSignal(recv, SIG_SINGLE));
	msg_initPort(&test_reply_port, event_createSignal(proc_current(), SIG_SINGLE));

	// Fill-in first message and send it out.
	msg1.x = 3;
	msg1.y = 2;
	kprintf("invio il msg..\n");
	msg1.msg.replyPort = &test_reply_port;
	msg_put(&test_port, &msg1.msg);

	// Wait for a reply...
	kprintf("prima sig wait..\n");
	sig_wait(SIG_SINGLE);
	kprintf("dopo sig wait..\n");
    reply = containerof(msg_get(&test_reply_port), TestMsg, msg);
	ASSERT(reply != NULL);
	ASSERT(reply->result == 5);

	return 0;
}

int msg_testSetup(void)
{
	kdbg_init();

	#if CONFIG_KERN_PREEMPT
		kprintf("Init Interrupt (preempt mode)..");
		irq_init();
		kprintf("Done.\n");
	#endif

	kprintf("Init Timer..");
	timer_init();
	kprintf("Done.\n");

	kprintf("Init Process..");
	proc_init();
	kprintf("Done.\n");
	return 0;
}

int msg_testTearDown(void)
{
	kputs("TearDown Message test.\n");
	return 0;
}

TEST_MAIN(msg);

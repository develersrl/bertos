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
 * Copyright 2012 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2004 Giovanni Bajo
 * All Rights Reserved.
 * -->
 *
 * \brief Readline module test.
 *
 * \author Giovanni Bajo <rasky@develer.com>
 * \author Daniele Basile <asterix@develer.com>
 */

#include "readline.h"

#include <cfg/compiler.h>
#include <cfg/debug.h>
#include <cfg/test.h>

#include <stdio.h>


char test1_in[] = "a\nb\nc\nd\ne\nf\ng\nh\ni\nj\nk\nl\nm\nn\no\np\nq\nr\ns\nt\nu\nv\nw\nx\ny\nz\n";
char test1_hist[HISTORY_SIZE] = "\0l\0m\0n\0o\0p\0q\0r\0s\0t\0u\0v\0w\0x\0y\0z";

/** Perform the unit test for the readline library */
void rl_test(void);

#if HISTORY_SIZE != 32
	#error This test needs HISTORY_SIZE to be set at 32
#endif

static struct RLContext test_ctx;

static char* test_getc_ptr;
static int test_getc(void* data)
{
	(void)data;
	return *test_getc_ptr++;
}

/**
 * Perform a readline test. The function pipes the characters from \a input_buffer
 * through the I/O to \c rl_readline(). After the whole string is sent, \c do_test()
 * checks if the current history within the context match \a expected_history.
 */
static bool do_test(char* input_buffer, char* expected_history)
{
	rl_init_ctx(&test_ctx);
	rl_sethook_get(&test_ctx, test_getc, NULL);

	test_getc_ptr = input_buffer;
	while (*test_getc_ptr)
		rl_readline(&test_ctx);

	if (memcmp(test_ctx.real_history, expected_history, HISTORY_SIZE) != 0)
	{
		ASSERT2(0, "history compare failed");
		return false;
	}

	return true;
}


int readline_testSetup(void)
{
	kdbg_init();

	return 0;
}


int readline_testRun(void)
{
	if (!do_test(test1_in, test1_hist))
		return -1;

	kprintf("rl_test successful\n");
	return 0;
}

int readline_testTearDown(void)
{
	return 0;
}

TEST_MAIN(readline);



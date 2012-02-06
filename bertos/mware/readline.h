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
 * Copyright (C) 2004 Giovanni Bajo
 * Copyright (C) 2004 Develer S.r.l. (http://www.develer.com/)
 * All Rights Reserved.
 * -->
 * \defgroup readline Line editing support with history
 * \ingroup mware
 * \{

 * \brief Line editing support with history
 *
 * This file implements a kernel for line editing through a terminal, with history of the typed lines.
 * Basic feature of this module:
 *
 * \li Abstracted from I/O. The user must provide hooks for getc and putc functions.
 * \li Basic support for ANSI escape sequences for input of special codes.
 * \li Support for command name completion (through a hook).
 *
 * Rationale for basic implementation choices:
 *
 * \li The history is implemented storing consecutive ASCIIZ strings within an array of memory. When
 * the history is full, the first (oldest) line is cancelled and the whole buffer is memmoved to
 * overwrite it and make room. while this is is obviously not the fastest algorithm (which would
 * require the use of a circular buffer) it is surely good enough for this module, which does not
 * aim at fast performances (line editing does not require to be blazingly fast).
 *
 * \li The first character in the history is always \c \\0, and it is used as a guard. By 'wasting' it
 * in this way, the code actually gets much simpler in that we remove many checks when moving
 * backward (\c i>0 and similar).
 *
 * \li While editing, the current index points to the position of the buffer which contains the
 * last character typed in (exactly like a stack pointer). This also allows to simplify calculations
 * and to make easier using the last byte of history.
 *
 * \li While editing, the current line is always kept null-terminated. This is important because
 * if the user press ENTER, we must have room to add a \c \\0 to terminate the line. If the line
 * is as long as the whole history buffer, there would not be space for it. By always keeping the
 * \c \\0 at the end, we properly ensure this without making index checks harder.
 *
 * \li When removing a line from the history (see \c pop_history()), instead of updating all the
 * indices we have around, we move backward the pointer to the history we use. This way, we don't
 * have to update anything. This means that we keep two pointers to the history: \c real_history
 * always points to the physical start, while \c history is the adjusted pointer (that is
 * dereference to read/write to it).
 *
 * \todo Use up/down to move through history  The history line will be copied to the current line,
 * making sure there is room for it.
 *
 *
 * \author Giovanni Bajo <rasky@develer.com>
 *
 * $WIZ$ module_name = "readline"
 * $WIZ$ module_depends = "sprintf"
 */


#ifndef MWARE_READLINE_H
#define MWARE_READLINE_H

#include <cfg/compiler.h>

#include <string.h>

#define HISTORY_SIZE 32

typedef int (*getc_hook)(void* user_data);
typedef void (*putc_hook)(char ch, void* user_data);
typedef const char* (*match_hook)(void* user_data, const char* word, int word_len);
typedef void (*clear_hook)(void* user_data);

struct RLContext
{
	getc_hook get;
	void* get_param;

	putc_hook put;
	void* put_param;

	match_hook match;
	void* match_param;

	clear_hook clear;
	void* clear_param;

	const char* prompt;

	char real_history[HISTORY_SIZE];
	char* history;
	size_t history_pos;
	size_t line_pos;
};

INLINE void rl_init_ctx(struct RLContext *ctx)
{
	memset(ctx, 0, sizeof(*ctx));
	ctx->history = ctx->real_history;
}

INLINE void rl_clear_history(struct RLContext *ctx)
{
	memset(ctx->real_history, 0, sizeof(ctx->real_history));
	ctx->history_pos = 0;
	ctx->line_pos = ctx->history_pos;
	ctx->history = ctx->real_history;
}

INLINE void rl_sethook_get(struct RLContext* ctx, getc_hook get, void* get_param)
{ ctx->get = get; ctx->get_param = get_param; }

INLINE void rl_sethook_put(struct RLContext* ctx, putc_hook put, void* put_param)
{ ctx->put = put; ctx->put_param = put_param; }

INLINE void rl_sethook_match(struct RLContext* ctx, match_hook match, void* match_param)
{ ctx->match = match; ctx->match_param = match_param; }

INLINE void rl_sethook_clear(struct RLContext* ctx, clear_hook clear, void* clear_param)
{ ctx->clear = clear; ctx->clear_param = clear_param; }

INLINE void rl_setprompt(struct RLContext* ctx, const char* prompt)
{ ctx->prompt = prompt; }

const char* rl_readline(struct RLContext* ctx);

void rl_refresh(struct RLContext* ctx);

/* Test prototype */
int readline_testSetup(void);
int readline_testRun(void);
int readline_testTearDown(void);

/** \} */ //defgroup readline.

#endif /* MWARE_READLINE_H */

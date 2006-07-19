/**
 * \file
 * <!--
 * Copyright (C) 2004 Giovanni Bajo
 * Copyright (C) 2004 Develer S.r.l. (http://www.develer.com/)
 * All Rights Reserved.
 * -->
 *
 * \brief Line editing support with history
 *
 * This file implements a kernel for line editing through a terminal, with history of the typed lines.
 * Basic feature of this module:
 *
 * \li Abstracted from I/O. The user must provide hooks for getc and putc functions.
 * \li Basic support for ANSI escape sequences for input of special codes.
 * \li Support for command name completion (through a hook).
 *
 * \version $Id$
 *
 * \author Giovanni Bajo <rasky@develer.com>
 */

/*#*
 *#* $Log$
 *#* Revision 1.2  2006/07/19 12:56:28  bernie
 *#* Convert to new Doxygen style.
 *#*
 *#* Revision 1.1  2006/06/01 12:27:39  marco
 *#* Added utilities for protocols
 *#*
 *#*/

#ifndef MWARE_READLINE_H
#define MWARE_READLINE_H

#include <cfg/compiler.h>

#include <string.h>

#define HISTORY_SIZE       1024

typedef int (*getc_hook)(void* user_data);
typedef void (*putc_hook)(char ch, void* user_data);
typedef const char* (*match_hook)(void* user_data, const char* word, int word_len);

struct RLContext
{
	getc_hook get;
	void* get_param;

	putc_hook put;
	void* put_param;

	match_hook match;
	void* match_param;

	const char* prompt;

	char real_history[HISTORY_SIZE];
	char* history;
	size_t history_pos;
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
	ctx->history = ctx->real_history;
}

INLINE void rl_sethook_get(struct RLContext* ctx, getc_hook get, void* get_param)
{ ctx->get = get; ctx->get_param = get_param; }

INLINE void rl_sethook_put(struct RLContext* ctx, putc_hook put, void* put_param)
{ ctx->put = put; ctx->put_param = put_param; }

INLINE void rl_sethook_match(struct RLContext* ctx, match_hook match, void* match_param)
{ ctx->match = match; ctx->match_param = match_param; }

INLINE void rl_setprompt(struct RLContext* ctx, const char* prompt)
{ ctx->prompt = prompt; }

const char* rl_readline(struct RLContext* ctx);

void rl_refresh(struct RLContext* ctx);

#endif /* MWARE_READLINE_H */

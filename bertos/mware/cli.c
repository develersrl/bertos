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
 * Copyright 2003, 2004, 2006, 2012 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2000 Bernie Innocenti <bernie@codewiz.org>
 *
 * -->
 *
 * \brief Command-line interface (CLI) module.
 *
 * \author Giovanni Bajo <rasky@develer.com>
 * \author Marco Benelli <marco@develer.com>
 * \author Bernie Innocenti <bernie@codewiz.org>
 * \author Daniele Basile <asterix@develer.com>
 */

#include "cli.h"

#include "cfg/cfg_parser.h"
#include "cfg/cfg_cli.h"

#define LOG_LEVEL   CLI_LOG_LEVEL
#define LOG_FORMAT  CLI_LOG_FORMAT
#include <cfg/log.h>
#include <cfg/compiler.h>
#include <cfg/debug.h>

#include <mware/readline.h>
#include <mware/parser.h>

#include <io/kfile.h>

static CLI *local_cli;

/*
 * Reply macro.
 * Send error message to client.
 *
 * \a fd kfile handler for serial.
 * \a err int error code.
 * \a err human-readable description of the error for debug purposes.
 */
INLINE void REPLY(KFile *fd, int err_code, const char *err)
{
#if CLI_LOG_LEVEL  == LOG_LVL_INFO
	kfile_printf(fd, "%d %s\r\n", err_code, err);
#else
	(void)err;
	kfile_printf(fd, "%d\r\n", err_code);
#endif
}

/*
 * Print args on s, with format specified in t->result_fmt.
 * Return number of valid arguments or -1 in case of error.
 */
static bool cli_reply(KFile *fd, const struct CmdTemplate *t, const parms *args)
{
	unsigned short offset = strlen(t->arg_fmt) + 1;
	unsigned short nres = strlen(t->result_fmt);

	for (unsigned short i = 0; i < nres; ++i)
	{
		if (t->result_fmt[i] == 'd')
		{
			kfile_printf(fd, " %ld", args[offset+i].l);
		}
		else if (t->result_fmt[i] == 's')
		{
			kfile_printf(fd, " %s", args[offset+i].s);
		}
		else
		{
			return false;
		}
	}

	kfile_printf(fd, "\r\n");
	return true;
}

static void cli_parse(CLI *cli, const char *buf)
{
	const struct CmdTemplate *templ;

	/* Command check.  */
	templ = parser_get_cmd_template(buf);
	if (!templ)
	{
		REPLY(cli->fd, CLI_INVALID_CMD, "Invalid command.");
		return;
	}

	parms args[CONFIG_PARSER_MAX_ARGS];

	if (!parser_get_cmd_arguments(buf, templ, args))
	{
		REPLY(cli->fd, CLI_INVALID_ARGS, "Invalid arguments.");
		return;
	}

	/* Execute. */
	if(!parser_execute_cmd(templ, args))
	{
		REPLY(cli->fd, CLI_ERR_EXE_CMD, "Error in executing command.");
	}

	if (!cli_reply(cli->fd, templ, args))
	{
		REPLY(cli->fd, CLI_INVALID_RET_FMT, "Invalid return format.");
	}

	return;
}

/**
 * CLI poll function.
 *
 * Call this function to process all incoming message from
 * fd channel.
 *
 * \param fd kfile channel context.
 */
void cli_poll(KFile *fd)
{
	/* Check with user function if session was end */
	if (!local_cli->is_new_session && local_cli->check_newSession)
	{
		if (local_cli->check_newSession(fd))
			local_cli->is_new_session = true;
	}

	/* Print ready promt at first time that we connect */
	if (local_cli->is_new_session)
	{
		/* If defined call the custom procedure for new session */
		if (local_cli->handshake)
			local_cli->handshake(local_cli->fd);

		rl_refresh(&local_cli->rl_ctx);
		local_cli->is_new_session = false;
	}

	const char *buf = rl_readline(&local_cli->rl_ctx);

	if((buf == NULL))
		return;

	if (buf[0] == '\0')
		return;

	/* If we enter lines beginning with sharp(#)
	they are stripped out from commands */
	if (buf[0] == '#')
	{
		rl_refresh(&local_cli->rl_ctx);
		return;
	}

	/* Close connetion on exit command */
	if (!strcmp(buf, "exit") || !strcmp(buf, "quit"))
	{
		rl_clear_history(&local_cli->rl_ctx);
		kfile_close(local_cli->fd);
		local_cli->is_new_session = true;
	}
	else
	{
		cli_parse(local_cli, buf);
		rl_refresh(&local_cli->rl_ctx);
	}
}


/**
 * Init CLI module.
 *
 * This module use the readline and parser module. The first one
 * manage all command line, while the parser tokenize the given command and
 * call related callback if it exist.
 *
 * \param cli cli context
 * \param ch pointer to kfile channel context
 * \param cmds_register user function to register the defined commands.
 * \param handshake custom fuction for negotiate beetwen the server and client
 * every new session, NULL to skip any init sequence.
 * \param check_newSession custom funtion should return true if we are in new session, false otherwise.
 */
void cli_init(CLI *cli, KFile *ch, cli_t cmds_register, cli_handshake_t handshake, cli_check_t check_newSession)
{
	ASSERT(cmds_register);

	parser_init();
	cmds_register();

	local_cli = cli;

	cli->fd = ch;
	cli->handshake = handshake;
	cli->check_newSession = check_newSession;
	cli->is_new_session = true;

	rl_init_ctx(&cli->rl_ctx);
	rl_setprompt(&cli->rl_ctx, CONFIG_CLI_PROMT_STR);
	rl_sethook_get(&cli->rl_ctx, (getc_hook)kfile_getc, cli->fd);
	rl_sethook_put(&cli->rl_ctx, (putc_hook)kfile_putc, cli->fd);
	rl_sethook_match(&cli->rl_ctx, parser_rl_match, NULL);
}

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
 * \brief Protocol module.
 *
 * This module supply a simple ascii protocol to send commands to
 * the device like pc "terminal". To use it we need to define all command
 * that we want supply, and then we should register they using a user defined
 * function. All commands can take arguments or/and return a value.
 *
 * \code
 * #include "verstag.h"
 * #include <mware/parser.h>
 *
 * //Define a function ver, that return 3 int.
 * //This macro will expand into a fuction named "ver" that not take
 * //an input and return 3 int (ddd).
 * MAKE_CMD(ver, "", "ddd",
 * ({
 *	args[1].l = VERS_MAJOR;
 *	args[2].l = VERS_MINOR;
 *	args[3].l = VERS_REV;
 *	0;
 * }), 0);
 *
 *
 * //Define the function to pass at protocol_init, to register
 * //all defined protocol function.
 * static void protocol_registerCmds(void)
 * {
 * 	  REGISTER_CMD(ver);
 * }
 *
 *
 * //Init the protocol module whit comunication channel and
 * //the callback to register the defined protocol functions.
 * protocol_init(&socket.fd, protocol_registerCmds);
 * \endcode
 *
 * \author Giovanni Bajo <rasky@develer.com>
 * \author Marco Benelli <marco@develer.com>
 * \author Bernie Innocenti <bernie@codewiz.org>
 * \author Daniele Basile <asterix@develer.com>
 */

#include "protocol.h"

#include "cfg/cfg_parser.h"
#include "cfg/cfg_protocol.h"

#define LOG_LEVEL   PROTOCOL_LOG_LEVEL
#define LOG_FORMAT  PROTOCOL_LOG_FORMAT
#include <cfg/log.h>

#include <cfg/compiler.h>
#include <cfg/debug.h>

#include <drv/timer.h>

#include <mware/readline.h>
#include <mware/parser.h>

#include <io/kfile.h>

#include <stdlib.h>
#include <string.h>

//Readline context, used for interactive mode.
static struct RLContext rl_ctx;

/**
 * Send a NAK asking the host to send the current message again.
 *
 * \a fd kfile handler for serial.
 * \a err human-readable description of the error for debug purposes.
 */
INLINE void NAK(KFile *fd, const char *err)
{
#if PROTOCOL_LOG_LEVEL  == LOG_LVL_INFO
	kfile_printf(fd, "NAK \"%s\"\r\n", err);
#else
	kfile_printf(fd, "NAK\r\n");
#endif
}

/*
 * Print args on s, with format specified in t->result_fmt.
 * Return number of valid arguments or -1 in case of error.
 */
static bool protocol_reply(KFile *fd, const struct CmdTemplate *t, const parms *args)
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

static void protocol_parse(KFile *fd, const char *buf)
{
	const struct CmdTemplate *templ;

	/* Command check.  */
	templ = parser_get_cmd_template(buf);
	if (!templ)
	{
		kfile_print(fd, "-1 Invalid command.\r\n");
		return;
	}

	parms args[CONFIG_PARSER_MAX_ARGS];

	/* Args Check.  TODO: Handle different case. see doc/PROTOCOL .  */
	if (!parser_get_cmd_arguments(buf, templ, args))
	{
		kfile_print(fd, "-2 Invalid arguments.\r\n");
		return;
	}

	/* Execute. */
	if(!parser_execute_cmd(templ, args))
	{
		NAK(fd, "Error in executing command.");
	}
	if (!protocol_reply(fd, templ, args))
	{
		NAK(fd, "Invalid return format.");
	}

	return;
}

void protocol_poll(KFile *fd)
{
	const char *buf = rl_readline(&rl_ctx);

	if((buf == NULL))
		return;

	if (buf[0] == '\0')
		return;

	/* If we enter lines beginning with sharp(#)
	they are stripped out from commands */
	if (buf[0] == '#')
	{
		rl_refresh(&rl_ctx);
		return;
	}

	/* Close connetion on exit command */
	if (!strcmp(buf, "exit") || !strcmp(buf, "quit"))
	{
		rl_clear_history(&rl_ctx);
		kfile_close(fd);
	}
	else
	{
		protocol_parse(fd, buf);
	}

	rl_refresh(&rl_ctx);
}


/*
 * Initialization: readline context, parser and register commands.
 */
void protocol_init(KFile *fd, protocol_t cmds_register)
{
	ASSERT(cmds_register);

	parser_init();
	cmds_register();

	rl_init_ctx(&rl_ctx);
	rl_setprompt(&rl_ctx, CONFIG_PROMT_STR);
	rl_sethook_get(&rl_ctx, (getc_hook)kfile_getc, fd);
	rl_sethook_put(&rl_ctx, (putc_hook)kfile_putc, fd);
	rl_sethook_match(&rl_ctx, parser_rl_match, NULL);
	rl_sethook_clear(&rl_ctx, (clear_hook)kfile_clearerr,fd);
	rl_refresh(&rl_ctx);
}

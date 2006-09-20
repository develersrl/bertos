/**
 * \file
 * <!--
 * Copyright 2006 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 * \brief Implementation of the command protocol between the board and the host
 *
 *
 * \version $Id$
 *
 * \author Giovanni Bajo <rasky@develer.com>
 * \author Marco Benelli <marco@develer.com>
 * \author Bernardo Innocenti <bernie@develer.com>
 */

/*#*
 *#* $Log$
 *#* Revision 1.6  2006/09/20 13:54:04  marco
 *#* Usage examples of MAKE_CMD.
 *#*
 *#* Revision 1.5  2006/06/14 01:03:01  marco
 *#* Add response code. Add command ping.
 *#*
 *#* Revision 1.4  2006/06/14 00:26:48  marco
 *#* Use new macros for defining commands.
 *#*
 *#* Revision 1.3  2006/06/13 19:07:31  marco
 *#* Fixed a bug in protocol_reply. Simplified rpc.
 *#*
 *#* Revision 1.2  2006/06/12 21:37:02  marco
 *#* implemented some commands (ver and sleep)
 *#*
 *#* Revision 1.1  2006/06/01 12:29:21  marco
 *#* Add first simple protocol command (version request).
 *#*
 *#*/

#include "protocol.h"

#include <drv/ser.h>
#include <drv/timer.h>
#include <mware/readline.h>
#include <mware/parser.h>
#include <cfg/compiler.h>
#include <cfg/debug.h>
#include <verstag.h>

#include <stdlib.h>
#include <string.h>

//#include <cmd_hunk.h>

#include "cmd_ctor.h"  // MAKE_CMD, REGISTER_CMD

// DEBUG: set to 1 to force interactive mode
#define FORCE_INTERACTIVE         1

/**
 * True if we are in interactive mode, false if we are in protocol mode.
 * In interactive mode, commands are read through readline() (prompt,
 * completion, history) without IDs, and replies/errors are sent to the serial
 * output.
 * In protocol mode, we implement the default protocol
 */
static bool interactive;

/// Readline context, used for interactive mode.
static struct RLContext rl_ctx;


/**
 * Send a NAK asking the host to send the current message again.
 *
 * \param err  human-readable description of the error for debug purposes.
 */
INLINE void NAK(Serial *ser, const char *err)
{
#ifdef _DEBUG
	ser_printf(ser, "NAK \"%s\"\r\n", err);
#else
	ser_printf(ser, "NAK\r\n");
#endif
}

/*
 * Print args on s, with format specified in t->result_fmt.
 * Return number of valid arguments or -1 in case of error.
 */
static bool protocol_reply(Serial *s, const struct CmdTemplate *t,
			  const parms *args)
{
	unsigned short offset = strlen(t->arg_fmt) + 1;
	unsigned short nres = strlen(t->result_fmt);

	ser_printf(s, "0");
	for (unsigned short i = 0; i < nres; ++i)
	{
		if (t->result_fmt[i] == 'd')
		{
			ser_printf(s, " %ld", args[offset+i].l);
		}
		else if (t->result_fmt[i] == 's')
		{
			ser_printf(s, " %s", args[offset+i].s);
		}
		else
		{
			abort();
		}
	}
	ser_printf(s, "\r\n");
	return true;
}

static void protocol_parse(Serial *ser, const char *buf)
{
	const struct CmdTemplate *templ;

	/* Command check.  */
	templ = parser_get_cmd_template(buf);
	if (!templ)
	{
		ser_print(ser, "-1 Invalid command.");
		return;
	}

	parms args[PARSER_MAX_ARGS];

	/* Args Check.  TODO: Handle different case. see doc/PROTOCOL .  */
	if (!parser_get_cmd_arguments(buf, templ, args))
	{
		ser_print(ser, "-2 Invalid arguments.");
		return;
	}

	/* Execute. */
	if(!parser_execute_cmd(templ, args))
	{
		NAK(ser, "Error in executing command.");
	}
	if (!protocol_reply(ser, templ, args))
	{
		NAK(ser, "Invalid return format.");
	}
	return;
}

void protocol_run(Serial *ser)
{
	/**
	 * \todo to be removed, we could probably access the serial FIFO
	 * directly
	 */
	static char linebuf[80];

	if (!interactive)
	{
		ser_gets(ser, linebuf, sizeof(linebuf));

		// reset serial port error anyway
		ser_setstatus(ser, 0);

		// check message minimum length
		if (linebuf[0])
		{
			if (linebuf[0] == 0x1B && linebuf[1] == 0x1B)  // ESC
			{
				interactive = true;
				ser_printf(ser,
					   "Entering interactive mode\r\n");
			}
			else
			{
				protocol_parse(ser, linebuf);
			}
		}
	}
	else
	{
		const char *buf;

		/*
		 * Read a line from serial. We use a temporary buffer
		 * because otherwise we would have to extract a message
		 * from the port immediately: there might not be any
		 * available, and one might get free while we read
		 * the line. We also add a fake ID at the start to
		 * fool the parser.
		 */
		buf = rl_readline(&rl_ctx);

		if (buf && buf[0] != '\0')
		{
			// exit special case to immediately change serial input
			if (!strcmp(buf, "exit") || !strcmp(buf, "quit"))
			{
				rl_clear_history(&rl_ctx);
				ser_printf(ser,
					   "Leaving interactive mode...\r\n");
				interactive = FORCE_INTERACTIVE;
			}
			else
			{
				//TODO: remove sequence numbers
				linebuf[0] = '0';
				linebuf[1] = ' ';

				strncpy(linebuf + 2, buf, sizeof(linebuf) - 3);
				linebuf[sizeof(linebuf) - 1] = '\0';
				protocol_parse(ser, linebuf);
			}
		}
	}
}

/*
 * Commands.
 * TODO: Command declarations and definitions should be in another file(s).
 * Maybe we should use CMD_HUNK_TEMPLATE.
 *
 */

/* Version. Example of declaring function and passing it to MAKE_CMD.  */
static int ver_fn(const char **str)
{
	*str = VERS_TAG;
	return 0;
}
MAKE_CMD(ver, "", "s", ver_fn(&args[1].s))

/* Sleep. Example of declaring function body directly in macro call.  */
MAKE_CMD(sleep, "d", "",
({
	timer_delay((mtime_t)args[1].l);
	0;
}))

/* Ping.  */
MAKE_CMD(ping, "", "",
({
	0;
}))

/* Register commands.  */
static void protocol_registerCmds(void)
{
	REGISTER_CMD(ver);
	REGISTER_CMD(sleep);
	REGISTER_CMD(ping);
}

/* Initialization: readline context, parser and register commands.  */
void protocol_init(Serial *ser)
{
	interactive = FORCE_INTERACTIVE;

	rl_init_ctx(&rl_ctx);
	rl_setprompt(&rl_ctx, ">> ");
	rl_sethook_get(&rl_ctx, (getc_hook)ser_getchar, ser);
	rl_sethook_put(&rl_ctx, (putc_hook)ser_putchar, ser);
	rl_sethook_match(&rl_ctx, parser_rl_match, NULL);

	parser_init();

	protocol_registerCmds();
}

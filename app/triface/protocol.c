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
 * Copyright 2003, 2004, 2006 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2000 Bernardo Innocenti <bernie@codewiz.org>
 *
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

#include "protocol.h"


#include <drv/timer.h>
#include <drv/ser.h>
#include <mware/readline.h>
#include <mware/parser.h>
#include <cfg/compiler.h>
#include <cfg/debug.h>
#include <drv/sipo.h>
#include <drv/wdt.h>
#include "hw_adc.h"

#include <stdlib.h>
#include <string.h>

//#include <cmd_hunk.h>

#include "cmd_ctor.h"  // MAKE_CMD, REGISTER_CMD
#include "hw_input.h"
#include "verstag.h"
#include <drv/buzzer.h>

// Define the format string for ADC
#define ADC_FORMAT_STR "dddd"

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

uint8_t reg_status_dout;
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

static void protocol_prompt(Serial *ser)
{
	ser_print(ser, ">> ");
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
		ser_print(ser, "-1 Invalid command.\r\n");
		protocol_prompt(ser);
		return;
	}

	parms args[PARSER_MAX_ARGS];

	/* Args Check.  TODO: Handle different case. see doc/PROTOCOL .  */
	if (!parser_get_cmd_arguments(buf, templ, args))
	{
		ser_print(ser, "-2 Invalid arguments.\r\n");
		protocol_prompt(ser);
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

	protocol_prompt(ser);
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
			/* If we enter lines beginning with sharp(#)
			they are stripped out from commands */
			if(linebuf[0] != '#')
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

		/* If we enter lines beginning with sharp(#)
		they are stripped out from commands */
		if(buf && buf[0] != '#')
		{
			if (buf[0] != '\0')
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
}

/*
 * Commands.
 * TODO: Command declarations and definitions should be in another file(s).
 * Maybe we should use CMD_HUNK_TEMPLATE.
 *
 */

MAKE_CMD(ver, "", "ddd",
({
	args[1].l = VERS_MAJOR;
	args[2].l = VERS_MINOR;
	args[3].l = VERS_REV;
	0;
}), 0);

/* Sleep. Example of declaring function body directly in macro call.  */
MAKE_CMD(sleep, "d", "",
({
	timer_delay((mtime_t)args[1].l);
	0;
}), 0)

/* Ping.  */
MAKE_CMD(ping, "", "",
({
	//Silence "args not used" warning.
	(void)args;
	0;
}), 0)

/* Dout  */
MAKE_CMD(dout, "d", "",
({
	sipo_putchar((uint8_t)args[1].l);

	//Store status of dout ports.
	reg_status_dout = (uint8_t)args[1].l;
	0;
}), 0)

/* rdout  read the status of out ports.*/
MAKE_CMD(rdout, "", "d",
({
	args[1].l = reg_status_dout;
	0;
}), 0)

/* Doutx sperimentale.......  */
MAKE_CMD(doutx, "d", "",
 ({
	 sipo_putchar((uint8_t)args[1].l);
	 
	 //Store status of dout ports.
	 reg_status_dout = (uint8_t)args[1].l;
	 0;
 }), 0)

/* Reset */
MAKE_CMD(reset, "", "",
({
	//Silence "args not used" warning.
	(void)args;
	wdt_init(7);
	wdt_start();
	0;
}), 0)

/* Din */
MAKE_CMD(din, "", "d",
({
	args[1].l = INPUT_GET();
	0;
}), 0)



/* Ain */
MAKE_CMD(ain, "", ADC_FORMAT_STR,
({
	STATIC_ASSERT((sizeof(ADC_FORMAT_STR) - 1) == ADC_CHANNEL_NUM);
	for(int i = 0; i < ADC_CHANNEL_NUM; i++)
		args[i+1].l = adc_read_ai_channel(i);

	0;
}), 0)

/* Beep  */
MAKE_CMD(beep, "d", "",
({
	buz_beep(args[1].l);
	0;
}), 0)

/* Register commands.  */
static void protocol_registerCmds(void)
{
	REGISTER_CMD(ver);
	REGISTER_CMD(sleep);
	REGISTER_CMD(ping);
	REGISTER_CMD(dout);
	//Set off all dout ports.
	reg_status_dout = 0;
	REGISTER_CMD(rdout);
	REGISTER_CMD(doutx);
	REGISTER_CMD(reset);
	REGISTER_CMD(din);
	REGISTER_CMD(ain);
	REGISTER_CMD(beep);
}

/* Initialization: readline context, parser and register commands.  */
void protocol_init(Serial *ser)
{
	interactive = FORCE_INTERACTIVE;

	rl_init_ctx(&rl_ctx);
	//rl_setprompt(&rl_ctx, ">> ");
	rl_sethook_get(&rl_ctx, (getc_hook)ser_getchar, ser);
	rl_sethook_put(&rl_ctx, (putc_hook)ser_putchar, ser);
	rl_sethook_match(&rl_ctx, parser_rl_match, NULL);
	rl_sethook_clear(&rl_ctx, (clear_hook)ser_clearstatus,ser);

	parser_init();

	protocol_registerCmds();

	protocol_prompt(ser);
}

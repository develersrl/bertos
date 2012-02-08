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
 * \defgroup cli_module Command-line interface (CLI) module.
 * \ingroup mware
 * \{
 *
 * \brief Command-line interface (CLI) parser interface.
 *
 * This module supply a simple ascii CLI to send commands to
 * the device like pc "terminal". To use it we need to define all command
 * that we want supply, and then we should register they using a user defined
 * function. All commands can take arguments or/and return a value.
 *
 * \code
 * #include "verstag.h"
 * #include <mware/parser.h>
 * #include <mware/cli.h>
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
 * //Define the function to pass at cli_init, to register
 * //all defined cli function.
 * static void cli_registerCmds(void)
 * {
 * 	  REGISTER_CMD(ver);
 * }
 *
 *
 * //Init the cli module whit comunication channel context and
 * //the register command function.
 * cli_init(&fd, cli_registerCmds);
 *
 * //To process all message that come from channel call the poll function
 * while (1)
 * {
 *    cli_poll(&fd);
 * }
 *
 * \endcode
 *
 * \author Marco Benelli <marco@develer.com>
 * \author Daniele Basile <asterix@develer.com>
 *
 * $WIZ$ module_name = "cli"
 * $WIZ$ module_configuration = "bertos/cfg/cfg_cli.h"
 * $WIZ$ module_depends = "kfile", "parser", "readline"
 */


#ifndef MWARE_CLI_H
#define MWARE_CLI_H

#include <mware/readline.h>

#include <io/kfile.h>

#define CLI_OK_CMD           0 ///< No error.
#define CLI_INVALID_CMD     -1 ///< Invalid command.
#define CLI_INVALID_ARGS    -2 ///< Invalid number of arguments for current command.
#define CLI_ERR_EXE_CMD     -3 ///< Command execution error.
#define CLI_INVALID_RET_FMT -4 ///< Invalid command return formart.

typedef void (*cli_t)(void);                ///< Register command function prototype.
typedef bool (*cli_check_t)(KFile *fd);          ///<
typedef void (*cli_handshake_t)(KFile *fd); ///< Handshake fuction prototype.

typedef struct CLI
{
	KFile *fd;                 ///< Kfile communication channel context
	struct RLContext rl_ctx;   ///< Readline context.
	cli_handshake_t handshake; ///< Custom function to be call every new session.
	cli_check_t check_newSession; ///<
	bool is_new_session;       ///< Flag to trac new session.
} CLI;

void cli_poll(KFile *fd);
void cli_init(CLI *cli, KFile *ch, cli_t cmds_register, cli_handshake_t handshake, cli_check_t check_newSession);

/** \} */ //defgroup cli_module.

#endif /* MWARE_CLI_H */

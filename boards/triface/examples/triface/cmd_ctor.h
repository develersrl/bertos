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
 * Copyright 2006 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 * \brief Macro for defining command for protocol.
 *
 * \author Marco Benelli <marco@develer.com>
 */

#ifndef CMD_CTOR_H
#define CMD_CTOR_H

#include <mware/parser.h>

#define REGISTER_FUNCTION parser_register_cmd

#define REGISTER_CMD(NAME) REGISTER_FUNCTION(&cmd_ ## NAME ## _template)

#define MAKE_TEMPLATE(NAME, ARGS, RES, FLAGS)				\
const struct CmdTemplate cmd_ ## NAME ## _template =			\
{									\
	#NAME, ARGS, RES, cmd_ ## NAME, FLAGS				\
};

#define MAKE_CMD(NAME, ARGS, RES, BODY, FLAGS)				\
static ResultCode cmd_ ## NAME (parms *args)				\
{									\
	return (ResultCode)BODY;			      		\
}									\
MAKE_TEMPLATE(NAME, ARGS, RES, FLAGS)

#endif // CMD_CTOR_H

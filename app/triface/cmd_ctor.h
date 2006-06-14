/**
 * \file
 * <!--
 * Copyright 2006 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 * \brief Macro for defining command for protocol.
 *
 *
 * \version $Id$
 *
 * \author Marco Benelli <marco@develer.com>
 */
#ifndef CMD_CTOR_H
#define CMD_CTOR_H

#include <mware/parser.h>

#define REGISTER_FUNCTION parser_register_cmd

#define REGISTER_CMD(NAME) REGISTER_FUNCTION(&cmd_ ## NAME ## _template)

#define MAKE_TEMPLATE(NAME, ARGS, RES)					\
const struct CmdTemplate cmd_ ## NAME ## _template =			\
{									\
	#NAME, ARGS, RES, cmd_ ## NAME, 0				\
};

#define MAKE_CMD(NAME, ARGS, RES, BODY)					\
static ResultCode cmd_ ## NAME (parms *args)				\
{									\
	return BODY;							\
}									\
MAKE_TEMPLATE(NAME, ARGS, RES)

#endif // CMD_CTOR_H

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
 * Copyright 2004 Giovanni Bajo
 * Copyright 2004 Develer S.r.l. (http://www.develer.com/)
 * All Rights Reserved.
 * -->
 * This module permits to create hunks for the functions that must be executed through
 * RPC commands. For instance, given this code:
 *
 * \code
 * ResultCode cmd_add(long a, long b, long* result);
 * DECLARE_COMMAND_HUNK(add, (long)(long)(NIL), (long)(NIL));
 * //                        ^ parameters       ^ return values
 * \endcode
 *
 * The macro is expanded to:
 *
 * \code
 * ResultCode cmd_add_hunk(params argv[], params results[])
 * {
 *    return cmd_add(argv[0].l, argv[1].l, &results[0].l);
 * }
 *
 * const struct CmdTemplate cmd_add_template = 
 * {
 *    "add", "dd", "d", cmd_add_hunk
 * };
 * \endcode
 *
 * which is all the boilerplate needed to make the function ready for the RPC.
 * The implementation uses the Boost Preprocessor Library (part of the Boost
 * library, available at http://www.boost.org). The version we developed the
 * code with is 1.31.
 *
 * \version $Id$
 *
 * \author Giovanni Bajo <rasky@develer.com>
 *
 * \brief Preprocessor magic to create hunks for the commands executed from the parser
 *
 */

/*#*
 *#* $Log$
 *#* Revision 1.2  2006/07/19 12:56:27  bernie
 *#* Convert to new Doxygen style.
 *#*
 *#* Revision 1.1  2006/06/01 12:27:39  marco
 *#* Added utilities for protocols
 *#*
 *#* Revision 1.8  2005/01/12 20:06:31  rasky
 *#* enum ResultCode è ora un typedef
 *#*
 *#* Revision 1.7  2004/12/14 15:10:53  user1
 *#* Bernardo e Francesco: Assegnazione di un codice di ritorno ai processi
 *#*
 *#* Revision 1.6  2004/12/02 17:43:15  rasky
 *#* Aggiunta la possibilità di settare le flag dei comandi con la nuova macro DECLARE_CMD_HUNK_FLAGS (altrimenti, il default è zero)
 *#*
 *#* Revision 1.5  2004/10/16 13:20:15  rasky
 *#* Aggiunto supporto per alias di uno stesso comando
 *#*
 *#* Revision 1.4  2004/08/25 14:17:10  rasky
 *#* Aggiornato il comment block dei log RCS
 *#*
 *#* Revision 1.3  2004/07/31 22:00:07  rasky
 *#* Fixata una dipendenza
 *#*
 *#* Revision 1.2  2004/07/31 14:32:58  rasky
 *#* args e results possono overlapparsi sempre grazie ai thunk generati: eliminati i due array separati per risparmiare memoria
 *#*
 *#* Revision 1.1  2004/05/22 16:29:16  rasky
 *#* Generazione degli hunk dei comandi RPC tramite preprocessore
 *#*
 *#*/

#ifndef CMD_HUNK_H
#define CMD_HUNK_H

#include "parser.h"

// Bring in the Boost Preprocess Library
#include <boost/preprocessor/library.hpp>

#define HUNK_INDEX_FOR_NIL      0
#define HUNK_INDEX_FOR_string   1
#define HUNK_INDEX_FOR_long     2
#define HUNK_ARRAY_LETTERS      (3, (NIL, s, l))
#define HUNK_ARRAY_STRINGS      (3, ("", "s", "d"))

// Transform int->l, float->f, etc.
#define HUNK_TYPE_LETTER(s, _, type) \
	BOOST_PP_CAT(HUNK_INDEX_FOR_, type) \
	/**/

#define HUNK_TRANSFORMER(_, array, elem) \
	BOOST_PP_ARRAY_ELEM(elem, array) \
	/**/

#define HUNK_SEQ_TRANS_ARRAY(seq, array) \
	BOOST_PP_SEQ_TRANSFORM(HUNK_TRANSFORMER, array, seq) \
	/**/

#define HUNK_PARAM(_, n, seq)    \
	args_results[n+1]. BOOST_PP_SEQ_ELEM(n, seq) \
	/**/

#define HUNK_RESULT(_, n, seq)    \
	&args_results[n]. BOOST_PP_SEQ_ELEM(n, seq) \
	/**/

#define HUNK_IDENTITY(_, dummy, x)  x
#define CMD_HUNK_TEMPLATE(func)         cmd_##func###_template

#define DECLARE_CMD_HUNK_2(func, name, param_types, result_types, flags)    \
	static ResultCode cmd_##name##_hunk(parms args_results[]) \
	{ \
		return cmd_##func( \
		       BOOST_PP_ENUM(BOOST_PP_DEC(BOOST_PP_SEQ_SIZE(param_types)),  HUNK_PARAM,  HUNK_SEQ_TRANS_ARRAY(param_types, HUNK_ARRAY_LETTERS)) \
		       BOOST_PP_COMMA_IF(BOOST_PP_AND(BOOST_PP_DEC(BOOST_PP_SEQ_SIZE(param_types)), BOOST_PP_DEC(BOOST_PP_SEQ_SIZE(result_types)))) \
		       BOOST_PP_ENUM(BOOST_PP_DEC(BOOST_PP_SEQ_SIZE(result_types)), HUNK_RESULT, HUNK_SEQ_TRANS_ARRAY(result_types, HUNK_ARRAY_LETTERS)) \
		); \
	} \
	const struct CmdTemplate CMD_HUNK_TEMPLATE(name) = { \
		#name, \
		BOOST_PP_SEQ_FOR_EACH(HUNK_IDENTITY, _, HUNK_SEQ_TRANS_ARRAY(param_types, HUNK_ARRAY_STRINGS)),  \
		BOOST_PP_SEQ_FOR_EACH(HUNK_IDENTITY, _, HUNK_SEQ_TRANS_ARRAY(result_types, HUNK_ARRAY_STRINGS)), \
		cmd_##name##_hunk, \
		flags \
	} \
	/**/

#define DECLARE_CMD_HUNK(func, param_types, result_types)    \
	DECLARE_CMD_HUNK_2(func, func, \
	                   BOOST_PP_SEQ_TRANSFORM(HUNK_TYPE_LETTER, _, param_types), \
	                   BOOST_PP_SEQ_TRANSFORM(HUNK_TYPE_LETTER, _, result_types), \
	                   0) \
	/**/

#define DECLARE_CMD_HUNK_NAME(func, name, param_types, result_types)    \
	DECLARE_CMD_HUNK_2(func, name, \
	                   BOOST_PP_SEQ_TRANSFORM(HUNK_TYPE_LETTER, _, param_types), \
	                   BOOST_PP_SEQ_TRANSFORM(HUNK_TYPE_LETTER, _, result_types), \
	                   0) \
	/**/

#define DECLARE_CMD_HUNK_FLAGS(func, param_types, result_types, flags)    \
	DECLARE_CMD_HUNK_2(func, func, \
	                   BOOST_PP_SEQ_TRANSFORM(HUNK_TYPE_LETTER, _, param_types), \
	                   BOOST_PP_SEQ_TRANSFORM(HUNK_TYPE_LETTER, _, result_types), \
	                   flags) \
	/**/

#endif

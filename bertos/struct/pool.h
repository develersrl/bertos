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
 * Copyright 2004, 2008 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 * \brief Pool macros.
 *
 * \author Giovanni Bajo <rasky@develer.com>
 */

#ifndef STRUCT_POOL_H
#define STRUCT_POOL_H

#include <cfg/macros.h>
#include <struct/list.h>

#define EXTERN_POOL(name) \
	extern List name

#define DECLARE_POOL_WITH_STORAGE(name, type, num, storage) \
	static type name##_items[num]; \
	storage name; \
	INLINE void name##_init(void (*init_func)(type*)) \
	{ \
		int i; \
		LIST_INIT(&name); \
		for (i=0;i<countof(name##_items);++i) \
		{ \
			if (init_func) init_func(&name##_items[i]); \
			ADDTAIL(&name, (Node*)&name##_items[i]); \
		} \
	} \
	INLINE void name##_init(void (*init_func)(type*)) \
	/**/

#define DECLARE_POOL(name, type, num) \
	DECLARE_POOL_WITH_STORAGE(name, type, num, List)

#define DECLARE_POOL_STATIC(name, type, num) \
	DECLARE_POOL_WITH_STORAGE(name, type, num, static List)

#define pool_init(name, init_func)     (*(name##_init))(init_func)
#define pool_alloc(name)               REMHEAD(name)
#define pool_free(name, elem)          ADDHEAD(name, (Node*)elem)
#define pool_empty(name)               ISLISTEMPTY(name)

#endif /* STRUCT_POOL_H */

/**
 * \file
 * <!--
 * Copyright 2004 Develer S.r.l. (http://www.develer.com/)
 * This file is part of DevLib - See README.devlib for information.
 * -->
 *
 * \brief Pool macros.
 *
 * \version $Id$
 * \author Giovanni Bajo <rasky@develer.com>
 */

/*#*
 *#* $Log$
 *#* Revision 1.6  2006/07/19 12:56:28  bernie
 *#* Convert to new Doxygen style.
 *#*
 *#* Revision 1.5  2005/11/04 16:20:02  bernie
 *#* Fix reference to README.devlib in header.
 *#*
 *#* Revision 1.4  2005/04/11 19:10:28  bernie
 *#* Include top-level headers from cfg/ subdir.
 *#*
 *#* Revision 1.3  2004/12/08 08:09:01  bernie
 *#* Add missing header.
 *#*
 *#*/
#ifndef MWARE_POOL_H
#define MWARE_POOL_H

#include <cfg/macros.h>
#include <mware/list.h>

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

#endif /* MWARE_POOL_H */

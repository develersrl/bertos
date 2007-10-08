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
 * Copyright 2001,2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 1999,2000,2001 Bernardo Innocenti <bernie@develer.com>
 * This file is part of DevLib - See README.devlib for information.
 * -->
 *
 * \brief Kernel configuration parameters
 *
 * \version $Id$
 *
 * \author Bernardo Innocenti <bernie@develer.com>
 */

/*#*
 *#* $Log$
 *#* Revision 1.8  2006/07/19 12:56:24  bernie
 *#* Convert to new Doxygen style.
 *#*
 *#* Revision 1.7  2006/02/21 16:05:53  bernie
 *#* Move from cfg/ to top-level.
 *#*
 *#* Revision 1.3  2005/11/04 16:20:01  bernie
 *#* Fix reference to README.devlib in header.
 *#*
 *#* Revision 1.2  2005/04/11 19:10:27  bernie
 *#* Include top-level headers from cfg/ subdir.
 *#*
 *#* Revision 1.1  2005/04/11 19:04:13  bernie
 *#* Move top-level headers to cfg/ subdir.
 *#*
 *#* Revision 1.5  2004/08/25 14:12:08  rasky
 *#* Aggiornato il comment block dei log RCS
 *#*
 *#* Revision 1.4  2004/08/24 16:19:38  bernie
 *#* Add missing header.
 *#*
 *#* Revision 1.3  2004/07/30 14:24:16  rasky
 *#* Task switching con salvataggio perfetto stato di interrupt (SR)
 *#* Kernel monitor per dump informazioni su stack dei processi
 *#*
 *#* Revision 1.2  2004/06/03 11:27:09  bernie
 *#* Add dual-license information.
 *#*
 *#* Revision 1.1  2004/05/23 17:48:35  bernie
 *#* Add top-level files.
 *#*
 *#*/
#ifndef CONFIG_KERN_H
#define CONFIG_KERN_H

#include <cfg/arch_config.h>  /* ARCH_EMUL */

/**
 * \name Modules activation
 *
 * \{
 */
/*      Module/option          Active    Dependencies */
#define CONFIG_KERN_SCHED       (1)
#define CONFIG_KERN_SIGNALS     (1    && CONFIG_KERN_SCHED)
#define CONFIG_KERN_TIMER       (1)
#define CONFIG_KERN_HEAP        (0)
#define CONFIG_KERN_SEMAPHORES  (0    && CONFIG_KERN_SIGNALS)
#define CONFIG_KERN_MONITOR     (1    && CONFIG_KERN_SCHED)
/*\}*/

/* EXPERIMENTAL */
#define CONFIG_KERN_PREEMPTIVE  (0    && CONFIG_KERN_SCHED && CONFIG_KERN_TIMER)

#define CONFIG_KERN_QUANTUM     50    /**< Time sharing quantum in timer ticks. */

#if (ARCH & ARCH_EMUL)
	#define CONFIG_KERN_DEFSTACKSIZE  65536
#else
	#define CONFIG_KERN_DEFSTACKSIZE  128  /**< Default stack size for each thread. */
#endif

/* Memory fill codes to help debugging */
#if CONFIG_KERN_MONITOR
	#define CONFIG_KERN_STACKFILLCODE  0xA5A5
	#define CONFIG_KERN_MEMFILLCODE    0xDBDB
#endif


#endif /*  CONFIG_KERN_H */

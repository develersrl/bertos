/*!
 * \file
 * <!--
 * Copyright 2001,2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 1999,2000,2001 Bernardo Innocenti <bernie@develer.com>
 * This file is part of DevLib - See devlib/README for information.
 * -->
 *
 * \brief Kernel configuration parameters
 *
 * \version $Id$
 *
 * \author Bernardo Innocenti <bernie@develer.com>
 */

/*
 * $Log$
 * Revision 1.3  2004/07/30 14:24:16  rasky
 * Task switching con salvataggio perfetto stato di interrupt (SR)
 * Kernel monitor per dump informazioni su stack dei processi
 *
 * Revision 1.2  2004/06/03 11:27:09  bernie
 * Add dual-license information.
 *
 * Revision 1.1  2004/05/23 17:48:35  bernie
 * Add top-level files.
 *
 */
#ifndef CONFIG_KERN_H
#define CONFIG_KERN_H

/*!
 * \name Modules activation
 *
 * \{
 */
/*      Module/option          Active    Dependencies */
#define CONFIG_KERN_SCHED       (1)
#define CONFIG_KERN_SIGNALS     (1    && CONFIG_KERN_SCHED)
#define CONFIG_KERN_TIMER       (1    && CONFIG_KERN_SIGNALS)
#define CONFIG_KERN_HEAP        (0)
#define CONFIG_KERN_SEMAPHORES  (0    && CONFIG_KERN_SIGNALS)
#define CONFIG_KERN_MONITOR     (1    && CONFIG_KERN_SCHED)
/*\}*/

/* EXPERIMENTAL */
#define CONFIG_KERN_PREEMPTIVE  (0    && CONFIG_KERN_SCHED && CONFIG_KERN_TIMER)


#define CONFIG_KERN_HEAPSIZE    0x4000  /*!< Heap size (16KB) */
#define CONFIG_KERN_QUANTUM     50      /*!< Scheduler time sharing quantum in ticks */

#if (ARCH & ARCH_EMUL)
	#define CONFIG_KERN_DEFSTACKSIZE  65536
#else
	#define CONFIG_KERN_DEFSTACKSIZE  128  /*!< Default stack size for each thread */
#endif

/* Memory fill codes to help debugging */
#if CONFIG_KERN_MONITOR
	#define CONFIG_KERN_STACKFILLCODE  0xA5A5
	#define CONFIG_KERN_MEMFILLCODE    0xDBDB
#endif



#endif /*  CONFIG_KERN_H */

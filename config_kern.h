/*!
 * \file
 * <!--
 * Copyright 2001,2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 1999,2000,2001 Bernardo Innocenti <bernie@develer.com>
 * All Rights Reserved.
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
#ifdef _DEBUG
	#define CONFIG_KERN_STACKFILLCODE  0xE1
	#define CONFIG_KERN_MEMFILLCODE    0xDB
#endif

#endif /*  CONFIG_KERN_H */

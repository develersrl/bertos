/*!
 * \file
 * <!--
 * Copyright 2004 Develer S.r.l. (http://www.develer.com/)
 * This file is part of DevLib - See devlib/README for information.
 * -->
 *
 * \brief Monitor to check for stack overflows
 *
 * \version $Id$
 *
 * \author Giovanni Bajo <rasky@develer.com>
 */

/*#*
 *#* $Log$
 *#* Revision 1.1  2004/10/03 20:39:03  bernie
 *#* Import in DevLib.
 *#*
 *#* Revision 1.1  2004/09/30 23:19:30  rasky
 *#* Estratto il monitor degli stack da proc.c in due file a parte: monitor.c/h
 *#* Rinominata monitor_debug_stacks in monitor_report
 *#*
 *#*/

#ifndef KERN_MONITOR_H
#define KERN_MONITOR_H

#include <cpu.h>
#include <config_kern.h>

#if CONFIG_KERN_MONITOR

/*!
 * Start the kernel monitor. It is a special process which checks every second the stacks of the
 * running processes trying to detect stack overflows.
 *
 * \param stacksize Size of stack in chars
 * \param stack Pointer to the stack that will be used by the monitor
 *
 * \note The stack is provided by the caller so that there is no wasted space if the monitor
 * is not used.
 */
void monitor_start(size_t stacksize, cpustack_t *stack);


/*!
 * Manually check if a given stack has overflown. This is used to check for stacks
 * of processes handled externally form the kernel, or for other stacks (for instance
 * the interrupt supervisor stack).
 *
 * \note For this function to work, the stack must have been filled at startup with
 * CONFIG_KERN_STACKFILLCODE.
 */
size_t monitor_check_stack(cpustack_t* stack_base, size_t stack_size);


/*! Print a report of the stack status through kdebug */
void monitor_report(void);


#endif /* CONFIG_KERN_MONITOR */
#endif /* KERN_MONITOR_H */

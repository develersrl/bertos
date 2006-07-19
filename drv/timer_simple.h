/**
 * \file
 * <!--
 * Copyright 2004 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 * \brief Simple delay routine
 *
 * \version $Id$
  * \author Francesco Sacchi <batt@develer.com>
 */

/*#*
 *#* $Log$
 *#* Revision 1.2  2006/07/19 12:56:26  bernie
 *#* Convert to new Doxygen style.
 *#*
 *#* Revision 1.1  2005/04/12 01:37:50  bernie
 *#* Import into DevLib.
 *#*
 *#* Revision 1.2  2005/04/12 01:18:09  bernie
 *#* time_t -> mtime_t.
 *#*
 *#* Revision 1.1  2004/10/13 16:36:32  batt
 *#* Simplified timer delay routines.
 *#*
 *#*
 */
#ifndef TIMER_SIMPLE_H
#include <compiler.h>

extern void timer_delay(mtime_t time);
#define TIMER_SIMPLE_H

#endif /* TIMER_SIMPLE_H */

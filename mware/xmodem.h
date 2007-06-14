/**
 * \file
 * <!--
 * Copyright 2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 1999, 2001 Bernardo Innocenti <bernie@develer.com>
 * This file is part of DevLib - See README.devlib for information.
 * -->
 * \brief X-Modem serial transmission protocol (interface)
 *
 * \version $Id$
 * \author Bernardo Innocenti <bernie@develer.com>
 * \author Francesco Sacchi <batt@develer.com>
 */

/*#*
 *#* $Log$
 *#* Revision 1.7  2007/06/14 14:42:28  batt
 *#* Move kfile to kern/ directory; remove duplicate file.h.
 *#*
 *#* Revision 1.6  2007/06/07 09:10:44  batt
 *#* Fix some todos.
 *#*
 *#* Revision 1.5  2006/07/19 12:56:28  bernie
 *#* Convert to new Doxygen style.
 *#*
 *#* Revision 1.4  2005/11/04 16:20:02  bernie
 *#* Fix reference to README.devlib in header.
 *#*
 *#* Revision 1.3  2004/08/25 14:12:09  rasky
 *#* Aggiornato il comment block dei log RCS
 *#*
 *#* Revision 1.2  2004/08/15 05:31:46  bernie
 *#* Add an #error to spread some FUD about the quality of this module;
 *#* Add a few TODOs from Rasky's review;
 *#* Update to the new drv/ser.c API;
 *#* Move FlushSerial() to drv/ser.c and generalize.
 *#*
 *#* Revision 1.1  2004/08/11 19:54:22  bernie
 *#* Import XModem protocol into DevLib.
 *#*
 *#*/
#ifndef MWARE_XMODEM_H
#define MWARE_XMODEM_H
#include <cfg/compiler.h>
#include <kern/kfile.h>

/**
 * Called to know if we want to abort data tranfer.
 * Redefine to whatever you need.
 * \{
 */
#ifndef XMODEM_CHECK_ABORT
#define XMODEM_CHECK_ABORT (false)
#endif
/*\}*/


/**
 * Called to printf progress messages.
 * Default to kprintf debug, redefine to whatever you need.
 * \{
 */
#ifndef XMODEM_PROGRESS
#define XMODEM_PROGRESS(msg, ...) kprintf(msg, ## __VA_ARGS__)
#endif


/* fwd decl */
struct Serial;

bool xmodem_recv(struct Serial *port, KFile *fd);
bool xmodem_send(struct Serial *port, KFile *fd);

#endif /* MWARE_XMODEM_H */

/*!
 * \file
 * <!--
 * Copyright 2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 1999, 2001 Bernardo Innocenti <bernie@develer.com>
 * This file is part of DevLib - See devlib/README for information.
 * -->
 * \brief X-Modem serial transmission protocol (interface)
 *
 * \version $Id$
 * \author Bernardo Innocenti <bernie@develer.com>
 */

/*
 * $Log$
 * Revision 1.1  2004/08/11 19:54:22  bernie
 * Import XModem protocol into DevLib.
 *
 */
#ifndef MWARE_XMODEM_H
#define MWARE_XMODEM_H

/* fwd decl */
struct _KFile;

bool xmodem_recv(struct _KFile *fd);
bool xmodem_send(struct _KFile *fd);

#endif /* MWARE_XMODEM_H */


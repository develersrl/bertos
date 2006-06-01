/**
 * \file
 * <!--
 * Copyright 2006 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 * \brief Implementation of the command protocol between the board and the host
 *
 * \version $Id$
 *
 * \author Marco Benelli <marco@develer.com>
 */

/*#*
 *#* $Log$
 *#* Revision 1.1  2006/06/01 12:29:21  marco
 *#* Add first simple protocol command (version request).
 *#*
 *#*/

#ifndef PROTOCOL_H
#define PROTOCOL_H

// fwd decl
struct Serial;

void protocol_init(struct Serial *ser);
void protocol_run(struct Serial *ser);

#endif // PROTOOCOL_H

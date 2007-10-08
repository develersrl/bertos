/**
 * \file
 * <!--
 * Copyright 2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 1999, 2000, 2001 Bernardo Innocenti <bernie@develer.com>
 * This file is part of DevLib - See README.devlib for information.
 * -->
 *
 * \brief General-purpose run-length {en,de}coding algorithm (interface)
 *
 * \version $Id$
 * \author Bernardo Innocenti <bernie@develer.com>
 */

/*#*
 *#* $Log$
 *#* Revision 1.4  2006/07/19 12:56:28  bernie
 *#* Convert to new Doxygen style.
 *#*
 *#* Revision 1.3  2005/11/04 16:20:02  bernie
 *#* Fix reference to README.devlib in header.
 *#*
 *#* Revision 1.2  2004/08/25 14:12:09  rasky
 *#* Aggiornato il comment block dei log RCS
 *#*
 *#* Revision 1.1  2004/08/04 02:35:54  bernie
 *#* Import simple RLE algorithm.
 *#*
 *#*/
#ifndef RLE_H
#define RLE_H

int rle(unsigned char *output, const unsigned char *input, int length);
int unrle(unsigned char *output, const unsigned char *input);

#endif /* RLE_H */

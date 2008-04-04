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
 * Copyright 2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 1999, 2000, 2001 Bernardo Innocenti <bernie@develer.com>
 *
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

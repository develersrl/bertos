/*!
 * \file
 * <!--
 * Copyright 2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 1999, 2000, 2001 Bernardo Innocenti <bernie@develer.com>
 * This file is part of DevLib - See devlib/README for information.
 * -->
 *
 * \brief General-purpose run-length {en,de}coding algorithm (interface)
 *
 * \version $Id$
 * \author Bernardo Innocenti <bernie@develer.com>
 */

/*
 * $Log$
 * Revision 1.1  2004/08/04 02:35:54  bernie
 * Import simple RLE algorithm.
 *
 */
#ifndef RLE_H
#define RLE_H

int rle(unsigned char *output, const unsigned char *input, int length);
int unrle(unsigned char *output, const unsigned char *input);

#endif /* RLE_H */

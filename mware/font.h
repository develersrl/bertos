/*!
 * \file
 * <!--
 * Copyright (C) 2003,2004 Develer S.r.l. (http://www.develer.com/)
 * All Rights Reserved.
 * -->
 *
 * \version $Id$
 *
 * \author Stefano Fedrigo <aleph@develer.com>
 *
 * \brief Font 8x6 IBM-PC 8bit
 */

/*
 * $Log$
 * Revision 1.1  2004/05/23 15:43:16  bernie
 * Import mware modules.
 *
 * Revision 1.2  2004/03/24 15:48:53  bernie
 * Remove Copyright messages from Doxygen output
 *
 * Revision 1.1  2004/01/13 12:15:28  aleph
 * Move font table in program memory; add font.h
 *
 */
#ifndef FONT_H
#define FONT_H

#include "avr/pgmspace.h"

/* Font size (in pixel) */
#define FONT_WIDTH 6
#define FONT_HEIGHT 8

/*! Font table */
extern const prog_uchar font[256 * FONT_WIDTH];

#endif /* FONT_H */

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
 * Copyright 2006 Develer S.r.l. (http://www.develer.com/)
 * All Rights Reserved.
 * -->
 *
 * \version $Id$
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 *
 * \brief Custom control for graphics LCD emulation (interface)
 */

/*#*
 *#* $Log$
 *#* Revision 1.5  2006/05/28 12:17:57  bernie
 *#* Drop almost all the Qt3 cruft.
 *#*
 *#* Revision 1.4  2006/02/20 02:00:40  bernie
 *#* Port to Qt 4.1.
 *#*
 *#* Revision 1.3  2006/02/15 09:13:16  bernie
 *#* Switch to BITMAP_FMT_PLANAR_V_LSB.
 *#*
 *#* Revision 1.2  2006/02/10 12:33:51  bernie
 *#* Make emulator display a bit larger.
 *#*
 *#* Revision 1.1  2006/01/16 03:51:35  bernie
 *#* Add LCD Qt emulator.
 *#*
 *#*/

#ifndef DRV_LCD_GFX_QT_H
#define DRV_LCD_GFX_QT_H

#include <QtGui/QColor>
#include <QtGui/QFrame>

// fwd decls
class QSizePolicy;
class QPaintEvent;
class QResizeEvent;


class EmulLCD : public QFrame
{
	Q_OBJECT

public:
// Attributes
	enum { WIDTH = 128, HEIGHT = 64 };

// Construction
	EmulLCD(QWidget *parent = 0, const char *name = 0);
	virtual ~EmulLCD();

// Base class overrides
protected:
	virtual QSizePolicy sizePolicy() const;
	virtual QSize sizeHint() const;
	virtual QSize minimumSizeHint() const;
	virtual void paintEvent(QPaintEvent *event);

// Operations
public:
	void writeRaster(uint8_t *raster);

// Implementation
protected:
	/// Frame thickness
	int frame_width;

	/// LCD colors
	QColor fg_color, bg_color;

	/// Pixel storage
	unsigned char raster[(WIDTH + 7 / 8) * HEIGHT];
};

#endif // DRV_LCD_GFX_QT_H

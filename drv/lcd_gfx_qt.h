/**
 * \file
 * <!--
 * Copyright 2006 Develer S.r.l. (http://www.develer.com/)
 * All Rights Reserved.
 * -->
 *
 * \version $Id$
 *
 * \author Bernardo Innocenti <bernie@develer.com>
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
	unsigned char raster[(WIDTH * HEIGHT) / 8];
};

#endif // DRV_LCD_GFX_QT_H

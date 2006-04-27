/**
 * \file
 * <!--
 * Copyright 2006 Develer S.r.l. (http://www.develer.com/)
 * This file is part of DevLib - See README.devlib for information.
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
 *#* Revision 1.7  2006/04/27 05:43:08  bernie
 *#* Fix naming conventions.
 *#*
 *#* Revision 1.6  2006/02/20 02:00:40  bernie
 *#* Port to Qt 4.1.
 *#*
 *#* Revision 1.5  2006/02/15 09:13:16  bernie
 *#* Switch to BITMAP_FMT_PLANAR_V_LSB.
 *#*
 *#* Revision 1.4  2006/02/10 12:33:49  bernie
 *#* Make emulator display a bit larger.
 *#*
 *#* Revision 1.3  2006/01/23 23:11:07  bernie
 *#* Use RASTER_SIZE() to compute... err... the raster size.
 *#*
 *#* Revision 1.2  2006/01/17 02:30:43  bernie
 *#* Fix QImage format.
 *#*
 *#* Revision 1.1  2006/01/16 03:51:35  bernie
 *#* Add LCD Qt emulator.
 *#*
 *#*/

#include "lcd_gfx_qt.h"
#include <emul/emul.h>
#include <cfg/debug.h>
#include <gfx/gfx.h> // CONFIG_BITMAP_FMT

#if _QT < 4
	#include <qpainter.h>
	#include <qimage.h>
	#include <qsizepolicy.h>
	#include <qsize.h>
#else
	#include <QtGui/QPainter>
	#include <QtGui/QImage>
	#include <QtGui/QSizePolicy>
	#include <QtCore/QSize>
#endif

// Display colors
#define LCD_FG_COLOR 0x0, 0x0, 0x0
#define LCD_BG_COLOR 0xBB, 0xCC, 0xBB


EmulLCD::EmulLCD(QWidget *parent, const char *name) :
	QFrame(parent, name, Qt::WRepaintNoErase | Qt::WResizeNoErase),
	fg_color(LCD_FG_COLOR),
	bg_color(LCD_BG_COLOR)
{
	// initialize bitmap
	memset(raster, 0xAA, sizeof(raster));

	// set widget frame
	setFrameStyle(QFrame::Panel | QFrame::Sunken);
	frame_width = frameWidth();
}


EmulLCD::~EmulLCD()
{
	// nop
}


QSizePolicy EmulLCD::sizePolicy() const
{
	return QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed, false);
}


QSize EmulLCD::sizeHint() const
{
	return QSize(
		WIDTH + frame_width * 2,
		HEIGHT + frame_width * 2);
}


void EmulLCD::paintEvent(QPaintEvent * /*event*/)
{
	QPainter p(this);
	QImage img(raster, WIDTH, HEIGHT, 1, NULL, 0, QImage::BigEndian);

	p.setBackgroundMode(Qt::OpaqueMode);
	p.setPen(fg_color);
	p.setBackgroundColor(bg_color);
	p.drawImage(frame_width, frame_width, img);
}

void EmulLCD::writeRaster(uint8_t *new_raster)
{
#if CONFIG_BITMAP_FMT == BITMAP_FMT_PLANAR_H_MSB

	/* Straight copy */
	memcpy(raster, new_raster, sizeof(raster));

#elif CONFIG_BITMAP_FMT == BITMAP_FMT_PLANAR_V_LSB

	/* Rotation */
	for (int y = 0; y < HEIGHT; ++y)
	{
		for (int xbyte = 0; xbyte < WIDTH/8; ++xbyte)
		{
			uint8_t v = 0;
			for (int xbit = 0; xbit < 8; ++xbit)
				v |= (new_raster[(xbyte * 8 + xbit) + (y / 8) * WIDTH] & (1 << (y%8)) )
					? (1 << (7 - xbit)) : 0;

			raster[y * ((WIDTH + 7) / 8) + xbyte] = v;
		}
	}

#else
	#error Unsupported bitmap format
#endif

	repaint();
}



#include <gfx/gfx.h>
#include <cfg/debug.h>

/*!
 * Raster buffer to draw into.
 *
 * Bits in the bitmap bytes have vertical orientation,
 * as required by the LCD driver.
 */
DECLARE_WALL(wall_before_raster, WALL_SIZE)
static uint8_t lcd_raster[RASTER_SIZE(EmulLCD::WIDTH, EmulLCD::HEIGHT)];
DECLARE_WALL(wall_after_raster, WALL_SIZE)

/*! Default LCD bitmap */
struct Bitmap lcd_bitmap;

/*extern "C"*/ void lcd_init(void)
{
	//FIXME INIT_WALL(wall_before_raster);
	//FIXME INIT_WALL(wall_after_raster);
	gfx_bitmapInit(&lcd_bitmap, lcd_raster, EmulLCD::WIDTH, EmulLCD::HEIGHT);
	gfx_bitmapClear(&lcd_bitmap);
}

/*extern "C"*/ void lcd_blitBitmap(Bitmap *bm)
{
	//FIXME CHECK_WALL(wall_before_raster);
	//FIXME CHECK_WALL(wall_after_raster);
	emul->emulLCD->writeRaster(bm->raster);
}

#include "lcd_gfx_qt_moc.cpp"


/**
 * \file
 * <!--
 * Copyright 2006 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2000,2001 Bernardo Innocenti <bernie@codewiz.org>
 * This file is part of DevLib - See README.devlib for information.
 * -->
 *
 * \version $Id$
 *
 * \author Bernardo Innocenti <bernie@develer.com>
 *
 * \brief Custom Qt widget for emulating a graphics LCD display (implementation)
 */

/*#*
 *#* $Log$
 *#* Revision 1.1  2006/01/16 03:51:35  bernie
 *#* Add LCD Qt emulator.
 *#*
 *#*/

#ifndef EMULLCD_H
#define EMULLCD_H

#include <qframe.h>
#include <qfont.h>
#include <qcolor.h>

// fwd decls
class QSizePolicy;
class QPaintEvent;
class QResizeEvent;

/**
 * Qt widget to emulate a dot-matrix LCD display.
 */
class EmulLCD : public QFrame
{
	Q_OBJECT

public:
// Attributes
	enum { COLS = 20, ROWS = 4 };

// Construction
	EmulLCD(QWidget *parent = 0, const char *name = 0);
	virtual ~EmulLCD();

// Base class overrides
protected:
	virtual QSizePolicy sizePolicy() const;
	virtual QSize sizeHint() const;
	virtual void drawContents(QPainter *p);

// Operations
public:
	void MoveCursor		(int col, int row);
	void ShowCursor		(bool show = true);
	void PutChar		(unsigned char c);
	char GetChar		();
	void Clear			();
	void SetCGRamAddr	(unsigned char addr);

// Implementation
protected:
	void SetPainter(QPainter & p);
	void RedrawText(QPainter & p);
	void PrintChar(QPainter & p, int row, int col);
	void AdvanceCursor();

	QFont lcd_font;					///< Display character font
	QColor fg_color, bg_color;		///< LCD colors
	int font_width, font_height;	///< Font dimensions
	int frame_width;				///< Frame width (and height)
	int	cr_row, cr_col;				///< Cursor position
	int cgramaddr;					///< CGRAM Address (-1 disabled)
	unsigned char ddram[ROWS][COLS];///< Display data RAM
	unsigned char cgram[8*8];		///< CGRAM
	bool show_cursor;				///< Cursor enabled?
};

#endif // !defined(EMULLCD_H)


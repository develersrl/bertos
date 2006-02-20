/**
 * \file
 * <!--
 * Copyright 2006 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2001 Bernardo Innocenti <bernie@codewiz.org>
 * This file is part of DevLib - See README.devlib for information.
 * -->
 *
 * \version $Id$
 *
 * \author Bernardo Innocenti <bernie@develer.com>
 *
 * \brief QT-based widget for leyboard emulation (interface)
 */

#if !defined(EMULKBD_H)
#define EMULKBD_H

#if defined (_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#endif // _MSC_VER > 1000

#if _QT < 4
#include <qframe.h>
#else
#include <QtGui/QFrame>
#endif

// fwd decl
class QGridLayout;
class EmulKey;

class EmulKbd : public QFrame
{
	Q_OBJECT

// Data members
protected:
	QGridLayout *layout;
	int frame_width;
	int active_row;

// Construction
public:
	EmulKbd(QWidget *parent = 0, const char *name = 0, Qt::WFlags f = 0);
	virtual ~EmulKbd();

// Public methods
	void addKey(const char *label, int keycode, int row, int col, int matrix_row = -1, int matrix_col = -1);
	void setRow(int row);
	int readCols(void);

// Protected methods
protected:
	void setKey(int row, int col, bool on);

// Base class overrides
protected:
	virtual QSizePolicy sizePolicy() const;
	virtual void resizeEvent(QResizeEvent *e);
	virtual bool event(QEvent *e);

// Friends
	friend class EmulKey;
};


// Private helper class for EmulKbd
// NOTE: with protected inheritance, dynamic_cast<> does not work (gcc 2.96)
#if _QT < 4
#include <qpushbutton.h>
#else
#include <QtGui/qpushbutton.h>
#endif
class EmulKey : public QPushButton
{
	Q_OBJECT

// Data members
protected:
	int row, col;
	int keycode;

// Construction
public:
	EmulKey(EmulKbd *parent, const char *label, int keycode, int _row, int _col);
	virtual ~EmulKey();

// superclass overrides
	void setDown(bool enable);

protected slots:
	void keyPressed(void);
	void keyReleased(void);

// Friends
public:
	friend class EmulKbd;
};

#endif // !defined(EMULKBD_H)


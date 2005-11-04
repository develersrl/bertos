/*!
 * \file
 * <!--
 * Copyright 2005 Develer S.r.l. (http://www.develer.com/)
 * This file is part of DevLib - See README.devlib for information.
 * -->
 *
 * \brief Generic text LCD driver (interface).
 *
 * \version $Id$
 * \author Bernardo Innocenti <bernie@develer.com>
 * \author Stefano Fedrigo <aleph@develer.com>
 */

/*#*
 *#* $Log$
 *#* Revision 1.1  2005/11/04 18:00:42  bernie
 *#* Import into DevLib.
 *#*
 *#* Revision 1.5  2005/06/14 14:43:43  bernie
 *#* Add DevLib headers.
 *#*
 *#* Revision 1.4  2005/06/06 17:41:57  batt
 *#* Add lcd_layerSet function.
 *#*
 *#* Revision 1.3  2005/06/01 10:36:23  batt
 *#* Layer: Rename member variables and Doxygenize.
 *#*
 *#* Revision 1.2  2005/05/26 08:31:23  batt
 *#* Add layer hiding/showing.
 *#*
 *#* Revision 1.1  2005/05/24 09:17:58  batt
 *#* Move drivers to top-level.
 *#*/

#ifndef DRV_LCD_H
#define DRV_LCD_H

#include "lcd_hd44.h"

#include <cfg/macros.h>
#include <cfg/compiler.h>
#include <mware/list.h>

#include <stdarg.h> // vprintf()


/* flags for lcd_printf() */
#define LCD_NORMAL     0       /* Scrittura normale */
#define LCD_FILL       BV(0)   /* Fill rest of line with spaces */
#define LCD_CENTER     BV(1)   /* Center string in line */
#define LCD_NOCURSOR   BV(2)   /* Scrittura senza spostamento cursore */

/*! Special priority value for lcd_setLayerDepth(). */
#define LAYER_HIDDEN   -128

/* Compute LCD address from x/y coordinates */
#define LCD_POS(x,y)  ((lcdpos_t)((uint8_t)(x) + (uint8_t)(y) * (uint8_t)LCD_COLS))
#define LCD_ROW0  (LCD_COLS * 0)
#define LCD_ROW1  (LCD_COLS * 1)
#define LCD_ROW2  (LCD_COLS * 2)
#define LCD_ROW3  (LCD_COLS * 3)

/*!
 * Overwrapping layer context.
 */
typedef struct _Layer
{
	/*!
	 * Active layers are linked together in a list
	 * sorted in top to bottom order.
	 */
	DECLARE_NODE_ANON(struct _Layer)

	/*! Current XY address into this layer, for write operations. */
	lcdpos_t addr;

	/*! Priority of this layer (greater in front of lesser). */
	char pri;

	/*!
	 * Buffer per il salvataggio ed il ripristino del
	 * contenuto del display. Le posizioni che contengono
	 * il carattere '\0' sono trasparenti.
	 */
	char buf[LCD_COLS * LCD_ROWS];
} Layer;


/* Global variables */
extern Layer *lcd_DefLayer;

/* Function prototypes */
extern void lcd_init(void);
extern void lcd_test(void);

extern void lcd_moveCursor(lcdpos_t addr);
extern char lcd_setCursor(char state);

extern void lcd_setAddr(Layer *layer, lcdpos_t addr);
extern void lcd_putChar(char c, Layer *layer);
extern int  lcd_vprintf(Layer *layer, lcdpos_t addr, uint8_t mode, const char *format, va_list ap) FORMAT(printf, 4, 0);
extern int  lcd_printf(Layer *layer, lcdpos_t addr, uint8_t mode, const char *format, ...) FORMAT(printf, 4, 5);
extern void lcd_clear(Layer *layer);
extern void lcd_layerSet(Layer *layer, char c);
extern void lcd_clearLine(Layer *layer, int y);

extern void lcd_setLayerDepth(Layer *layer, char pri);
extern Layer *lcd_newLayer(char pri);
extern void lcd_deleteLayer(Layer *layer);
extern void lcd_lock(void);
extern void lcd_unlock(void);

#endif /* DRV_LCD_H */

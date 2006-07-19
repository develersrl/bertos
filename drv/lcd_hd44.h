/**
 * \file
 * <!--
 * Copyright 2005 Develer S.r.l. (http://www.develer.com/)
 * This file is part of DevLib - See README.devlib for information.
 * -->
 *
 * \brief Hitachi HD44780 and clones LCD module (interface)
 *
 * \version $Id$
 * \author Bernardo Innocenti <bernie@develer.com>
 * \author Stefano Fedrigo <aleph@develer.com>
 */

/*#*
 *#* $Log$
 *#* Revision 1.2  2006/07/19 12:56:25  bernie
 *#* Convert to new Doxygen style.
 *#*
 *#* Revision 1.1  2005/11/04 18:00:42  bernie
 *#* Import into DevLib.
 *#*
 *#* Revision 1.2  2005/06/14 14:43:43  bernie
 *#* Add DevLib headers.
 *#*
 *#* Revision 1.1  2005/05/24 09:17:58  batt
 *#* Move drivers to top-level.
 *#*
 *#* Revision 1.3  2005/04/22 13:14:58  batt
 *#* Better lcd rows and cols definition.
 *#*
 *#* Revision 1.2  2005/04/17 22:41:39  bernie
 *#* lcd_test(): Rename to lcd_hw_test() and conditionalize on CONFIG_TEST.
 *#*
 *#* Revision 1.1  2005/04/16 19:48:35  aleph
 *#* Rename lcd driver
 *#*/

#ifndef DRV_LCD_HD44_H
#define DRV_LCD_HD44_H

#include <cfg/compiler.h> /* For stdint types */

/**
 * \name Display dimensions (in chars)
 * \{
 */
#define LCD_ROWS  2
#define LCD_COLS  16
/* \} */

/**
 * \name Hitachi HD44 commands.
 * \{
 */
#define LCD_CMD_DISPLAY_INI      0x30
//#define LCD_CMD_SETFUNC        0x38   /**< 8 bits, 2 lines, 5x7 dots */
#define LCD_CMD_SETFUNC          0x28   /**< 4 bits, 2 lines, 5x7 dots */
#define LCD_CMD_DISPLAY_ON       0x0F   /**< Switch on display */
#define LCD_CMD_DISPLAY_OFF      0x08   /**< Switch off display */
#define LCD_CMD_CLEAR            0x01   /**< Clear display */
#define LCD_CMD_CURSOR_BLOCK     0x0D   /**< Show cursor (block) */
#define LCD_CMD_CURSOR_LINE      0x0F   /**< Show cursor (line) */
#define LCD_CMD_CURSOR_OFF       0x0C   /**< Hide cursor */
#define LCD_CMD_DISPLAYMODE      0x06
#define LCD_CMD_SET_CGRAMADDR    0x40
#define LCD_CMD_RESET_DDRAM      0x80
#define LCD_CMD_SET_DDRAMADDR    0x80
#define LCD_CMD_DISPLAY_SHIFT    0x18
#define LCD_CMD_MOVESHIFT_LEFT   0x00
#define LCD_CMD_MOVESHIFT_RIGHT  0x04
/*\}*/

/** Type for combined LCD cursor position (x,y). */
typedef uint8_t lcdpos_t;

void lcd_waitBusy(void);
void lcd_moveTo(uint8_t addr);
void lcd_setReg(uint8_t val);
void lcd_putc(uint8_t a, uint8_t c);
void lcd_remapChar(const char *glyph, char code);
void lcd_hw_init(void);
void lcd_hw_test(void);

#endif /* DRV_LCD_HD44_H */

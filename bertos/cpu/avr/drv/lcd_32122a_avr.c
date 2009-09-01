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
 * Copyright 2003, 2004, 2005, 2006 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2001 Bernie Innocenti <bernie@codewiz.org>
 *
 * -->
 *
 * \brief Displaytech 32122A LCD driver
 *
 * \version $Id$
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 * \author Stefano Fedrigo <aleph@develer.com>
 *
 * notest: avr
 *
 */

#include "lcd_32122a_avr.h"

#include "cfg/cfg_lcd.h"

#include <cfg/macros.h> /* BV() */
#include <cfg/debug.h>
#include <cfg/module.h>

#include <gfx/gfx.h>
#include <drv/timer.h>

#include <cpu/irq.h>
#include <cpu/types.h>

#include <avr/io.h>

#include <stdbool.h>
#include <inttypes.h>

#warning TODO:Refactor this module. Split code to hw file.

/* Configuration sanity checks */
#if !defined(CONFIG_LCD_SOFTINT_REFRESH) || (CONFIG_LCD_SOFTINT_REFRESH != 0 && CONFIG_LCD_SOFTINT_REFRESH != 1)
	#error CONFIG_LCD_SOFTINT_REFRESH must be defined to either 0 or 1
#endif
#if !defined(CONFIG_LCD_SOFTINT_REFRESH) || (CONFIG_LCD_SOFTINT_REFRESH != 0 && CONFIG_LCD_SOFTINT_REFRESH != 1)
	#error CONFIG_LCD_SOFTINT_REFRESH must be defined to either 0 or 1
#endif


#if CONFIG_LCD_SOFTINT_REFRESH

	/** Interval between softint driven lcd refresh */
#	define LCD_REFRESH_INTERVAL 20  /* 20ms -> 50fps */

#endif /* CONFIG_LCD_SOFTINT_REFRESH */

/** Number of LCD pages */
#define LCD_PAGES 4

/** Width of an LCD page */
#define LCD_PAGESIZE (LCD_WIDTH / 2)

/**
 * \name LCD I/O pins/ports
 * @{
 */
#define LCD_PF_DB0   PF4
#define LCD_PF_DB1   PF5
#define LCD_PF_DB2   PF6
#define LCD_PF_DB3   PF7
#define LCD_PD_DB4   PD4
#define LCD_PD_DB5   PD5
#define LCD_PD_DB6   PD6
#define LCD_PD_DB7   PD7
#define LCD_PB_A0    PB0
#define LCD_PE_RW    PE7
#define LCD_PE_E1    PE2
#define LCD_PE_E2    PE6
/*@}*/

/**
 * \name DB high nibble (DB[4-7])
 * @{
 */
#define LCD_DATA_HI_PORT    PORTD
#define LCD_DATA_HI_PIN     PIND
#define LCD_DATA_HI_DDR     DDRD
#define LCD_DATA_HI_SHIFT   0
#define LCD_DATA_HI_MASK    0xF0
/*@}*/

/**
 * \name DB low nibble (DB[0-3])
 * @{
 */
#define LCD_DATA_LO_PORT    PORTF
#define LCD_DATA_LO_PIN     PINF
#define LCD_DATA_LO_DDR     DDRF
#define LCD_DATA_LO_SHIFT   4
#define LCD_DATA_LO_MASK    0xF0
/*@}*/

/**
 * \name LCD bus control macros
 * @{
 */
#define LCD_CLR_A0   (PORTB &= ~BV(LCD_PB_A0))
#define LCD_SET_A0   (PORTB |=  BV(LCD_PB_A0))
#define LCD_CLR_RD   (PORTE &= ~BV(LCD_PE_RW))
#define LCD_SET_RD   (PORTE |=  BV(LCD_PE_RW))
#define LCD_CLR_E1   (PORTE &= ~BV(LCD_PE_E1))
#define LCD_SET_E1   (PORTE |=  BV(LCD_PE_E1))
#define LCD_CLR_E2   (PORTE &= ~BV(LCD_PE_E2))
#define LCD_SET_E2   (PORTE |=  BV(LCD_PE_E2))
#define LCD_SET_E(x) (PORTE |= (x))
#define LCD_CLR_E(x) (PORTE &= ~(x))
/*@}*/

/**
 * \name Chip select bits for LCD_SET_E()
 * @{
 */
#define LCDF_E1 (BV(LCD_PE_E1))
#define LCDF_E2 (BV(LCD_PE_E2))
/*@}*/

/** Read from the LCD data bus (DB[0-7]) */
#define LCD_READ ( \
		((LCD_DATA_LO_PIN & LCD_DATA_LO_MASK) >> LCD_DATA_LO_SHIFT) | \
		((LCD_DATA_HI_PIN & LCD_DATA_HI_MASK) >> LCD_DATA_HI_SHIFT) \
	)

/** Write to the LCD data bus (DB[0-7]) */
#define LCD_WRITE(d) \
	do { \
		LCD_DATA_LO_PORT = (LCD_DATA_LO_PORT & ~LCD_DATA_LO_MASK) | (((d)<<LCD_DATA_LO_SHIFT) & LCD_DATA_LO_MASK); \
		LCD_DATA_HI_PORT = (LCD_DATA_HI_PORT & ~LCD_DATA_HI_MASK) | (((d)<<LCD_DATA_HI_SHIFT) & LCD_DATA_HI_MASK); \
	} while (0)

/** Set data bus direction to output (write to display) */
#define LCD_DB_OUT \
	do { \
		LCD_DATA_LO_DDR |= LCD_DATA_LO_MASK; \
		LCD_DATA_HI_DDR |= LCD_DATA_HI_MASK; \
	} while (0)

/** Set data bus direction to input (read from display) */
#define LCD_DB_IN \
	do { \
		LCD_DATA_LO_DDR &= ~LCD_DATA_LO_MASK; \
		LCD_DATA_HI_DDR &= ~LCD_DATA_HI_MASK; \
	} while (0)

/** Delay for tEW (160ns) */
#define LCD_DELAY_WRITE \
	do { \
		NOP; \
		NOP; \
	} while (0)

/** Delay for tACC6 (180ns) */
#define LCD_DELAY_READ \
	do { \
		NOP; \
		NOP; \
		NOP; \
	} while (0)


/**
 * \name 32122A Commands
 * @{
 */
#define LCD_CMD_DISPLAY_ON  0xAF
#define LCD_CMD_DISPLAY_OFF 0xAE
#define LCD_CMD_STARTLINE   0xC0
#define LCD_CMD_PAGEADDR    0xB8
#define LCD_CMD_COLADDR     0x00
#define LCD_CMD_ADC_LEFT    0xA1
#define LCD_CMD_ADC_RIGHT   0xA0
#define LCD_CMD_STATIC_OFF  0xA4
#define LCD_CMD_STATIC_ON   0xA5
#define LCD_CMD_DUTY_32     0xA9
#define LCD_CMD_DUTY_16     0xA8
#define LCD_CMD_RMW_ON      0xE0
#define LCD_CMD_RMW_OFF     0xEE
#define LCD_CMD_RESET       0xE2
/*@}*/

MOD_DEFINE(lcd)


/* Status flags */
#define LCDF_BUSY BV(7)

#if CONFIG_LCD_WAIT
/**
 * \code
 *      __              __
 * RS   __\____________/__
 *         ____________
 * R/W  __/            \__
 *            _______
 * E1   _____/       \____
 *        ______      ____
 * DATA X/      \====/
 *
 * \endcode
 */
#define WAIT_LCD \
	do { \
		uint8_t status; \
		LCD_DB_IN; \
		do { \
			LCD_SET_RD; \
			LCD_CLR_A0; \
			LCD_SET_E1; \
			LCD_DELAY_READ; \
			status = LCD_READ; \
			LCD_CLR_E1; \
			LCD_SET_A0; \
			LCD_CLR_RD; \
		} while (status & LCDF_BUSY); \
		LCD_DB_OUT; \
	} while (0)

#else /* CONFIG_LCD_WAIT */

#define WAIT_LCD do {} while(0)

#endif /* CONFIG_LCD_WAIT */


/**
 * Raster buffer to draw into.
 *
 * Bits in the bitmap bytes have vertical orientation,
 * as required by the LCD driver.
 */
DECLARE_WALL(wall_before_raster, WALL_SIZE)
static uint8_t lcd_raster[RAST_SIZE(LCD_WIDTH, LCD_HEIGHT)];
DECLARE_WALL(wall_after_raster, WALL_SIZE)

/** Default LCD bitmap */
struct Bitmap lcd_bitmap;


#if CONFIG_LCD_SOFTINT_REFRESH

/** Timer for regular LCD refresh */
static Timer *lcd_refresh_timer;

#endif /* CONFIG_LCD_SOFTINT_REFRESH */


/*
static bool lcd_check(void)
{
	uint8_t status;
	uint16_t retries = 32768;
	PORTA = 0xFF;
	DDRA = 0x00;
	do {
		cbi(PORTC, PCB_LCD_RS);
		sbi(PORTC, PCB_LCD_RW);
		sbi(PORTC, PCB_LCD_E);
		--retries;
		NOP;
		status = PINA;
		cbi(PORTC, PCB_LCD_E);
		cbi(PORTC, PCB_LCD_RW);
	} while ((status & LCDF_BUSY) && retries);

	return (retries != 0);
}
*/


static inline void lcd_32122_cmd(uint8_t cmd, uint8_t chip)
{
	WAIT_LCD;

	/*      __              __
	 * A0   __\____________/__
	 *
	 * R/W  __________________
	 *            ______
	 * E1   _____/      \_____
	 *
	 * DATA --<============>--
	 */
	LCD_WRITE(cmd);
	//LCD_DB_OUT;
	LCD_CLR_A0;
	LCD_SET_E(chip);
	LCD_DELAY_WRITE;
	LCD_CLR_E(chip);
	LCD_SET_A0;
	//LCD_DB_IN;
}


static inline uint8_t lcd_32122_read(uint8_t chip)
{
	uint8_t data;

	WAIT_LCD;

	/**
	 * \code
	 *      __________________
	 * A0   __/            \__
	 *         ____________
	 * R/W  __/            \__
	 *            _______
	 * E1   _____/       \____
	 *
	 * DATA -------<=====>----
	 *
	 * \endcode
	 */
	LCD_DB_IN;
	//LCD_SET_A0;
	LCD_SET_RD;
	LCD_SET_E(chip);
	LCD_DELAY_READ;
	data = LCD_READ;
	LCD_CLR_E(chip);
	LCD_CLR_RD;
	//LCD_CLR_A0;
	LCD_DB_OUT;

	return data;
}

static inline void lcd_32122_write(uint8_t c, uint8_t chip)
{
	WAIT_LCD;

	/**
	 * \code
	 *      __________________
	 * A0   ___/          \___
	 *
	 * R/W  __________________
	 *            ______
	 * E1   _____/      \_____
	 *
	 * DATA -<==============>-
	 *
	 * \endcode
	 */
	LCD_WRITE(c);
	//LCD_DB_OUT;
	//LCD_SET_A0;
	LCD_SET_E(chip);
	LCD_DELAY_WRITE;
	LCD_CLR_E(chip);
	//LCD_CLR_A0;
	//LCD_DB_IN;
}

static void lcd_32122_clear(void)
{
	uint8_t page, j;

	for (page = 0; page < LCD_PAGES; ++page)
	{
		lcd_32122_cmd(LCD_CMD_COLADDR | 0, LCDF_E1 | LCDF_E2);
		lcd_32122_cmd(LCD_CMD_PAGEADDR | page, LCDF_E1 | LCDF_E2);
		for (j = 0; j < LCD_PAGESIZE; j++)
			lcd_32122_write(0, LCDF_E1 | LCDF_E2);
	}
}


static void lcd_32122_writeRaster(const uint8_t *raster)
{
	uint8_t page, rows;
	const uint8_t *right_raster;

	CHECK_WALL(wall_before_raster);
	CHECK_WALL(wall_after_raster);

	for (page = 0; page < LCD_PAGES; ++page)
	{
		lcd_32122_cmd(LCD_CMD_PAGEADDR | page, LCDF_E1 | LCDF_E2);
		lcd_32122_cmd(LCD_CMD_COLADDR | 0, LCDF_E1 | LCDF_E2);

		/* Super optimized lamer loop */
		right_raster = raster + LCD_PAGESIZE;
		rows = LCD_PAGESIZE;
		do
		{
			lcd_32122_write(*raster++, LCDF_E1);
			lcd_32122_write(*right_raster++, LCDF_E2);
		}
		while (--rows);
		raster = right_raster;
	}
}

#if CONFIG_LCD_SOFTINT_REFRESH

static void lcd_32122_refreshSoftint(void)
{
	lcd_blit_bitmap(&lcd_bitmap);
	timer_add(lcd_refresh_timer);
}

#endif /* CONFIG_LCD_SOFTINT_REFRESH */

/**
 * Set LCD contrast PWM.
 */
void lcd_32122_setPwm(int duty)
{
	ASSERT(duty >= LCD_MIN_PWM);
	ASSERT(duty <= LCD_MAX_PWM);

	OCR3C = duty;
}

/**
 * Update the LCD display with data from the provided bitmap.
 */
void lcd_32122_blitBitmap(Bitmap *bm)
{
	MOD_CHECK(lcd);
	lcd_32122_writeRaster(bm->raster);
}


/**
 * Initialize LCD subsystem.
 *
 * \note The PWM used for LCD contrast is initialized in drv/pwm.c
 *       because it is the same PWM used for output attenuation.
 */
void lcd_32122_init(void)
{
	MOD_CHECK(timer);

	// FIXME: interrupts are already disabled when we get here?!?
	cpu_flags_t flags;
	IRQ_SAVE_DISABLE(flags);

	PORTB |= BV(LCD_PB_A0);
	DDRB |= BV(LCD_PB_A0);

	PORTE &= ~(BV(LCD_PE_RW) | BV(LCD_PE_E1) | BV(LCD_PE_E2));
	DDRE |= BV(LCD_PE_RW) | BV(LCD_PE_E1) | BV(LCD_PE_E2);

/* LCD hw reset
	LCD_RESET_PORT |= BV(LCD_RESET_BIT);
	LCD_RESET_DDR |= BV(LCD_RESET_BIT);
	LCD_DELAY_WRITE;
	LCD_DELAY_WRITE;
	LCD_RESET_PORT &= ~BV(LCD_RESET_BIT);
	LCD_DELAY_WRITE;
	LCD_DELAY_WRITE;
	LCD_RESET_PORT |= BV(LCD_RESET_BIT);
*/
	/*
	 * Data bus is in output state most of the time:
	 * LCD r/w functions assume it is left in output state
	 */
	LCD_DB_OUT;

	// Wait for RST line to stabilize at Vcc.
	IRQ_ENABLE;
	timer_delay(20);
	IRQ_SAVE_DISABLE(flags);

	lcd_32122_cmd(LCD_CMD_RESET, LCDF_E1 | LCDF_E2);
	lcd_32122_cmd(LCD_CMD_DISPLAY_ON, LCDF_E1 | LCDF_E2);
	lcd_32122_cmd(LCD_CMD_STARTLINE | 0, LCDF_E1 | LCDF_E2);

	/* Initialize anti-corruption walls for raster */
	INIT_WALL(wall_before_raster);
	INIT_WALL(wall_after_raster);

	IRQ_RESTORE(flags);

	lcd_32122_clear();
	lcd_32122_setPwm(LCD_DEF_PWM);

	gfx_bitmapInit(&lcd_bitmap, lcd_raster, LCD_WIDTH, LCD_HEIGHT);
	gfx_bitmapClear(&lcd_bitmap);

#if CONFIG_LCD_SOFTINT_REFRESH

	/* Init IRQ driven LCD refresh */
	lcd_refresh_timer = timer_new();
	ASSERT(lcd_refresh_timer != NULL);
	INITEVENT_INT(&lcd_refresh_timer->expire, (Hook)lcd_refresh_softint, 0);
	lcd_refresh_timer->delay = LCD_REFRESH_INTERVAL;
	timer_add(lcd_refresh_timer);

#endif /* CONFIG_LCD_SOFTINT_REFRESH */

	MOD_INIT(lcd);
}


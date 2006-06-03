/**
 * \file
 * <!--
 * Copyright 2006 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2001 Bernardo Innocenti <bernie@codewiz.org>
 * -->
 *
 * \brief Display Blanker (implementation).
 *
 * \version $Id$
 *
 * \author Bernardo Innocenti <bernie@develer.com>
 */

#include "blanker.h"
#include <drv/kbd.h>
#include <drv/timer.h>

/* Time without input events before starting blanker */
#define BLK_BLANKTIMEOUT	(15 * 1000)	/* ms */

// TODO: move to blanker_hw.h
#include <drv/power.h>
#define BLK_LCDON	power_LcdOn()
#define BLK_LCDOFF	power_LcdOff()


/** Keyboard event handler to listen for key presses in blanker. */
static KbdHandler blk_KbdHandler;

/** Time since last key event. */
static ticks_t blk_lastevent;

/** Display blanking function is enabled. */
static bool blk_enabled;

/** Display blanker is engaged right now. */
static bool blk_active;


static bool blk_on(void)
{
	if (!blk_active)
	{
		blk_active = true;
		BLK_LCDOFF;
	}
	return true;
}


static void blk_off(void)
{
	if (blk_active)
	{
		blk_active = false;
		BLK_LCDON;
	}
}


void blk_retrigger(void)
{
	blk_lastevent = timer_clock();
	blk_off();
}

#if 0
/**
 * Matrix-like screen saver effect
 */
static void blk_hack(void)
{
	static signed char blk_colstart[LCD_COLS];
	UBYTE row, col;


	if (rand()%3 == 0)
	{
		/* Modify one column */
		col = rand() % LCD_COLS;
		blk_colstart[col] += rand() % 12 - 5;
	}

	for (col = 0; col < LCD_COLS; ++col)
	{
		if (blk_colstart[col] > 0)
		{
			--blk_colstart[col];

			/* Scroll down */
			for(row = LCD_ROWS-1; row; --row)
			{
				lcd_SetAddr(blk_layer, LCD_POS(col,row));
				lcd_PutChar(blk_layer->Buf[LCD_POS(col,row-1)], blk_layer);
			}

			/* Add new kanji */
			lcd_SetAddr(blk_layer, LCD_POS(col,0));
			lcd_PutChar((char)(rand() % 127 + 128), blk_layer);
		}
		else if (blk_colstart[col] < 0)
		{
			++blk_colstart[col];

			/* Clear tail */
			for(row = 0; row < LCD_ROWS; ++row)
			{
				if (blk_layer->Buf[LCD_POS(col,row)] != ' ')
				{
					lcd_SetAddr(blk_layer, LCD_POS(col,row));
					lcd_PutChar(' ', blk_layer);
					break;
				}
			}
		}
	}
}
#endif


static keymask_t blk_handlerFunc(keymask_t key)
{
	/* key used to turn off blanker */
	static keymask_t offkey;

	ticks_t now = timer_clock();

	/* If key pressed */
	if (key != 0)
	{
		blk_lastevent = now;
		if (blk_active)
		{
			blk_off();

			/* remember and eat key event */
			offkey = key;
			key = 0;
		}
		else if (key == offkey)
		{
			/* keep eating the key until released */
			key = 0;
		}

		/* pass key through */
		return key;
	}

	/* reset off key */
	offkey = 0;

	/* Blank timeout reached? */
	if (now - blk_lastevent > ms_to_ticks(BLK_BLANKTIMEOUT))
	{
		/* Enable blanker unless already done */
		if (!blk_active && !blk_on())
			return 0;

#if 0
		/* Do some nice visual effect */
		blk_hack();
#endif /* _DEBUG */
	}

	return 0;
}


void blk_enable(void)
{
	if (!blk_enabled)
	{
		blk_active = false;
		blk_lastevent = timer_clock();

		/* Add display blanker handler */
		blk_KbdHandler.hook = blk_handlerFunc;
		blk_KbdHandler.pri = 100; /* high priority */
		blk_KbdHandler.flags = KHF_RAWKEYS;
		kbd_addHandler(&blk_KbdHandler);

		blk_enabled = true;
	}
}


void blk_disable(void)
{
	if (blk_enabled)
	{
		kbd_remHandler(&blk_KbdHandler);
		blk_off();
		blk_enabled = false;
	}
}

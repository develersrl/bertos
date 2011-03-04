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
 * Copyright 2010 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief HX8347 low-level hardware macros for Atmel SAM3X-EK board.
 *
 * The LCD controller is connected to the cpu static memory controller.
 * LCD has 16 data lines and usual RS/WR/RD lines.  The data lines
 * are connected to the SMC data bus (D0-15), while the SCM address bus
 * (A1 only) is used to drive the RS pin.  WR/RD are connected to SMC's
 * NWE and NRD respectively.
 *
 * \author Stefano Fedrigo <aleph@develer.com>
 */

#ifndef HW_HX8347_H
#define HW_HX8347_H

#include "cfg/macros.h"
#include <io/sam3.h>
#include <drv/timer.h>


/*
 * LCD I/O pins/ports and peripherals
 */
#define LCD_DATABUS_PINS    (0xFFFF << 2)
#define LCD_DATABUS_PORT    PIOC_BASE
#define LCD_DATABUS_PERIPH  PIO_PERIPH_A

#define LCD_NRD_PIN         BV(29)
#define LCD_NRD_PORT        PIOA_BASE
#define LCD_NRD_PERIPH      PIO_PERIPH_B

#define LCD_NWE_PIN         BV(18)
#define LCD_NWE_PORT        PIOC_BASE
#define LCD_NWE_PERIPH      PIO_PERIPH_A

#define LCD_NCS2_PIN        BV(24)
#define LCD_NCS2_PORT       PIOB_BASE
#define LCD_NCS2_PERIPH     PIO_PERIPH_B

#define LCD_RS_PIN          BV(22)
#define LCD_RS_PORT         PIOC_BASE
#define LCD_RS_PERIPH       PIO_PERIPH_A


// How many cpu clocks per nanosecond.
#define CLOCKS_PER_NS(ns)  ((uint32_t)((ns * (CPU_FREQ/1000000)) / 1000) + 1)


// LCD Base Address, chip select 2
#define LCD_BASE  0x62000000

// LCD index register address
#define LCD_IR  (*(uint16_t *)(LCD_BASE))

// LCD data address (A1 drives RS signal)
#define LCD_D   (*(uint16_t *)(LCD_BASE + 2))

/**
 * Send a command to LCD controller.
 */
INLINE void hx8347_cmd(uint8_t cmd)
{
	LCD_IR = cmd;
}

/**
 * Send data to LCD controller.
 */
INLINE void hx8347_write(uint16_t data)
{
	LCD_D = data;
}

/**
 * Read data from LCD controller.
 */
INLINE uint16_t hx8347_read(void)
{
	return LCD_D;
}

/**
 * Bus initialization: on SAM3X-EK the display is wired
 * on the static memory controller, chip select 2.
 */
INLINE void hx8347_busInit(void)
{
	// Configure pins: disable PIO...
	HWREG(LCD_DATABUS_PORT + PIO_PDR_OFF) = LCD_DATABUS_PINS;
	HWREG(LCD_NRD_PORT     + PIO_PDR_OFF) = LCD_NRD_PIN;
	HWREG(LCD_NWE_PORT     + PIO_PDR_OFF) = LCD_NWE_PIN;
	HWREG(LCD_NCS2_PORT    + PIO_PDR_OFF) = LCD_NCS2_PIN;
	HWREG(LCD_RS_PORT      + PIO_PDR_OFF) = LCD_RS_PIN;

	// ... enable pull-up...
	HWREG(LCD_DATABUS_PORT + PIO_PUER_OFF) = LCD_DATABUS_PINS;
	HWREG(LCD_NRD_PORT     + PIO_PUER_OFF) = LCD_NRD_PIN;
	HWREG(LCD_NWE_PORT     + PIO_PUER_OFF) = LCD_NWE_PIN;
	HWREG(LCD_NCS2_PORT    + PIO_PUER_OFF) = LCD_NCS2_PIN;
	HWREG(LCD_RS_PORT      + PIO_PUER_OFF) = LCD_RS_PIN;

	// ... and select appropriate peripheral.
	PIO_PERIPH_SEL(LCD_DATABUS_PORT, LCD_DATABUS_PINS, LCD_DATABUS_PERIPH);
	PIO_PERIPH_SEL(LCD_NRD_PORT, LCD_NRD_PIN, LCD_NRD_PERIPH);
	PIO_PERIPH_SEL(LCD_NWE_PORT, LCD_NWE_PIN, LCD_NWE_PERIPH);
	PIO_PERIPH_SEL(LCD_NCS2_PORT, LCD_NCS2_PIN, LCD_NCS2_PERIPH);
	PIO_PERIPH_SEL(LCD_RS_PORT, LCD_RS_PIN, LCD_RS_PERIPH);

	// Enable peripheral clock
	pmc_periphEnable(SMC_SDRAMC_ID);

	// Static memory controller configuration
	SMC_SETUP2 =
		SMC_SETUP_NWE_SETUP(CLOCKS_PER_NS(10)) |
		SMC_SETUP_NCS_WR_SETUP(CLOCKS_PER_NS(10)) |
		SMC_SETUP_NRD_SETUP(CLOCKS_PER_NS(90)) |
		SMC_SETUP_NCS_RD_SETUP(CLOCKS_PER_NS(90));

	SMC_PULSE2 =
		SMC_PULSE_NWE_PULSE(CLOCKS_PER_NS(35)) |
		SMC_PULSE_NCS_WR_PULSE(CLOCKS_PER_NS(35)) |
		SMC_PULSE_NRD_PULSE(CLOCKS_PER_NS(355)) |
		SMC_PULSE_NCS_RD_PULSE(CLOCKS_PER_NS(355));

	SMC_CYCLE2 =
		SMC_CYCLE_NWE_CYCLE(CLOCKS_PER_NS(100)) |
		SMC_CYCLE_NRD_CYCLE(CLOCKS_PER_NS(460));

	SMC_MODE2 =
		SMC_MODE_WRITE_MODE | SMC_MODE_READ_MODE | SMC_MODE_DBW;
}

#endif /* HW_HX8347_H */

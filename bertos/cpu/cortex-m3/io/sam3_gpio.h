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
 * \brief AT91SAM3 GPIO hardware definitions.
 */

#ifndef SAM3_GPIO_H
#define SAM3_GPIO_H

/**
 * GPIO registers base addresses.
 */
/*\{*/
#define GPIO_PORTA_BASE  0x400E0E00
#define GPIO_PORTB_BASE  0x400E1000
#define GPIO_PORTC_BASE  0x400E1200
/*\}*/

/**
 * GPIO register offsets.
 */
/*\{*/
#define GPIO_PER       0x00  //< PIO Enable Register
#define GPIO_PDR       0x04  //< PIO Disable Register
#define GPIO_PSR       0x08  //< PIO Status Register
#define GPIO_OER       0x10  //< Output Enable Register
#define GPIO_ODR       0x14  //< Output Disable Register
#define GPIO_OSR       0x18  //< Output Status Register
#define GPIO_IFER      0x20  //< Glitch Input Filter Enable Register
#define GPIO_IFDR      0x24  //< Glitch Input Filter Disable Register
#define GPIO_IFSR      0x28  //< Glitch Input Filter Status Register
#define GPIO_SODR      0x30  //< Set Output Data Register
#define GPIO_CODR      0x34  //< Clear Output Data Register
#define GPIO_ODSR      0x38  //< Output Data Status Register
#define GPIO_PDSR      0x3C  //< Pin Data Status Register
#define GPIO_IER       0x40  //< Interrupt Enable Register
#define GPIO_IDR       0x44  //< Interrupt Disable Register
#define GPIO_IMR       0x48  //< Interrupt Mask Register
#define GPIO_ISR       0x4C  //< Interrupt Status Register
#define GPIO_MDER      0x50  //< Multi-driver Enable Register
#define GPIO_MDDR      0x54  //< Multi-driver Disable Register
#define GPIO_MDSR      0x58  //< Multi-driver Status Register
#define GPIO_PUDR      0x60  //< Pull-up Disable Register
#define GPIO_PUER      0x64  //< Pull-up Enable Register
#define GPIO_PUSR      0x68  //< Pad Pull-up Status Register
#define GPIO_ABCDSR1   0x70  //< Peripheral Select Register 1
#define GPIO_ABCDSR2   0x74  //< Peripheral Select Register 2
#define GPIO_IFSCDR    0x80  //< Input Filter Slow Clock Disable Register
#define GPIO_IFSCER    0x84  //< Input Filter Slow Clock Enable Register
#define GPIO_IFSCSR    0x88  //< Input Filter Slow Clock Status Register
#define GPIO_SCDR      0x8C  //< Slow Clock Divider Debouncing Register
#define GPIO_PPDDR     0x90  //< Pad Pull-down Disable Register
#define GPIO_PPDER     0x94  //< Pad Pull-down Enable Register
#define GPIO_PPDSR     0x98  //< Pad Pull-down Status Register
#define GPIO_OWER      0xA0  //< Output Write Enable
#define GPIO_OWDR      0xA4  //< Output Write Disable
#define GPIO_OWSR      0xA8  //< Output Write Status Register
#define GPIO_AIMER     0xB0  //< Additional Interrupt Modes Enable Register
#define GPIO_AIMDR     0xB4  //< Additional Interrupt Modes Disables Register
#define GPIO_AIMMR     0xB8  //< Additional Interrupt Modes Mask Register
#define GPIO_ESR       0xC0  //< Edge Select Register
#define GPIO_LSR       0xC4  //< Level Select Register
#define GPIO_ELSR      0xC8  //< Edge/Level Status Register
#define GPIO_FELLSR    0xD0  //< Falling Edge/Low Level Select Register
#define GPIO_REHLSR    0xD4  //< Rising Edge/ High Level Select Register
#define GPIO_FRLHSR    0xD8  //< Fall/Rise - Low/High Status Register
#define GPIO_LOCKSR    0xE0  //< Lock Status
#define GPIO_WPMR      0xE4  //< Write Protect Mode Register
#define GPIO_WPSR      0xE8  //< Write Protect Status Register
#define GPIO_SCHMITT  0x100  //< Schmitt Trigger Register
/*\}*/

#endif /* SAM3_GPIO_H */

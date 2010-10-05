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
 * \brief ATSAM3 enhanced embedded flash controller definitions.
 */

#ifndef SAM3_FLASH_H
#define SAM3_FLASH_H

/**
 * EEFC registers.
 */
/*\{*/
#define EEFC_FMR_R  (*((reg32_t *)0x400E0A00))  ///< Flash Mode Register
#define EEFC_FCR_R  (*((reg32_t *)0x400E0A04))  ///< Flash Command Register
#define EEFC_FSR_R  (*((reg32_t *)0x400E0A08))  ///< Flash Status Register
#define EEFC_FRR_R  (*((reg32_t *)0x400E0A0C))  ///< Flash Result Register
/*\}*/

/**
 * EFC register addresses.
 */
/*\{*/
#define EEFC_FMR  0x400E0A00   ///< Flash Mode Register
#define EEFC_FCR  0x400E0A04   ///< Flash Command Register
#define EEFC_FSR  0x400E0A08   ///< Flash Status Register
#define EEFC_FRR  0x400E0A0C   ///< Flash Result Register
/*\}*/


/**
 * Defines for bit fields in EEFC_FMR register.
 */
/*\{*/
#define EEFC_FMR_FRDY        BV(0)                   ///< Ready Interrupt Enable
#define EEFC_FMR_FWS_S       8
#define EEFC_FMR_FWS_M       (0xf << EEFC_FMR_FWS_S) ///< Flash Wait State
#define EEFC_FMR_FWS(value)  (EEFC_FMR_FWS_M & ((value) << EEFC_FMR_FWS_S))
#define EEFC_FMR_FAM BV(24)                          ///< Flash Access Mode
/*\}*/

/**
 * Defines for bit fields in EEFC_FCR register.
 */
/*\{*/
#define EEFC_FCR_FCMD_M       0xff                        ///< Flash Command
#define EEFC_FCR_FCMD(value)  (EEFC_FCR_FCMD_M & (value))
#define EEFC_FCR_FARG_S       8
#define EEFC_FCR_FARG_M       (0xffff << EEFC_FCR_FARG_S) ///< Flash Command Argument
#define EEFC_FCR_FARG(value)  (EEFC_FCR_FARG_M & ((value) << EEFC_FCR_FARG_S))
#define EEFC_FCR_FKEY_S       24
#define EEFC_FCR_FKEY_M       (0xff << EEFC_FCR_FKEY_S)   ///< Flash Writing Protection Key
#define EEFC_FCR_FKEY(value)  (EEFC_FCR_FKEY_M & ((value) << EEFC_FCR_FKEY_S))
/*\}*/

/**
 * Defines for bit fields in EEFC_FSR register.
 */
/*\{*/
#define EEFC_FSR_FRDY    BV(0)  ///< Flash Ready Status
#define EEFC_FSR_FCMDE   BV(1)  ///< Flash Command Error Status
#define EEFC_FSR_FLOCKE  BV(2)  ///< Flash Lock Error Status
/*\}*/

#endif /* SAM3_FLASH_H */

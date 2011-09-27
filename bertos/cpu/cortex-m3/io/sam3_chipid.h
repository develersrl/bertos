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
 * Copyright 2011 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief CHIP ID SAM3 definitions.
 */

#ifndef SAM3_CHIPID_H
#define SAM3_CHIPID_H

/**
 * CHIP ID base registers addresses.
 */
#define CHIPID_CIDR      (*((reg32_t *)(0x400E0940)))

#define CHIPID_VERSION_MASK     0x1F
#define CHIPID_VERSION()        ((CHIPID_CIDR) & CHIPID_VERSION_MASK) ///< Current version of the device.

#define CHIPID_EPRCOC_SHIFT     5
#define CHIPID_EPRCOC_MASK      0xE0
#define CHIPID_EPRCOC()      (((CHIPID_CIDR) & CHIPID_EPRCOC_MASK) >>  CHIPID_EPRCOC_SHIFT)    ///< Embedded processor.

#define CHIPID_NVPSIZ_SHIFT      8
#define CHIPID_NVPSIZ_MASK      0xF00      ///< Nonvolatile program memory size.
#define CHIPID_NVPSIZ()      (((CHIPID_CIDR) & CHIPID_NVPSIZ_MASK) >>  CHIPID_NVPSIZ_SHIFT) ///< Nonvolatile program memory size.

#define CHIPID_NVPSIZ2_SHIFT     12
#define CHIPID_NVPSIZ2_MASK     0xF000     ///< Second nonvolatile program memory size.
#define CHIPID_NVPSIZ2()      (((CHIPID_CIDR) & CHIPID_NVPSIZ2_MASK) >>  CHIPID_NVPSIZ2_SHIFT) ///< Second nonvolatile program memory size.

#define CHIPID_SRAMSIZ_SHIFT     16
#define CHIPID_SRAMSIZ_MASK     0xF0000    ///< Internal SRAM size.
#define CHIPID_SRAMSIZ()      (((CHIPID_CIDR) & CHIPID_SRAMSIZ_MASK) >>  CHIPID_SRAMSIZ_SHIFT) ///< Internal SRAM size.

#define CHIPID_ARCH_SHIFT        20
#define CHIPID_ARCH_MASK        0xFF00000  ///< Architecture identifier.
#define CHIPID_ARCH()      (((CHIPID_CIDR) & CHIPID_ARCH_MASK) >>  CHIPID_ARCH_SHIFT) ///< Architecture identifier.

#define CHIPID_NVTYP_SHIFT       28
#define CHIPID_NVTYP_MASK       0x70000000 ///< Nonvolatile program memory type.
#define CHIPID_NVTYP()      (((CHIPID_CIDR) & CHIPID_NVTYP_MASK) >>  CHIPID_NVTYP_SHIFT) ///< Nonvolatile program memory type.


const char *chipid_eproc_name(int idx);
const char *chipid_nvpsize(int idx);
const char *chipid_sramsize(int idx);
const char *chipid_archnames(unsigned value);
const char *chipid_nvptype(int idx);

#endif /* SAM3_CHIPID_H */

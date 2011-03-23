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
 * \defgroup dac Generic DAC driver
 * \ingroup drivers
 * \{
 * \brief Digital to Analog Converter driver (DAC).
 *
 * <b>Configuration file</b>: cfg_dac.h
 *
 * \author Daniele Basile <asterix@develer.com>
 *
 * $WIZ$ module_name = "dac"
 * $WIZ$ module_configuration = "bertos/cfg/cfg_dac.h"
 * $WIZ$ module_supports = "sam3x"
 */


#ifndef DRV_DAC_H
#define DRV_DAC_H

#include <cfg/compiler.h>
#include <cfg/debug.h>
#include <cpu/attr.h>

int dac_write(int ch, void *buf, size_t len);

INLINE int dac_putHalfWord(int ch, uint16_t sample)
{
	return dac_write(ch, &sample, sizeof(uint16_t));
}

INLINE int dac_putWord(int ch, uint32_t sample)
{
	return dac_write(ch, &sample, sizeof(uint32_t));
}

void dac_init(void);

/** \} */ //defgroup dac
#endif /* DRV_DAC_H */

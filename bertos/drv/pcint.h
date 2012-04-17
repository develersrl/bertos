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
 * \defgroup pcint_driver Pin change interrupt driver
 * \ingroup drivers
 * \{
 * \brief Pin change interrupt driver for Atmega controllers.
 *
 * <b>Configuration file</b>: cfg_pcint.h
 *
 * \author Tóth Balázs <balazs.toth@jarkon.hu>
 *
 * $WIZ$ module_name = "pcint"
 * $WIZ$ module_configuration = "bertos/cfg/cfg_pcint.h"
 * $WIZ$ module_hw = "bertos/hw/hw_pcint.h"
 * $WIZ$ supports = "atmega1280 and atmega1281 and atmega2560 and atmega88p and atmega168 and atmega328p and atmega324p and atmega644p"
 */

#ifndef DRV_PCINT_H
#define DRV_PCINT_H

#include <cfg/compiler.h>

#include <cpu/detect.h>
#include <cpu/types.h>
#include <cpu/attr.h>



typedef uint8_t pcint_mask_t;

/**
 * PCINT context structure
 */
typedef struct Pcint
{
	uint8_t unit;       ///< Physical port number
	pcint_mask_t mask;       ///< Pin mask
	bool inited;

} Pcint;


#include CPU_HEADER(pcint)

void pcint_init(struct Pcint *ctx, uint8_t unit);

/* Low level prototype. */
void pcint_hw_init(uint8_t unit);
void pcint_hw_setmask(uint8_t unit, pcint_mask_t mask);


/**
 * Set PCINT mask.
 * \note This function sets mask of context only.
 * Use it together with pcint_start(struct Pcint *ctx).
 */
INLINE void pcint_setmask(struct Pcint *ctx, pcint_mask_t mask)
{
	ctx->mask = mask;
};


/**
 * Start pin change detection.
 * \note This function alters the masks, not the interrupt enable flags.
 */
INLINE void pcint_start(struct Pcint *ctx)
{
	pcint_hw_setmask(ctx->unit, ctx->mask);
}


/**
 * Stop pin change detection.
 * \note This function alters the masks, not the interrupt enable flags.
 */
INLINE void pcint_stop(struct Pcint *ctx)
{
	pcint_hw_setmask(ctx->unit, 0);
}

/** \} */ //defgroup pcint_driver
#endif /* DRV_PCINT_H */

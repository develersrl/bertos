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
 * \brief DMAC utility interface.
 *
 * \author Daniele Basile <asterix@develer.com>
 */

#ifndef DRV_DMAC_SAM3_H
#define DRV_DMAC_SAM3_H

#include <cfg/macros.h>

#include <cpu/types.h>

/**
 * DMA Transfer Descriptor as well as Linked List Item
 */
typedef struct DmacDesc
{
    uint32_t src_addr;     ///< Source buffer address
    uint32_t dst_addr;     ///< Destination buffer address
    uint32_t ctrl_a;       ///< Control A register settings
    uint32_t ctrl_b;       ///< Control B register settings
    uint32_t dsc_addr;     ///< Next descriptor address
} DmacDesc;

typedef struct Dmac
{
	uint8_t errors;
	size_t transfer_size;
} Dmac;

#define DMAC_ERR_CH_ALREDY_ON    BV(0)

void dmac_setSources(Dmac *dmac, uint8_t ch, uint32_t src, uint32_t dst, size_t transfer_size);
void dmac_configureDmac(Dmac *dmac, uint8_t ch, uint32_t cfg, uint32_t ctrla, uint32_t ctrlb);
int dmac_start(Dmac *dmac, uint8_t ch);
bool dmac_isDone(Dmac *dmac, uint8_t ch);
bool dmac_waitDone(Dmac *dmac, uint8_t ch);

void dmac_init(Dmac *dmac);

#endif /* DRV_DMAC_SAM3_H */

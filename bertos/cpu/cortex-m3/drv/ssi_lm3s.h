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
 * \brief LM3S1968 Synchronous Serial Interface (SSI) driver.
 */

#ifndef SSI_LM3S_H
#define SSI_LM3S_H

#include <cpu/power.h> /* cpu_relax() */

/**
 * LM3S1968 SSI frame format
 */
/*\{*/
#define SSI_FRF_MOTO_MODE_0     0x00000000  //< Moto fmt, polarity 0, phase 0
#define SSI_FRF_MOTO_MODE_1     0x00000002  //< Moto fmt, polarity 0, phase 1
#define SSI_FRF_MOTO_MODE_2     0x00000001  //< Moto fmt, polarity 1, phase 0
#define SSI_FRF_MOTO_MODE_3     0x00000003  //< Moto fmt, polarity 1, phase 1
#define SSI_FRF_TI              0x00000010  //< TI frame format
#define SSI_FRF_NMW             0x00000020  //< National MicroWire frame format
/*\}*/

/**
 * LM3S1968 SSI operational mode
 */
/*\{*/
#define SSI_MODE_MASTER         0x00000000  //< SSI master
#define SSI_MODE_SLAVE          0x00000001  //< SSI slave
#define SSI_MODE_SLAVE_OD       0x00000002  //< SSI slave with output disabled
/*\}*/

int lm3s_ssi_init(uint32_t base, uint32_t frame, int mode,
                   unsigned int bitrate, unsigned int data_width);
void lm3s_ssi_enable(uint32_t base);
void lm3s_ssi_disable(uint32_t base);
void lm3s_ssi_write_frame(uint32_t base, uint32_t val);
int lm3s_ssi_write_frame_nonblocking(uint32_t base, uint32_t val);
void lm3s_ssi_read_frame(uint32_t base, uint32_t *val);
int lm3s_ssi_read_frame_nonblocking(uint32_t base, uint32_t *val);
bool lm3s_ssi_txdone(uint32_t base);

INLINE void lm3s_ssi_wait_txdone(uint32_t base)
{
	while (!lm3s_ssi_txdone(base))
		cpu_relax();
}

#endif /* LM3S_SSI_H */

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
 * Copyright 2008 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief AFSK1200 modem.
 *
 * \version $Id$
 * \author Francesco Sacchi <asterix@develer.com>
 *
 * $WIZ$ module_name = "afsk"
 * $WIZ$ module_configuration = "bertos/cfg/cfg_afsk.h"
 * $WIZ$ module_depends = "timer", "kfile"
 * $WIZ$ module_hw = "bertos/hw/hw_afsk.h"
 */

#ifndef DRV_AFSK_H
#define DRV_AFSK_H

#include <kern/kfile.h>
#include <cfg/compiler.h>
#include "hw/hw_afsk.h"

typedef struct Afsk
{
  KFile fd;
} Afsk;

#define KFT_AFSK MAKE_ID('A', 'F', 'S', 'K')

INLINE Afsk *AFSK_CAST(KFile *fd)
{
  ASSERT(fd->_type == KFT_AFSK);
  return (Afsk *)fd;
}

void afsk_init(Afsk *af);

/**
 * \name afsk filter type
 * $WIZ$ afsk_filter_list = "AFSK_BUTTERWORTH", "AFSK_CHEBYSHEV"
 * \{
 */
#define AFSK_BUTTERWORTH  0
#define AFSK_CHEBYSHEV    1
/* \} */


#endif

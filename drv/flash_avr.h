/**
 * \file
 * <!--
 * Copyright 2007 Develer S.r.l. (http://www.develer.com/)
 * This file is part of DevLib - See README.devlib for information.
 * -->
 *
 * \brief Self programming routines (interface).
 *
 * \version $Id$
 * \author Francesco Sacchi <batt@develer.com>
 * \author Daniele Basile <asterix@develer.com>
 */

#ifndef DRV_FLASH_AVR_H
#define DRV_FLASH_AVR_H

#include <cfg/compiler.h>
#include <kern/kfile.h>

bool flash_avr_test(void);
void flash_avr_init(struct _KFile *fd);


#endif /* DRV_FLASH_AVR_H */

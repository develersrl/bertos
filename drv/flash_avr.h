/**
 * \file
 * <!--
 * Copyright 2007 Develer S.r.l. (http://www.develer.com/)
 * All Rights Reserved.
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

bool avrflash_test(void);
void avrflash_init(struct _KFile *fd);


#endif /* DRV_FLASH_AVR_H */

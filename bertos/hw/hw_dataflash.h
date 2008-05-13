/**
 * \file
 * <!--
 * Copyright 2008 Develer S.r.l. (http://www.develer.com/)
 * All Rights Reserved.
 * -->
 *
 * \brief Dataflash HW control routines (interface).
 *
 * \version $Id$
 * \author Francesco Sacchi <batt@develer.com>
 */

#ifndef HW_DATAFLASH_H
#define HW_DATAFLASH_H

#include <cfg/compiler.h>

void dataflash_hw_init(void);
void dataflash_hw_setCS(bool enable);

#endif /* HW_DATAFLASH_H */

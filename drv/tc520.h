/**
 * \file
 * <!--
 * Copyright 2006 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 * \version $Id$
 *
 * \brief TC520 ADC driver (intercace)
 *
 * \version $Id$
 * \author Francesco Sacchi <batt@develer.com>
 * \author Marco Benelli <marco@develer.com>
 */

#ifndef DRV_TC520_H
#define DRV_TC520_H

#include <drv/ser.h>
#include <cfg/compiler.h>

typedef uint32_t tc520_data_t;

/* 17 bit max value */
#define TC520_MAX_VALUE 0x1FFFFUL

tc520_data_t tc520_read(void);
void tc520_init(Serial *spi_port);

#endif  /* DRV_TC520_H */

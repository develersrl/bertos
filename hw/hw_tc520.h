/**
 * \file
 * <!--
 * Copyright 2005 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 * \version $Id$
 *
 * \brief TC520 hardware-specific definitions
 *
 * \version $Id$
 * \author Francesco Sacchi <batt@develer.com>
 */

#ifndef HW_TC520_H
#define HW_TC520_H

#include <cfg/compiler.h>
#include <avr/io.h>

#define CE_PIN   PE6
#define DV_PIN   PE5
#define LOAD_PIN PE3
#define READ_PIN PE4

#define TC520_DDR  DDRE
#define TC520_PORT PORTE
#define TC520_PIN  PINE

#define DV_HIGH() (TC520_PIN & BV(DV_PIN))
#define DV_LOW()  (!DV_HIGH())

#define CE_HIGH() (TC520_PORT |= BV(CE_PIN))
#define CE_LOW()  (TC520_PORT &= ~BV(CE_PIN))

#define LOAD_HIGH() (TC520_PORT |= BV(LOAD_PIN))
#define LOAD_LOW()  (TC520_PORT &= ~BV(LOAD_PIN))

#define READ_HIGH() (TC520_PORT |= BV(READ_PIN))
#define READ_LOW()  (TC520_PORT &= ~BV(READ_PIN))

#define TC520_HW_INIT \
do\
{\
	TC520_PORT |= (BV(CE_PIN) | BV(LOAD_PIN) | BV(READ_PIN));\
	TC520_DDR  |= (BV(CE_PIN) | BV(LOAD_PIN) | BV(READ_PIN));\
	TC520_DDR  &= ~BV(DV_PIN);\
}\
while(0)

#endif /* HW_TC520_H */

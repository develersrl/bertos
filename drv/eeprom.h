/*!
 * \file
 * <!--
 * Copyright 2003, 2004 Develer S.r.l. (http://www.develer.com/)
 * All Rights Reserved.
 * -->
 *
 * \version $Id$
 *
 * \author Stefano Fedrigo <aleph@develer.com>
 *
 * \brief I2C eeprom driver
 */

/*
 * $Log$
 * Revision 1.1  2004/07/20 17:11:18  bernie
 * Import into DevLib.
 *
 */
#ifndef DRV_EEPROM_H
#define DRV_EEPROM_H

#include "compiler.h"

//! Eeprom block size
#define EEPROM_BLKSIZE 0x40

//! Type for eeprom addresses
typedef uint16_t e2addr_t;

bool eeprom_write(e2addr_t addr, const void *buf, size_t count);
bool eeprom_read(e2addr_t addr, void *buf, size_t count);
bool eeprom_write_char(e2addr_t addr, char c);
int eeprom_read_char(e2addr_t addr);
void eeprom_init(void);
void eeprom_test(void);

#endif /* DRV_EEPROM_H */

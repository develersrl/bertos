/*!
 * \file
 * <!--
 * Copyright 2003, 2004, 2005 Develer S.r.l. (http://www.develer.com/)
 * This file is part of DevLib - See devlib/README for information.
 * -->
 *
 * \version $Id$
 *
 * \author Stefano Fedrigo <aleph@develer.com>
 * \author Bernardo Innocenti <bernie@develer.com>
 *
 * \brief Driver for the AVR ATMega TWI (interface)
 */

/*#*
 *#* $Log$
 *#* Revision 1.1  2005/01/06 16:09:40  aleph
 *#* Split twi/eeprom functions from eeprom module in separate twi module
 *#*
 *#*/
#ifndef DRV_TWI_H
#define DRV_TWI_H

#include <compiler.h>

bool twi_start_w(uint8_t slave_addr);
bool twi_start_r(uint8_t slave_addr);
void twi_stop(void);
bool twi_send(const void *_buf, size_t count);
bool twi_recv(void *_buf, size_t count);
void twi_init(void);

#endif /* DRV_EEPROM_H */

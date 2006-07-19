/**
 * \file
 * <!--
 * Copyright 2003, 2004, 2005 Develer S.r.l. (http://www.develer.com/)
 * This file is part of DevLib - See README.devlib for information.
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
 *#* Revision 1.5  2006/07/19 12:56:26  bernie
 *#* Convert to new Doxygen style.
 *#*
 *#* Revision 1.4  2006/03/20 17:49:49  bernie
 *#* Make the TWI driver more generic to work with devices other than EEPROMS.
 *#*
 *#* Revision 1.3  2005/04/11 19:10:28  bernie
 *#* Include top-level headers from cfg/ subdir.
 *#*
 *#* Revision 1.2  2005/02/18 11:19:52  bernie
 *#* Update copyright info.
 *#*
 *#*/
#ifndef DRV_TWI_H
#define DRV_TWI_H

#include <cfg/compiler.h>

bool twi_start_w(uint8_t id);
bool twi_start_r(uint8_t id);
void twi_stop(void);
bool twi_put(const uint8_t data);
bool twi_send(const void *_buf, size_t count);
bool twi_recv(void *_buf, size_t count);
void twi_init(void);

#endif /* DRV_EEPROM_H */

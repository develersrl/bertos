/**
 * \file
 * <!--
 * Copyright 2003, 2004 Develer S.r.l. (http://www.develer.com/)
 * This file is part of DevLib - See README.devlib for information.
 * -->
 *
 * \version $Id$
 *
 * \author Stefano Fedrigo <aleph@develer.com>
 * \author Bernardo Innocenti <bernie@develer.com>
 *
 * \brief Driver for the 24xx16 and 24xx256 I2C EEPROMS (interface)
 */

/*#*
 *#* $Log$
 *#* Revision 1.9  2006/07/19 12:56:25  bernie
 *#* Convert to new Doxygen style.
 *#*
 *#* Revision 1.8  2005/11/27 23:33:40  bernie
 *#* Use appconfig.h instead of cfg/config.h.
 *#*
 *#* Revision 1.7  2005/11/04 16:20:02  bernie
 *#* Fix reference to README.devlib in header.
 *#*
 *#* Revision 1.6  2005/04/11 19:10:27  bernie
 *#* Include top-level headers from cfg/ subdir.
 *#*
 *#* Revision 1.5  2004/11/02 17:50:02  bernie
 *#* CONFIG_EEPROM_VERIFY: New config option.
 *#*
 *#* Revision 1.4  2004/08/25 14:12:08  rasky
 *#* Aggiornato il comment block dei log RCS
 *#*
 *#* Revision 1.3  2004/08/10 06:57:22  bernie
 *#* eeprom_erase(): New function.
 *#*
 *#* Revision 1.2  2004/07/29 22:57:09  bernie
 *#* Add 24LC16 support.
 *#*
 *#* Revision 1.1  2004/07/20 17:11:18  bernie
 *#* Import into DevLib.
 *#*
 *#*/
#ifndef DRV_EEPROM_H
#define DRV_EEPROM_H

#include <cfg/compiler.h>
#include <appconfig.h>

/**
 * \name Values for CONFIG_EEPROM_TYPE
 * \{
 */
#define EEPROM_24XX16 1
#define EEPROM_24XX256 2
/*\}*/

#if CONFIG_EEPROM_TYPE == EEPROM_24XX16
	#define EEPROM_BLKSIZE   0x10 ///< Eeprom block size (16byte)
	#define EEPROM_SIZE     0x800 ///< Eeprom total size (2kB)
#elif CONFIG_EEPROM_TYPE == EEPROM_24XX256
	#define EEPROM_BLKSIZE   0x40 ///< Eeprom block size (64byte)
	#define EEPROM_SIZE    0x8000 ///< Eeprom total size (32kB)
#else
	#error Unsupported EEPROM type.
#endif

/// Type for EEPROM addresses
typedef uint16_t e2addr_t;

bool eeprom_write(e2addr_t addr, const void *buf, size_t count);
bool eeprom_read(e2addr_t addr, void *buf, size_t count);
bool eeprom_write_char(e2addr_t addr, char c);
int eeprom_read_char(e2addr_t addr);
void eeprom_erase(e2addr_t addr, size_t count);
void eeprom_init(void);
void eeprom_test(void);

#endif /* DRV_EEPROM_H */

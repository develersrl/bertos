/*!
 * \file
 * <!--
 * Copyright 2003,2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2001,2002,2003 Bernardo Innocenti <bernie@codewiz.org>
 * This file is part of DevLib - See README.devlib for information.
 * -->
 *
 * \version $Id$
 *
 * \author Bernardo Innocenti <bernie@develer.com>
 *
 * \brief Set system configuration
 */

/*#*
 *#* $Log$
 *#* Revision 1.2  2005/11/04 16:20:01  bernie
 *#* Fix reference to README.devlib in header.
 *#*
 *#* Revision 1.1  2005/04/11 19:04:13  bernie
 *#* Move top-level headers to cfg/ subdir.
 *#*
 *#* Revision 1.3  2004/08/25 14:12:08  rasky
 *#* Aggiornato il comment block dei log RCS
 *#*
 *#* Revision 1.2  2004/06/03 11:27:09  bernie
 *#* Add dual-license information.
 *#*
 *#* Revision 1.1  2004/05/23 17:48:35  bernie
 *#* Add top-level files.
 *#*
 *#*/

#ifndef ARCH_CONFIG_H
#define ARCH_CONFIG_H

/*!
 * \name Architectures
 * \{
 */
#error Add project-specific architectures
#define ARCH_FOO     (1<<0)
#define ARCH_BAR     (1<<1)
#define ARCH_EMUL    (1<<8)
/*\}*/

#endif /* ARCH_CONFIG_H */

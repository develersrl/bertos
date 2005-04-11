/*!
 * \file
 * <!--
 * Copyright 2003 Develer S.r.l. (http://www.develer.com/)
 * Copyright 1999,2003 Bernardo Innocenti <bernie@develer.com>
 * This file is part of DevLib - See devlib/README for information.
 * -->
 *
 * \version $Id$
 *
 * \author Bernardo Innocenti <bernie@develer.com>
 *
 * \brief Buzzer driver
 */

/*#*
 *#* $Log$
 *#* Revision 1.7  2005/04/11 19:10:27  bernie
 *#* Include top-level headers from cfg/ subdir.
 *#*
 *#* Revision 1.6  2004/12/08 09:11:53  bernie
 *#* Rename time_t to mtime_t.
 *#*
 *#* Revision 1.5  2004/08/25 14:12:08  rasky
 *#* Aggiornato il comment block dei log RCS
 *#*
 *#* Revision 1.4  2004/08/24 16:53:43  bernie
 *#* Add missing headers.
 *#*
 *#* Revision 1.3  2004/06/03 11:27:09  bernie
 *#* Add dual-license information.
 *#*
 *#* Revision 1.2  2004/05/23 18:21:53  bernie
 *#* Trim CVS logs and cleanup header info.
 *#*
 *#*/
#ifndef DRV_BUZZER_H
#define DRV_BUZZER_H

#include <cfg/compiler.h>

extern void buz_init(void);
extern void buz_beep(mtime_t time);
extern void buz_repeat_start(mtime_t duration, mtime_t interval);
extern void buz_repeat_stop(void);

#endif /* DRV_BUZZER_H */

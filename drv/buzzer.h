/*!
 * \file
 * Copyright (C) 1999,2003 Bernardo Innocenti <bernie@develer.com>
 * Copyright (C) 2003 Develer S.r.l. (http://www.develer.com/)
 * All Rights Reserved.
 *
 * \version $Id$
 *
 * \author Bernardo Innocenti <bernie@develer.com>
 *
 * \brief Buzzer driver
 */

/*
 * $Log$
 * Revision 1.1  2004/05/23 18:10:11  bernie
 * Import drv/ modules.
 *
 * Revision 1.1  2003/12/13 23:53:37  aleph
 * Add buzzer driver
 *
 */
#ifndef BUZZER_H
#define BUZZER_H

extern void buz_init(void);
extern void buz_beep(time_t time);
extern void buz_repeat_start(time_t duration, time_t interval);
extern void buz_repeat_stop(void);

#endif /* BUZZER_H */

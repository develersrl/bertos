/*!
 * \file
 * <!--
 * Copyright 2003 Develer S.r.l. (http://www.develer.com/)
 * Copyright 1999,2003 Bernardo Innocenti <bernie@develer.com>
 * All Rights Reserved.
 * -->
 *
 * \version $Id$
 *
 * \author Bernardo Innocenti <bernie@develer.com>
 *
 * \brief Buzzer driver
 */

/*
 * $Log$
 * Revision 1.2  2004/05/23 18:21:53  bernie
 * Trim CVS logs and cleanup header info.
 *
 */
#ifndef DRV_BUZZER_H
#define DRV_BUZZER_H

extern void buz_init(void);
extern void buz_beep(time_t time);
extern void buz_repeat_start(time_t duration, time_t interval);
extern void buz_repeat_stop(void);

#endif /* DRV_BUZZER_H */

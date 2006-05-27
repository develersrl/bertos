/**
 * \file
 * <!--
 * Copyright 2006 Develer S.r.l. (http://www.develer.com/)
 * This file is part of DevLib - See README.devlib for information.
 * -->
 *
 * \brief Simple notifier for the subject/observer pattern (interface)
 *
 * \version $Id$
 * \author Bernardo Innocenti <bernie@develer.com>
 */
#ifndef MWARE_OBSERVER_H
#define MWARE_OBSERVER_H

#include <mware/list.h>

/**
 *
 * Here's a simple example:
 *
 * \code
 * Subject kbd_driver;
 *
 * Observer kbd_observer;
 *
 * void key_pressed(int event, void *_param)
 * {
 *     char *param = (char *)_param;
 *
 *     if (event == EVENT_KBD_PRESSED)
 *         printf("You pressed %c\n", *param);
 * }
 *
 * void register_kbd_listener(void)
 * {
 *     observer_SetEvent(&kbd_observer, key_pressed);
 *     observer_Subscribe(&kbd_driver, &kbd_observer);
 * }
 * \endcode
 */
typedef struct Observer
{
	Node link;
	void (*event)(int event_id, void *param);
} Observer;

typedef struct Subject
{
	/// Subscribed observers.
	List observers;

} Subject;

void observer_SetEvent(Observer *observer, void (*event)(int event_id, void *param));

#define OBSERVER_INITIALIZER(callback) { { NULL, NULL }, callback }

void observer_InitSubject(Subject *subject);

/// Aggiunge un Observer all'insieme
void observer_Subscribe(Subject *subject, Observer *observer);

/// Rimuove un Observer dall'insieme
void observer_Unsubscribe(Subject *subject, Observer *observer);

/// per tutti gli elementi nel set notifica l'evento, chiamando la relativa
/// funzione event
void observer_notify(Subject *subject, int event_id, void *param);

#endif /* MWARE_OBSERVER_H */

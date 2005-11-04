/*!
 * \file
 * <!--
 * Copyright 2005 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 * \version $Id$
 *
 * \brief Notifier obj (interface).
 *
 * \version $Id$
 * \author Bernardo Innocenti <bernie@develer.com>
 * \author Francesco Sacchi <batt@develer.com>
 */

/*#*
 *#* $Log$
 *#* Revision 1.1  2005/11/04 18:26:38  bernie
 *#* Import into DevLib.
 *#*
 *#* Revision 1.4  2005/06/09 13:23:58  batt
 *#* Add some comments.
 *#*
 *#* Revision 1.3  2005/06/08 17:32:33  batt
 *#* Switch to new messaging system.
 *#*
 *#* Revision 1.2  2005/06/06 11:04:12  batt
 *#* Add some comments.
 *#*
 *#* Revision 1.1  2005/05/26 08:32:53  batt
 *#* Add new Develer widget system :)
 *#*
 *#*/
#ifndef DT_DNOTIFIER_H
#define DT_DNOTIFIER_H

#include <cfg/debug.h>
#include <dt/dtag.h>
#include <mware/list.h>

//Fwd declaretion.
struct DNotifier;
struct DFilter;

typedef void (* update_func_ptr)(struct DNotifier *, dtag_t, dval_t);
typedef void (* update_filter_ptr)(struct DFilter *, dtag_t, dval_t);

/*!
 * Base object for receive and forward messages.
 * It contains an update function used to update itslef and a list to
 * notify other DNotifer eventually connected.
 */
typedef struct DNotifier
{
	//! Receive new attributes from other notifiers.
	update_func_ptr update;

	//! List of target notifiers to set new attributes to.
	List targets;
} DNotifier;

/*!
 * Map for messages.
 * Used to translate src message to dst message.
 */
typedef struct DFilterMap
{
	DTagItem src;
	DTagItem dst;
} DFilterMap;


/*!
 * A filter is an interface between two notifier.
 * It can translate messages between them through a map (if it is not null).
 */
typedef struct DFilter
{
	//! Allow creating a list of dfilter objects.
	Node link;

	//! Target of the filter
	DNotifier *target;

	//! Update function called by the source dnotifier
	update_filter_ptr update;

	//!Map for translating messages for target
	const DFilterMap *map;

	//!Used in debug to prevent inserting this filter in more than one list
	DB(uint8_t magic;)
} DFilter;

//! Type for filter-mask checking
typedef uint16_t dfilter_mask_t;

//! Filter init
void filter_init(DFilter *f, const DFilterMap *map, bool masked, DNotifier *source, DNotifier *target);

//! Filter update function without masking capabilities.
void filter_update(DFilter *f, dtag_t tag, dval_t val);

//! Filter update function with masking capabilities.
void filter_mask_update(DFilter *f, dtag_t tag, dval_t val);

//! Notifier init
void notifier_init(DNotifier *n);


/*!
 * Macro to notify the target object.
 */
INLINE void dnotify(DNotifier *target, dtag_t tag, dval_t val)
{
	if (target)
		target->update(target, tag, val);
}

/*!
 * Macro to notify all the targets of \a target object.
 */
INLINE void dnotify_targets(DNotifier *target, dtag_t tag, dval_t val)
{
	DFilter *f;
	if (!ISLISTEMPTY(&target->targets))
		FOREACHNODE(f, &target->targets)
			f->update(f, tag, val);
}


/*!
 * Macro that connect \a src notifier to \a tgt using \a map and passing \a opt for filtering option.
 * It declares a static filter to achieve connection and messages translation.
 * \note Due its static filter declaration, DCONNECT MUST NOT be used inside loops or in functions called multiple times.
 * Failing to do so will lead to unpredictable connections between notifiers.
 */
#define DCONNECT(src, tgt, map, opt) \
	do { \
		static DFilter _filter_; /* Declare a filter */ \
		filter_init(&(_filter_), map, opt, src, tgt); /* Init it. */ \
	} while (0)


#endif /* DT_DNOTIFIER_H */

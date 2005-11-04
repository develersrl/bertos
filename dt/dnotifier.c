/*!
 * \file
 * <!--
 * Copyright 2005 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 * \version $Id$
 *
 * \brief Notifier obj (implementation).
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
 *#* Revision 1.5  2005/06/09 13:49:22  batt
 *#* Reformat; correct some comments.
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

#include <cfg/debug.h>

#include <dt/dtag.h>
#include <dt/dnotifier.h>
#include <mware/list.h>

/*!
 * Default update used to notify target: notify all trasparently all
 * targets in the list.
 */
static void notifier_update(DNotifier *n, dtag_t tag, dval_t val)
{
	dnotify_targets(n, tag, val);
}

/*!
 * Init.
 */
void notifier_init(DNotifier *n)
{
	// Init instance
	n->update = notifier_update;
	LIST_INIT(&n->targets);
}

/*!
 * Search in the map a tag and a val corresponding to the ones supplied.
 * If a match is found change them to the corresponding ones in the map.
 * If map is NULL the filter is trasparent and all messages sent to filter
 * will be forwarded to its target.
 */
void filter_update(DFilter *f, dtag_t tag, dval_t val)
{

	const DFilterMap *map = f->map;

	if (map)
	{
		while (map->src.tag != TAG_END)
		{
			if ((map->src.tag == tag) && (map->src.val == val))
			{
				tag = map->dst.tag;
				val = map->dst.val;
				break;
			}
			/* TAG_ANY matches anything */
			if (map->src.tag == TAG_ANY)
				break;
			map++;
		}

		if (map->src.tag != TAG_END)
			dnotify(f->target, tag, val);
	}
	else
		dnotify(f->target, tag, val);
}


/*!
 * Search in the table a tag corresponding to the one supplied and a val
 * that has at least the mask map supplied bits to one.
 * If a match is found change them to the corresponding ones in the map.
 * If map is NULL the filter is trasparent and all messages sent to filter
 * will be forwarded to its target.
 */
void filter_mask_update(DFilter *f, dtag_t tag, dval_t val)
{

	const DFilterMap *map = f->map;
	dfilter_mask_t mask;

	if (map)
	{
		while (map->src.tag != TAG_END)
		{
			mask = (dfilter_mask_t) map->src.val;
			if ((map->src.tag == tag) && ((mask & (dfilter_mask_t)val) == mask))
			{
				tag = map->dst.tag;
				val = map->dst.val;
				break;
			}
			/* TAG_ANY matches anything */
			if (map->src.tag == TAG_ANY)
				break;
			map++;
		}


		if (map->src.tag != TAG_END)
			dnotify(f->target, tag, val);
	}
	else
		dnotify(f->target, tag, val);
}


#define FILTER_MAGIC_ACTIVE 0xAA
/*!
 * Init filter.
 * If \a masked is true, all the fields value in \a map must be interpreted as a mask of bits.
 */
void filter_init(DFilter *f, const DFilterMap *map, bool masked, DNotifier *source, DNotifier *target)
{
	// Init instance
	if (masked)
		f->update = (update_filter_ptr)filter_mask_update;
	else
		f->update = (update_filter_ptr)filter_update;

	/* set filter map and target */
	f->map = map;
	f->target = target;

	/* these ensure that the filter is not inserted in more than one list */
	ASSERT(f->magic != FILTER_MAGIC_ACTIVE);
	DB(f->magic = FILTER_MAGIC_ACTIVE;)

	/* Add the filter to source filter list */
	ADDTAIL(&source->targets, &f->link);
}

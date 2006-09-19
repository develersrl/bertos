#ifndef MWARE_RESOURCE_H
#define MWARE_RESOURCE_H

#include <drv/timer.h> // time_t
#include <kern/sem.h>

/*
 * Abstract locking primitives used by host OS.
 */
#if CONFIG_KERNEL

	typedef Semaphore ResourceLock;
	#define ResMan_sleep()         timer_delay(1)
	#define ResMan_time_t          mtime_t

#else /* FreeRTOS */

	#include <freertos.h>
	#include <semphr.h>
	#include <task.h> // vTaskDelay()

	#define ResMan_sleep()        vTaskDelay((portTickType)1 * portTICK_RATE_MS)
	#define ResMan_time_t         portTickType
#endif


// Forward decl
struct Observer;

/**
 * Hold context information for a resource such as an audio channel.
 *
 * Each driver registers one or more Resource instances with the
 * ResMan using ResMan_Register().
 *
 * Clients can then allocate the resource through ResMan_Alloc()
 * providing a desired priority and an Observer for asynchronous
 * notification.
 *
 * Allocated resources can be stolen by other clients asking for a
 * higher priority.  ResMan notifies a preemption request by invoking
 * the Observer of the current owner.
 *
 * The Observer callback must take whatever action is needed to
 * release the resource as soon as possible to avoid blocking the
 * new owner.
 */
typedef struct Resource
{
//Private
	/// Control access to fields below.
	Semaphore lock;

	/// Pointer to current owner's observer.  NULL if resource is free.
	struct Observer *owner;

	/// Priority of current owner (higher values mean higher priority).
	int pri;

	/// Queue of processes waiting to obtain the resource.
	List queue;
} Resource;

/// Event sent by ResMan to owners when to request resource release.
enum { EVENT_RELEASE = 1 };

/// Try to allocate a resource \a res with priority \a pri for at most \a timeout ticks.
bool ResMan_Alloc(Resource *res, int pri, ResMan_time_t timeout, struct Observer *releaseRequest);

/// Free resource \a res.  Will eventually wake-up other queued owners.
void ResMan_Free(Resource *res);

void ResMan_Init(Resource *res);

#endif /* MWARE_RESOURCE_H */

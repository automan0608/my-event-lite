
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef _USE_SELECT
#include "prefix_event_op_select.h"
#endif

#include "prefix_event_op.h"
#include "prefix_log.h"
#include "prefix_event.h"
#include "prefix_pipe.h"
#include "prefix_min_heap.h"
#include "prefix_base.h"

#include "prefix_event_base.h"

prefix_event_base_t *prefix_event_base_new()
{
	prefix_event_base_t *base;

	base = (prefix_event_base_t *)prefix_malloc(sizeof(prefix_event_base_t));
	if (NULL == base)
	{
		prefix_log("error", "malloc event base error!");
		return NULL;
	}
	memset(base, 0, sizeof(prefix_event_base_t));

#ifdef _USE_SELECT
	base->eventOps = &selectOps;
#endif

	int result = prefix_pipe_init(base->notifyFd);
	if (SUCCESS != result)
	{
		prefix_event_base_free(base);
		prefix_log("error", "create pipe error");
		return NULL;
	}

	base->timeHeap = prefix_min_heap_init();
	if (NULL == base->timeHeap)
	{
		prefix_event_base_free(base);
		prefix_log("error", "create min heap error");
		return NULL;
	}

	return base;
}

int prefix_event_base_add_event(int type, prefix_event_t *event)
{
	if (!(type & (EVENT_TYPE_IO | EVENT_TYPE_SIG | EVENT_TYPE_TIME)))
	{
		prefix_log("error", "parameter error");
		return ERROR;
	}

	prefix_event_t **ptr = NULL;

	switch (type)
	{
	case EVENT_TYPE_IO:
		ptr = &event->base->eventIOHead;
		break;
	case EVENT_TYPE_SIG:
		ptr = &event->base->eventSigHead;
		break;
	case EVENT_TYPE_TIME:
		ptr = &event->base->eventTimeHead;
		break;
	default:
		prefix_log("error", "no such type");
		break;
	}

	if (NULL == *ptr)
	{
		*ptr = event;
	}
	else
	{
		while (NULL != (*ptr)->next)
		{
			ptr = &(*ptr)->next;
		}
		(*ptr)->next = event;
//		event->prev = (*ptr)->next;
		event->prev = *ptr;
		event->next = NULL;

	}
	return SUCCESS;
}

int prefix_event_base_set_event_active(prefix_event_base_t *base, prefix_event_t *event)
{
	if (NULL == base || NULL == event)
	{
		prefix_log("error", "parameter error");
		return ERROR;
	}

	prefix_event_t **ptr = base->eventActive;

	if (NULL == *ptr)
	{
		*ptr = event;
	}
	else
	{
		while (NULL != (*ptr)->activeNext)
		{
			ptr = &(*ptr)->activeNext;
		}
		(*ptr)->activeNext = event;
		event->activePrev = (*ptr);
		event->activeNext = NULL;
	}

	event->eventStatus |= EVENT_STATUS_ACTIVE;

	return SUCCESS;
}

int prefix_event_base_dispatch(prefix_event_base_t *base)
{
	if (NULL == base)
	{
		prefix_log("error", "parameter error");
		return ERROR;
	}

	if (NULL == base->eventIOHead
		&& NULL == base->eventSigHead
		&& NULL == base->eventTimeHead)
	{
		prefix_log("error", "no event registed");
		return ERROR;
	}

	prefix_event_t *ptr;
	int flag = 0;
	int result = 0;
	struct timeval tvReactor = {0, 0};

	struct timeval tvNow = {0, 0};
	struct timeval tvMinHeapPut = {0, 0};
	struct timeval *tvMinHeapGet = NULL;

	// not all events are consumered
	while (base->eventIOHead || base->eventSigHead || base->eventTimeHead)
	{
		// add io events to reactor
		if (NULL != base->eventIOHead)
		{
				ptr = base->eventIOHead;

				while(ptr)
				{
					// add the events with timeout to the min heap
					// should check whether it has been added.
					if (!(0 == ptr->ev.io.timeout.tv_sec && 0 == ptr->ev.io.timeout.tv_usec)
						&& 0 == (ptr->eventStatus|EVENT_STATUS_IN_MIN_HEAP))
					{
						gettimeofday(&tvNow, NULL);
						tvMinHeapPut.tv_sec = tvNow.tv_sec + ptr->ev.io.timeout.tv_sec;
						tvMinHeapPut.tv_usec = tvNow.tv_usec + ptr->ev.io.timeout.tv_usec;

						result = prefix_min_heap_push(base->timeHeap, tvMinHeapPut, ptr);
						if (SUCCESS != result)
						{
							prefix_log("error", "min heap push error");
							return  ERROR;
						}

						// already set in prefix_min_heap_push
						// ptr->eventStatus |= EVENT_STATUS_IN_MIN_HEAP;
					}

					flag = base->eventOps->add(base, ptr->ev.io.fd, 0, ptr->ev.io.events, NULL);
					if (0 != flag)
					{
						//TODO
					}

					ptr = ptr->next;
				}
    	}

		// add sig events to reactor
		if (NULL != base->eventSigHead)
		{
			flag = base->eventOps->add(base, base->notifyFd[0], 0, PREFIX_EV_READ, NULL);
			if (0 != flag)
			{
				//TODO
			}
		}

		// add time events to reactor
		if (NULL != base->eventTimeHead)
		{
			ptr = base->eventTimeHead;

			while(ptr)
			{
				if (!(0 == ptr->ev.time.timeout.tv_sec && 0 == ptr->ev.time.timeout.tv_usec)
					&& 0 == (ptr->eventStatus|EVENT_STATUS_IN_MIN_HEAP))
				{
					gettimeofday(&tvNow, NULL);
					tvMinHeapPut.tv_sec = tvNow.tv_sec + ptr->ev.time.timeout.tv_sec;
					tvMinHeapPut.tv_usec = tvNow.tv_usec + ptr->ev.time.timeout.tv_usec;

					result = prefix_min_heap_push(base->timeHeap, tvMinHeapPut, ptr);
					if (SUCCESS != result)
					{
						prefix_log("error", "min heap push error");
						return  ERROR;
					}

					ptr->eventStatus |= EVENT_STATUS_IN_MIN_HEAP;
				}
			}
		}

		// set the timeout of the reactor
		// tvMinHeapGet is a pointer
		tvMinHeapGet = prefix_min_heap_get_top(base->timeHeap);
		gettimeofday(&tvNow, NULL);
		tvReactor.tv_sec = tvMinHeapGet->tv_sec - tvNow.tv_usec;
		tvReactor.tv_usec = tvMinHeapGet->tv_usec - tvNow.tv_usec;

		result = base->eventOps->dispatch(base, &tvReactor);
		if (0 != flag)
		{
			//TODO
		}

		// invoke the callbacks
		ptr = base->eventActive;
		while (ptr)
		{
			// eventStatus will be set in the function
			prefix_event_invoke(ptr);

			ptr = ptr->activeNext;
		}

		// clean the active chain
		// only need to clean the head,
		// since the chain nodes will be reset when add to the chain again.
		base->eventActive = NULL;

		// reset all status
    }

	return SUCCESS;
}

// free all events and the event_base
void prefix_event_base_free(prefix_event_base_t *base)
{
	if (NULL == base)
	{
		prefix_log("debug", "already freed");
		return;
	}

	prefix_event_t *ptr;

	while (NULL != base->eventIOHead)
	{
		ptr = base->eventIOHead;
		base->eventIOHead = base->eventIOHead->next;
		prefix_event_free(ptr);
	}

	while (NULL != base->eventSigHead)
	{
		ptr = base->eventSigHead;
		base->eventSigHead = base->eventSigHead->next;
		prefix_event_free(ptr);
	}

	while (NULL != base->eventTimeHead)
	{
		ptr = base->eventTimeHead;
		base->eventTimeHead = base->eventTimeHead->next;
		prefix_event_free(ptr);
	}

	prefix_min_heap_free(base->timeHeap);

	// no need to free eventActive since it's all from the basic events

	prefix_free(base);
}

void prefix_event_base_dump(prefix_event_base_t *base)
{
	if (NULL == base)
	{
		prefix_log("error", "parameter error!");
		return;
	}

	prefix_event_t *ptr;

	printf("********************************************\n");
	printf("************* event base dump **************\n");
	printf("********************************************\n");
	printf("   base:      %p                                        \n", base);
	printf("         eventOps:        %p                 \n", base->eventOps);
	printf("             op->name:        %s                 \n", base->eventOps->name);
	printf("         eventIOHead:     %p                 \n", base->eventIOHead);
	for (ptr = base->eventIOHead; ptr != NULL; ptr = ptr->next)
	{
	printf("             event:           %p             \n", ptr);
	}
	printf("         eventSigHead:    %p                 \n", base->eventSigHead);
	for (ptr = base->eventSigHead; ptr != NULL; ptr = ptr->next)
	{
	printf("             event:           %p             \n", ptr);
	}
	printf("         eventTimeHead:   %p                 \n", base->eventTimeHead);
	for (ptr = base->eventTimeHead; ptr != NULL; ptr = ptr->next)
	{
	printf("              event:          %p             \n", ptr);
	}
	printf("         timeHeap:        %p                 \n", base->timeHeap);
	printf("         eventActive:     %p                 \n", base->eventActive);
	printf("*********************************************\n");
}

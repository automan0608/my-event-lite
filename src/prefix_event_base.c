
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>

#ifdef _USE_SELECT
#include "prefix_event_op_select.h"
#endif

#include "prefix_event_op.h"
#include "prefix_log.h"
#include "prefix_event.h"
#include "prefix_bufferevent.h"
#include "prefix_event_signal.h"
#include "prefix_pipe.h"
#include "prefix_min_heap.h"
#include "prefix_base.h"

#include "prefix_event_base.h"

prefix_event_base_t *prefix_event_base_new()
{
	prefix_log("debug", "in");

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
	prefix_log("debug", "create pipe success, [%d:%d]",
					base->notifyFd[0], base->notifyFd[1]);

	base->timeHeap = prefix_min_heap_init();
	if (NULL == base->timeHeap)
	{
		prefix_event_base_free(base);
		prefix_log("error", "create min heap error");
		return NULL;
	}

	prefix_log("debug", "out");
	return base;
}

int prefix_event_base_add_event_use_thread(prefix_event_t *event)
{
	prefix_log("debug", "in");

	if (NULL == event)
	{
		prefix_log("error", "parameter error");
		return ERROR;
	}

	int result = 0;
	char buf[1] = {0};

	buf[0] = NOTIFYTYPE_EVENT_NEW;

	// write the type of this notify
	result = prefix_pipe_write(event->base->notifyFd[1], buf, 1);
	if (SUCCESS != result)
	{
		prefix_log("error", "write notify event new error");
		return ERROR;
	}

	result = prefix_pipe_write(event->base->notifyFd[1], (void *)&event, sizeof(prefix_event_t *));
	if (SUCCESS != result)
	{
		prefix_log("error", "notifyfd write error");
		return ERROR;
	}

	prefix_log("debug", "notifyfd write success");

	return SUCCESS;
}

int prefix_event_base_add_event(prefix_event_t *event)
{
	prefix_log("debug", "in");

	if (NULL == event)
	{
		prefix_log("error", "parameter error");
		return ERROR;
	}

	int type = event->eventType;
	prefix_event_t **ptr = NULL;

	if (!(type & (EVENT_TYPE_IO | EVENT_TYPE_SIG | EVENT_TYPE_TIME)))
	{
		prefix_log("error", "parameter error");
		return ERROR;
	}

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
		event->prev = *ptr;
		event->next = NULL;
	}

	prefix_log("debug", "out");
	return SUCCESS;
}

int prefix_event_base_add_bufferevent(prefix_bufferevent_t *event)
{
	prefix_log("debug", "in");

	if (NULL == event)
	{
		prefix_log("error", "parameter error");
		return ERROR;
	}

	prefix_bufferevent_t **ptr = NULL;
	ptr = &event->base->buffereventHead;

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
		event->prev = *ptr;
		event->next = NULL;
	}

	prefix_log("debug", "out");
	return SUCCESS;
}

int prefix_event_base_add_bufferevent_use_thread(prefix_bufferevent_t *event)
{
	prefix_log("debug", "in");

	if (NULL == event)
	{
		prefix_log("error", "parameter error");
		return ERROR;
	}

	int result = 0;
	char buf[1] = {0};

	buf[0] = NOTIFYTYPE_BUFFEREVENT_NEW;

	// write the type of this notify
	result = prefix_pipe_write(event->base->notifyFd[1], buf, 1);
	if (SUCCESS != result)
	{
		prefix_log("error", "write notify bufferevent new error");
		return ERROR;
	}

	result = prefix_pipe_write(event->base->notifyFd[1], (void *)&event, sizeof(prefix_bufferevent_t *));
	if (SUCCESS != result)
	{
		prefix_log("error", "notifyfd write error");
		return ERROR;
	}

	prefix_log("debug", "notifyfd write bufferevent new success");

	return SUCCESS;
}

int prefix_event_base_set_event_active(prefix_event_base_t *base, prefix_event_t *event)
{
	prefix_log("debug", "in");

	if (NULL == base || NULL == event)
	{
		prefix_log("error", "parameter error");
		return ERROR;
	}

	prefix_event_t **ptr = &base->eventActive;

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

	prefix_log("debug", "out");
	return SUCCESS;
}

int prefix_event_base_set_bufferevent_active(prefix_event_base_t *base, prefix_bufferevent_t *event)
{
	prefix_log("debug", "in");

	if (NULL == base || NULL == event)
	{
		prefix_log("error", "parameter error");
		return ERROR;
	}

	prefix_bufferevent_t **ptr = &base->buffereventActive;

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

	prefix_log("debug", "out");
	return SUCCESS;
}

int prefix_event_base_dispatch(prefix_event_base_t *base)
{
	prefix_log("debug", "in");

//	prefix_event_base_dump(base);

	int firstLoop = 1;

	if (NULL == base)
	{
		prefix_log("error", "parameter error");
		return ERROR;
	}

	if (NULL == base->eventIOHead
		&& NULL == base->eventSigHead
		&& NULL == base->eventTimeHead
		&& 1 != firstLoop)
	{
			prefix_log("error", "no event registed");
		return ERROR;
	}

	prefix_event_t *ptr;
	prefix_bufferevent_t *ptrbuf;
	int result = 0;
	struct timeval tvReactor = {0, 0};

	struct timeval tvNow = {0, 0};
	struct timeval tvMinHeapPut = {0, 0};
	struct timeval *tvMinHeapGet = NULL;

	// not all events are consumered
	while (base->eventIOHead
			|| base->eventSigHead
			|| base->eventTimeHead
			|| 1 == firstLoop)
	{
		prefix_log("debug", "in dispatch while loop");
//		prefix_event_base_dump(base);
		firstLoop = 0;

		// init event base reactor
		base->eventOps->init(base);

		// add io events to reactor
		if (NULL != base->eventIOHead)
		{
			prefix_log("debug", "base io event chain not NULL");

			for(ptr=base->eventIOHead;ptr;ptr=ptr->next)
			{
				// add the events with timeout to the min heap
				// should check whether it has been added.
				if (!(0 == ptr->ev.io.timeout.tv_sec && 0 == ptr->ev.io.timeout.tv_usec)
					&& 0 == (ptr->eventStatus & EVENT_STATUS_IN_MIN_HEAP))
				{
					gettimeofday(&tvNow, NULL);
					tvMinHeapPut.tv_sec = tvNow.tv_sec + ptr->ev.io.timeout.tv_sec;
					tvMinHeapPut.tv_usec = tvNow.tv_usec + ptr->ev.io.timeout.tv_usec;

					// will set eventStatus
					result = prefix_min_heap_push(base->timeHeap, tvMinHeapPut, ptr);
					if (SUCCESS != result)
					{
						prefix_log("error", "min heap push error");
						return  ERROR;
					}

					// already set in prefix_min_heap_push
					// ptr->eventStatus |= EVENT_STATUS_IN_MIN_HEAP;
				}

				result = base->eventOps->add(base, ptr->ev.io.fd, 0, ptr->ev.io.events, NULL);
				if (0 != result)
				{
					//TODO
				}
			}
		}

		// add sig events to reactor
		if (NULL != base->eventSigHead || 0 != base->useThread)
		{
			prefix_log("debug", "base sig event chain not NULL");

			result = base->eventOps->add(base, base->notifyFd[0], 0, EV_READ, NULL);
			if (0 != result)
			{
				//TODO
			}
		}

		// add time events to reactor
		if (NULL != base->eventTimeHead)
		{
			prefix_log("debug", "base time event chain not NULL");

			for(ptr=base->eventTimeHead;ptr;ptr=ptr->next)
			{
				prefix_log("debug", "ptr not NULL");

//				if (!(0 == ptr->ev.time.timeout.tv_sec || 0 == ptr->ev.time.timeout.tv_usec)
				if ((0 != ptr->ev.time.timeout.tv_sec || 0 != ptr->ev.time.timeout.tv_usec)
					&& (0 == (ptr->eventStatus & EVENT_STATUS_IN_MIN_HEAP)))
				{
					prefix_log("debug", "will push min heap");

					gettimeofday(&tvNow, NULL);
					tvMinHeapPut.tv_sec = tvNow.tv_sec + ptr->ev.time.timeout.tv_sec;
					tvMinHeapPut.tv_usec = tvNow.tv_usec + ptr->ev.time.timeout.tv_usec;

					// will set eventStatus
					result = prefix_min_heap_push(base->timeHeap, tvMinHeapPut, ptr);
					if (SUCCESS != result)
					{
						prefix_log("error", "min heap push error");
						return  ERROR;
					}

					// ptr->eventStatus |= EVENT_STATUS_IN_MIN_HEAP;
				}
			}
		}

		// add bufferevent events to reactor
		if (NULL != base->buffereventHead)
		{
			prefix_log("debug", "base buffer event chain not NULL");

			for(ptrbuf=base->buffereventHead; ptrbuf; ptrbuf=ptrbuf->next)
			{
				prefix_log("debug", "ptr not NULL");

				// always add read events
				result = base->eventOps->add(base, ptrbuf->fd, 0, EV_READ, NULL);

				// not always add write events
				// flush type == CHAR
				if (BUFFEREVENT_FLUSHTYPE_CHAR == ptrbuf->attr.flushType
							&& 0 != ptrbuf->output->blockNum)
				{
					result = base->eventOps->add(base, ptrbuf->fd, 0, EV_WRITE, NULL);
				}
				// flush type == BLOCK
				else if (BUFFEREVENT_FLUSHTYPE_BLOCK == ptrbuf->attr.flushType
							&& ptrbuf->output->blockHead
							&& ptrbuf->output->blockSize == ptrbuf->output->blockHead->ptrTail)
				{
					result = base->eventOps->add(base, ptrbuf->fd, 0, EV_WRITE, NULL);
				}
				// flush type == LINE
				// set as the same with flusy type == CHAR
				else if (BUFFEREVENT_FLUSHTYPE_LINE == ptrbuf->attr.flushType
							&& 0 != ptrbuf->output->blockNum)
				{
					result = base->eventOps->add(base, ptrbuf->fd, 0, EV_WRITE, NULL);
				}

			}
		}

		// for test
		prefix_log("debug", "set the timeout of the reactor");
//		prefix_min_heap_dump(base->timeHeap);

		// set the timeout of the reactor
		// tvMinHeapGet is a pointer
		// in case some event has already timeout, need a while loop
		do {
			tvMinHeapGet = prefix_min_heap_get_top(base->timeHeap);
			if (NULL == tvMinHeapGet)
			{
				prefix_log("debug", "min heap empty");
				// default reactor timeout,
				// also as the firstloop timeout
				tvReactor.tv_sec = 5;
				tvReactor.tv_usec = 0;
			}
			else
			{

				gettimeofday(&tvNow, NULL);
				tvReactor.tv_sec = tvMinHeapGet->tv_sec - tvNow.tv_sec;
				tvReactor.tv_usec = tvMinHeapGet->tv_usec - tvNow.tv_usec;
				if (0 > tvReactor.tv_usec)
				{
					tvReactor.tv_sec --;
					tvReactor.tv_usec += 1000000;
				}
				if (0 > tvReactor.tv_sec)
				{
			        prefix_event_set_active(prefix_min_heap_pop(base->timeHeap), EVENT_ACTIVETYPE_TIMEOUT);
				}
			}
		} while (0 > tvReactor.tv_sec);

		// for test
		prefix_log("debug", "reactor dispatching, tv: %d.%d",
							(int)tvReactor.tv_sec, (int)tvReactor.tv_usec);

		// will set eventStatus in pop func
		result = base->eventOps->dispatch(base, &tvReactor);
		// for test
		prefix_log("debug", "dispatch result:%d", result);
		//prefix_event_base_dump(base);
		if (SUCCESS != result)
		{
			//TODO
		}

		// for test
		prefix_log("debug", "invoke the callback");
//		prefix_event_base_dump(base);

		// invoke the callbacks
		for(ptr=base->eventActive; ptr; ptr=ptr->activeNext)
		{
			// eventStatus will be set in the function
			prefix_event_invoke(ptr);
		}

		// invoke the callbacks
		for(ptrbuf=base->buffereventActive; ptrbuf; ptrbuf=ptrbuf->activeNext)
		{
			// eventStatus will be set in the function
			prefix_bufferevent_invoke(ptrbuf);
		}

		// for test
		prefix_log("debug", "clean the active chain");

		// clean the active chain
		// only need to clean the head,
		// since the chain nodes will be reset when add to the chain again.
		base->eventActive = NULL;
		base->buffereventActive = NULL;

		// for test
		// prefix_event_base_dump(base);

		// clean the event chains (io)
		// delete the useless events & reset all eventStatus
		// CANNOT MODIFY to for-loop
		ptr = base->eventIOHead;
		while (ptr)
		{
			if ((!(ptr->ev.io.events & EV_PERSIST) && (ptr->eventStatus & EVENT_STATUS_INVOKED))
				|| (ptr->eventStatus & EVENT_STATUS_FREED))
			{
				prefix_event_t *ptrtmp =ptr;
				ptr = ptr->next;
				result = prefix_event_delete(ptrtmp);
				if (SUCCESS != result)
				{
					//TODO
				}

				// for test
				prefix_log("debug", "one io event removed");
			}
			else
			{
				ptr->eventStatus &= ~EVENT_STATUS_ACTIVE;
				ptr->eventStatus &= ~EVENT_STATUS_INVOKED;
				ptr = ptr->next;
			}

			// for test
			//prefix_event_base_dump(base);
		}

		// clean the event chains (sig)
		// delete the useless events & reset all eventStatus
		// CANNOT MODIFY to for-loop
		ptr = base->eventSigHead;
		while (ptr)
		{
			if ((!(ptr->ev.sig.events & EV_PERSIST) && (ptr->eventStatus & EVENT_STATUS_INVOKED))
				|| (ptr->eventStatus & EVENT_STATUS_FREED))
			{
				prefix_event_t *ptrtmp =ptr;
				ptr = ptr->next;
				result = prefix_event_delete(ptrtmp);
				if (SUCCESS != result)
				{
					//TODO
				}

				// for test
				prefix_log("debug", "one sig event removed");
			}
			else
			{
				ptr->eventStatus &= ~EVENT_STATUS_ACTIVE;
				ptr->eventStatus &= ~EVENT_STATUS_INVOKED;
				ptr = ptr->next;
			}

			// for test
			// prefix_event_base_dump(base);
		}

		// clean the event chains (time)
		// delete the useless events & reset all eventStatus
		// CANNOT MODIFY to for-loop
		ptr = base->eventTimeHead;
		while (ptr)
		{
			if ((!(ptr->ev.time.events & EV_PERSIST) && (ptr->eventStatus & EVENT_STATUS_INVOKED))
				|| (ptr->eventStatus & EVENT_STATUS_FREED))
			{
				prefix_event_t *ptrtmp =ptr;
				ptr = ptr->next;
				result = prefix_event_delete(ptrtmp);
				if (SUCCESS != result)
				{
					//TODO
				}

				// for test
				prefix_log("debug", "one time event removed");
			}
			else
			{
				ptr->eventStatus &= ~EVENT_STATUS_ACTIVE;
				ptr->eventStatus &= ~EVENT_STATUS_INVOKED;
				ptr = ptr->next;
			}

			// for test
			// prefix_event_base_dump(base);
		}

		// clean the event chains (time)
		// delete the useless events & reset all eventStatus
		// CANNOT MODIFY to for-loop
		ptrbuf = base->buffereventHead;
		while (ptrbuf)
		{
			if ((!(ptrbuf->events & EV_PERSIST) && (ptrbuf->eventStatus & EVENT_STATUS_INVOKED))
				|| (ptrbuf->eventStatus & EVENT_STATUS_FREED))
			{
				prefix_bufferevent_t *ptrtmp =ptrbuf;
				ptrbuf = ptrbuf->next;
				result = prefix_bufferevent_delete(ptrtmp);
				if (SUCCESS != result)
				{
					//TODO
				}

				// for test
				prefix_log("debug", "one buffer event removed");
			}
			else
			{
				ptrbuf->eventStatus &= ~EVENT_STATUS_ACTIVE;
				ptrbuf->eventStatus &= ~EVENT_STATUS_INVOKED;
				ptrbuf = ptrbuf->next;
			}

			// for test
			// prefix_event_base_dump(base);
		}

		prefix_log("debug", "at the tail of base dispatch");
//		prefix_event_base_dump(base);
    }

	// for test
	prefix_log("debug", "dispatch loop exit");

	return SUCCESS;
}

// remove the event from the event_base event chain
int prefix_event_base_remove_event(prefix_event_base_t *base, prefix_event_t *event)
{
	prefix_log("debug", "in");

	if (NULL == base || NULL == event)
	{
		prefix_log("error", "parameter error");
		return ERROR;
	}

	prefix_event_t **ptr = NULL;
	switch (event->eventType)
	{
	case EVENT_TYPE_IO:
		ptr = &base->eventIOHead;
		break;
	case EVENT_TYPE_SIG:
		ptr = &base->eventSigHead;
		break;
	case EVENT_TYPE_TIME:
		ptr = &base->eventTimeHead;
		break;
	default:
		prefix_log("debug", "no such event type");
		return ERROR;
	}

	if (*ptr == event)
	{
		*ptr = event->next;
	}
	else
	{
		event->prev->next = event->next;
		if (NULL != event->next)
		{
			event->next->prev = event->prev;
		}
	}

	event->prev = NULL;
	event->next = NULL;

	prefix_log("debug", "out");
	return SUCCESS;
}

// remove the bufferevent from the event_base event chain
int prefix_event_base_remove_bufferevent(prefix_event_base_t *base, prefix_bufferevent_t *event)
{
	prefix_log("debug", "in");

	if (NULL == base || NULL == event)
	{
		prefix_log("error", "parameter error");
		return ERROR;
	}

	prefix_bufferevent_t **ptr = NULL;
	ptr = &base->buffereventHead;

	if (*ptr == event)
	{
		*ptr = event->next;
	}
	else
	{
		event->prev->next = event->next;
		if (NULL != event->next)
		{
			event->next->prev = event->prev;
		}
	}

	event->prev = NULL;
	event->next = NULL;

	prefix_log("debug", "out");
	return SUCCESS;
}

// free all events and the event_base
void prefix_event_base_free(prefix_event_base_t *base)
{
	prefix_log("debug", "in");

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
		prefix_event_free_inner(ptr);
	}

	while (NULL != base->eventSigHead)
	{
		ptr = base->eventSigHead;
		base->eventSigHead = base->eventSigHead->next;
		prefix_event_free_inner(ptr);
	}

	while (NULL != base->eventTimeHead)
	{
		ptr = base->eventTimeHead;
		base->eventTimeHead = base->eventTimeHead->next;
		prefix_event_free_inner(ptr);
	}

	prefix_min_heap_free(base->timeHeap);

	// no need to free eventActive since it's all from the basic events

	prefix_free(base);

	prefix_log("debug", "out");
}

int prefix_event_base_use_thread(prefix_event_base_t *base)
{
	if (NULL == base)
	{
		prefix_log("error", "parameter error");
		return ERROR;
	}

	base->useThread = 1;

	return SUCCESS;
}

void prefix_event_base_dump(prefix_event_base_t *base)
{
	if (NULL == base)
	{
		prefix_log("error", "parameter error!");
		return;
	}

	prefix_event_t *ptr;
	prefix_bufferevent_t *ptrbuf;

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
	prefix_event_dump(ptr);
	}
	printf("         eventSigHead:    %p                 \n", base->eventSigHead);
	for (ptr = base->eventSigHead; ptr != NULL; ptr = ptr->next)
	{
	printf("             event:           %p             \n", ptr);
	prefix_event_dump(ptr);
	}
	printf("         eventTimeHead:   %p                 \n", base->eventTimeHead);
	for (ptr = base->eventTimeHead; ptr != NULL; ptr = ptr->next)
	{
	printf("              event:          %p             \n", ptr);
	prefix_event_dump(ptr);
	}
	printf("         buffereventHead: %p                 \n", base->buffereventHead);
	for (ptrbuf = base->buffereventHead; ptrbuf != NULL; ptrbuf = ptrbuf->next)
	{
	printf("              bufferevent:    %p             \n", ptrbuf);
	prefix_bufferevent_dump(ptrbuf);
	}
	printf("         timeHeap:        %p                 \n", base->timeHeap);
	printf("         eventActive:     %p                 \n", base->eventActive);
	for (ptr = base->eventActive; ptr != NULL; ptr = ptr->activeNext)
	{
	printf("             event:           %p             \n", ptr);
	prefix_event_dump(ptr);
	}
	printf("*********************************************\n");
}


#include <stdlib.h>
#include <string.h>

#include "prefix_log.h"
#include "prefix_event_base.h"
#include "prefix_event_signal.h"

#include "prefix_event.h"

prefix_event_t *prefix_event_new(prefix_event_base_t *base,
                                        prefix_socket_t fd,     short events,   const struct timeval *tv,
                                        void (*cb)(prefix_socket_t, short, void *),   void *arg)
{
	prefix_log("debug", "in");

	if (NULL == base)
	{
		prefix_log("error", "parameter error");
		return NULL;
	}

	if ( ((events & EV_SIG) && (events & EV_READ))
		|| ((events & EV_SIG) && (events & EV_WRITE))
		|| ((events & EV_TIME) && (events & EV_READ))
		|| ((events & EV_TIME) && (events & EV_WRITE))
		|| ((events & EV_SIG) && (events & EV_TIME))
		|| ((events & EV_READ) && (events & EV_WRITE)) )
	{
		prefix_log("error", "event conflicts");
		return NULL;
	}

	int result = 0;
	prefix_event_t *event = NULL;

	event = (prefix_event_t *)prefix_malloc(sizeof(prefix_event_t));
	if (NULL == event)
	{
		prefix_log("error", "malloc error");
		return NULL;
	}
	memset(event, 0, sizeof(prefix_event_t));

	event->base = base;
	event->callback = cb;
	event->arg = arg;

	if (events & EV_READ || events & EV_WRITE)
	{
		event->eventType = EVENT_TYPE_IO;
		event->ev.io.events = events;
		event->ev.io.fd = fd;
		if (NULL != tv)
		{
			event->ev.io.timeout.tv_sec = tv->tv_sec;
			event->ev.io.timeout.tv_usec = tv->tv_usec;
		}
		// else timeout.tv_sec = 0, timeout.tv_usec = 0
	}
	else if (events & EV_SIG)
	{
		event->eventType = EVENT_TYPE_SIG;
		event->ev.sig.events = events;
		event->ev.sig.signo = fd;
		result = prefix_event_signal_register(base, fd);
		if (SUCCESS != result)
		{
			prefix_log("error", "signal register error");
			return NULL;
		}
	}
	else if (events & EV_TIME)
	{
		event->eventType = EVENT_TYPE_TIME;
		event->ev.time.events = events;
		if (NULL != tv)
		{
			event->ev.time.timeout.tv_sec = tv->tv_sec;
			event->ev.time.timeout.tv_usec = tv->tv_usec;
		}
	}
	else
	{
		prefix_log("error", "switch events error");
		prefix_free(event);
		return NULL;
	}

	result = prefix_event_base_add_event(event->eventType, event);
	if (SUCCESS != result)
	{
		prefix_log("error", "add event to eventbase error");
		prefix_free(event);
		return NULL;
	}

	event->eventStatus = EVENT_STATUS_AVAIL;

	prefix_log("debug", "out");
	return event;
}

int prefix_event_set_active(prefix_event_t *event)
{
	prefix_log("debug", "in");

	if (NULL == event)
	{
		prefix_log("error", "parameter error");
		return ERROR;
	}

	int result = prefix_event_base_set_event_active(event->base, event);
	if (SUCCESS != result)
	{
		prefix_log("error", "set event active error");
		return ERROR;
	}

	prefix_log("debug", "out");
	return SUCCESS;
}

int prefix_event_invoke(prefix_event_t *event)
{
	prefix_log("debug", "in");

	if (NULL == event)
	{
		prefix_log("error", "parameter error");
		return ERROR;
	}

	// void (*cb)(prefix_socket_t fd, short event, void *arg);
	switch (event->eventType)
	{
	case EVENT_TYPE_IO:
		event->callback(event->ev.io.fd, CALLBACK_EVENT_GENERIC, event->arg);
		break;
	case EVENT_TYPE_SIG:
		event->callback(event->ev.sig.signo, CALLBACK_EVENT_GENERIC, event->arg);
		break;
	case EVENT_TYPE_TIME:
		event->callback(0, CALLBACK_EVENT_GENERIC, NULL);
		break;
	default:
		prefix_log("debug", "no such event type");
		return ERROR;
	}

	event->eventStatus |= EVENT_STATUS_INVOKED;

	prefix_log("debug", "out");
	return SUCCESS;
}

int prefix_event_delete(prefix_event_t *event)
{
	prefix_log("debug", "in");

	if (NULL == event)
	{
		prefix_log("error", "parameter error");
		return ERROR;
	}

	int result = prefix_event_base_remove_event(event->base, event);
	if (SUCCESS != result)
	{
		prefix_log("error", "remove event from base error");
		return ERROR;
	}

	// if is sig event, need to unregister
	if (event->eventType == EVENT_TYPE_SIG)
	{
		prefix_event_signal_unregister(event->ev.sig.signo);
	}

	prefix_event_free_inner(event);
	prefix_log("debug", "out");
	return SUCCESS;
}

void prefix_event_free(prefix_event_t *event)
{
	prefix_log("debug", "in");

	if (NULL == event)
	{
		prefix_log("debug", "already freed");
	}

	event->eventStatus |= EVENT_STATUS_FREED;

	prefix_log("debug", "out");
}

void prefix_event_free_inner(prefix_event_t *event)
{
	prefix_log("debug", "in");

	if (NULL == event)
	{
		prefix_log("debug", "already freed");
	}

	prefix_free(event);

	prefix_log("debug", "out");
}

void prefix_event_dump(prefix_event_t *event)
{
	if (NULL == event)
	{
		prefix_log("error", "parameter error");
		return;
	}

	printf("--------------------------------------------\n");
	printf("--------------- event dump -----------------\n");
	printf("--------------------------------------------\n");
	printf("   event: %p                              \n", event);
	printf("         prev:              %p            \n", event->prev);
	printf("         next:              %p            \n", event->next);
	printf("         activePrev:        %p            \n", event->activePrev);
	printf("         activeNext:        %p            \n", event->activeNext);
	printf("         base:              %p            \n", event->base);
	printf("         callback:          %p            \n", event->callback);
	printf("         arg:               %p            \n", event->arg);
	printf("         eventType:         %d            \n", event->eventType);
	printf("         eventStatus:       %d            \n", event->eventStatus);
	if (EVENT_TYPE_IO == event->eventType)
	{
	printf("         ev.io.events:          %d        \n", event->ev.io.events);
	printf("         ev.io.fd:              %d        \n", event->ev.io.fd);
	printf("         ev.io.timeout.sec:     %d        \n", (int)event->ev.io.timeout.tv_sec);
	printf("         ev.io.timeout.usec:    %d        \n", (int)event->ev.io.timeout.tv_usec);
	}
	else if (EVENT_TYPE_SIG == event->eventType)
	{
	printf("         ev.sig.events:         %d        \n", event->ev.sig.events);
	printf("         ev.sig.signo:          %d        \n", event->ev.sig.signo);
	}
	else if (EVENT_TYPE_TIME == event->eventType)
	{
	printf("         ev.time.events:        %d        \n", event->ev.time.events);
	printf("         ev.time.timeout.sec:   %d        \n", (int)event->ev.time.timeout.tv_sec);
	printf("         ev.time.timeout.usec:  %d        \n", (int)event->ev.time.timeout.tv_usec);
	}
	else
	{
	printf("             !!! eventType not found !!!!        \n");
	}
	printf("--------------------------------------------\n");
}

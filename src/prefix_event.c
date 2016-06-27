
#include <stdlib.h>
#include <string.h>

#include "prefix_log.h"
#include "prefix_event_base.h"

#include "prefix_event.h"

prefix_event_t *prefix_event_new(prefix_event_base_t *base,
                                        prefix_socket_t fd,     short events,   const struct timeval *tv,
                                        void (*cb)(void *),   void *arg)
{
	if (NULL == base)
	{
		prefix_log("error", "parameter error!");
		return NULL;
	}

	if ( ((events & PREFIX_EV_SIG) && (events & PREFIX_EV_READ))
		|| ((events & PREFIX_EV_SIG) && (events & PREFIX_EV_WRITE))
		|| ((events & PREFIX_EV_TIME) && (events & PREFIX_EV_READ))
		|| ((events & PREFIX_EV_TIME) && (events & PREFIX_EV_WRITE))
		|| ((events & PREFIX_EV_SIG) && (events & PREFIX_EV_TIME))
		|| ((events & PREFIX_EV_READ) && (events & PREFIX_EV_WRITE)) )
	{
		prefix_log("error", "event conflicts!");
		return NULL;
	}

	prefix_event_t *event = (prefix_event_t *)prefix_malloc(sizeof(prefix_event_t));
	if (NULL == event)
	{
		prefix_log("error", "malloc error");
		return NULL;
	}
	memset(event, 0, sizeof(prefix_event_t));

	event->base = base;
	event->callback = cb;
	event->arg = arg;

	if (events & PREFIX_EV_READ || events & PREFIX_EV_WRITE)
	{
		event->eventType = EVENT_TYPE_IO;
		event->ev.io.events = events;
		event->ev.io.fd = fd;
		if (NULL != tv)
		{
			event->ev.io.timeout.tv_sec = tv->tv_sec;
			event->ev.io.timeout.tv_usec = tv->tv_usec;
		}
	}
	else if (events & PREFIX_EV_SIG)
	{
		event->eventType = EVENT_TYPE_SIG;
		event->ev.sig.events = events;
		event->ev.sig.signo = fd;
	}
	else if (events & PREFIX_EV_TIME)
	{
		event->eventType = EVENT_TYPE_TIME;
		event->ev.time.events = events;
		if (NULL != tv)
		{
			event->ev.io.timeout.tv_sec = tv->tv_sec;
			event->ev.io.timeout.tv_usec = tv->tv_usec;
		}
	}
	else
	{
		prefix_log("error", "switch events error");
		prefix_free(event);
	}

	int result = prefix_event_base_add_event(event->eventType, event);
	if (SUCCESS != result)
	{
		prefix_log("error", "add event to eventbase error");
		return NULL;
	}

	return event;
}

void prefix_event_free(prefix_event_t *event)
{
	if (NULL == event)
	{
		prefix_log("debug", "already freed");
	}

	prefix_free(event);
}

void prefix_event_dump(prefix_event_t *event)
{
	if (NULL == event)
	{
		prefix_log("error", "parameter error");
		return;
	}

	printf("********************************************\n");
	printf("*************** event dump *****************\n");
	printf("********************************************\n");
	printf("   event: %p                              \n", event);
	printf("         prev:              %p            \n", event->prev);
	printf("         next:              %p            \n", event->next);
	printf("         base:              %p            \n", event->base);
	printf("         callback:          %p            \n", event->callback);
	printf("         arg:               %p            \n", event->arg);
	printf("         eventType:         %d            \n", event->eventType);
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
	printf("                 !!! eventType not found !!!!        \n");
	}
	printf("********************************************\n");
}

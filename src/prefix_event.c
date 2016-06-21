
#include <stdlib.h>

#include "prefix_log.h"
#include "prefix_event_base.h"

#include "prefix_event.h"

prefix_event_t *prefix_event_new(struct prefix_event_base *base,
                                        prefix_socket_t fd,     short events,   const struct timeval *tv,
                                        void (*cb)(prefix_socket_t,short,void *),   void *arg)
{
	if (NULL == base)
	{
		prefix_log("error", "parameter error!");
		return NULL;
	}

	if ((events & (PREFIX_EV_SIG | PREFIX_EV_READ))
		|| (events & (PREFIX_EV_SIG | PREFIX_EV_WRITE))
		|| (events & (PREFIX_EV_TIME | PREFIX_EV_READ))
		|| (events & (PREFIX_EV_TIME | PREFIX_EV_WRITE))
		|| (events & (PREFIX_EV_SIG | PREFIX_EV_TIME))
		|| (events & (PREFIX_EV_READ | PREFIX_EV_WRITE)) )
	{
		prefix_log("error", "event conflicts");
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

	switch (events)
	{
	case (events & PREFIX_EV_READ):
	case (events & PREFIX_EV_WRITE):
		event->eventType = EVENT_TYPE_IO;
		event->ev.io.events = events;
		event->ev.io.fd = fd;
		event->ev.io.timeout = tv;
		break;

	case (events & PREFIX_EV_SIG):
		event->eventType = EVENT_TYPE_SIG;
		event->ev.sig.events = events;
		event->ev.sig.signo = fd;
		break;

	case (events & PREFIX_EV_TIME):
		event->eventType = EVENT_TYPE_TIME;
		event->ev.time.events = events;
		event->ev.time.timeout = tv;
		break;
	default:
		prefix_log("error", "switch events error");
		prefix_free(event);
		break;
	}

	int result = prefix_event_base_add_event(event->eventType, event);
	if (SUCCESS != result)
	{
		prefix_log("error", "add event to eventbase error");
		return NULL;
	}
}

void prefix_event_free(struct prefix_event *ev)
{

}

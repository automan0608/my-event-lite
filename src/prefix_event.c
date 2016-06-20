
#include <stdlib.h>

#include "prefix_log.h"
#include "prefix_event_base.h"

#include "prefix_event.h"

struct prefix_event *prefix_event_new(struct prefix_event_base *base,
                                        prefix_socket_t fd,     short events,   const struct timeval *tv,
                                        void (*cb)(prefix_socket_t,short,void *),   void *arg)
{
	if (NULL == base)
	{
		prefix_log("parameter error!");
	}
}

void prefix_event_free(struct prefix_event *ev)
{

}

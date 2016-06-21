
#ifndef _PREFIX_EVENT_OP_H_
#define _PREFIX_EVENT_OP_H_

#include "prefix_event_core.h"

struct prefix_event_op_s
{
	const char *name;

	void *(*init)(prefix_event_base_t *);

	int (*add)(prefix_event_base_t *, prefix_socket_t fd, short old, short events, void *fdinfo);

	int (*del)(prefix_event_base_t *, prefix_socket_t fd, short old, short events, void *fdinfo);

	int (*dispatch)(prefix_event_base_t *, struct timeval *);
};

#endif

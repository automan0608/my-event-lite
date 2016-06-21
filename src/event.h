
#ifndef _EVENT_H_
#define _EVENT_H_

#include <sys/time.h>

#include "prefix_event_core.h"
/* event base related */
typedef struct prefix_event_base_s prefix_event_base_t;

prefix_event_base_t *prefix_event_base_new();

int prefix_event_base_dispatch(prefix_event_base_t *base);

void prefix_event_base_free(prefix_event_base_t *base);


/* event related */
typedef struct prefix_event_s prefix_event_t;

prefix_event_t *prefix_event_new(prefix_event_base_t *base,
                                        prefix_socket_t fd,
                                        short events,
                                        const struct timeval *tv,
                                        void (*cb)(prefix_socket_t, short, void *),
                                        void *arg);

void prefix_event_free(prefix_event_t *ev);

#endif

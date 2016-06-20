
#ifndef _EVENT_H_
#define _EVENT_H_

/* event base related */
struct prefix_event_base;

struct prefix_event_base *prefix_event_base_new();

int prefix_event_base_dispatch(struct prefix_event_base *base);

void prefix_event_base_free(struct prefix_event_base *base);


/* event related */
struct prefix_event;

struct prefix_event *prefix_event_new(struct prefix_event_base *base,
                                        prefix_socket_t fd,
                                        short events,
                                        const struct timeval *tv,
                                        void (*cb)(prefix_socket_t,short,void *),
                                        void *arg);

void prefix_event_free(struct prefix_event *ev);

#endif

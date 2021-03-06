
#ifndef _EVENT_H_
#define _EVENT_H_

#include <sys/time.h>
#include <stdlib.h>

#include "prefix_core.h"

/* event base related */
prefix_event_base_t *prefix_event_base_new();

int prefix_event_base_dispatch(prefix_event_base_t *base);

void prefix_event_base_free(prefix_event_base_t *base);

int prefix_event_base_use_thread(prefix_event_base_t *base);

/* event related */
prefix_event_t *prefix_event_new(prefix_event_base_t *base,
                                        prefix_socket_t fd,
                                        short events,
                                        const struct timeval *tv,
                                        void (*cb)(prefix_socket_t, short, void *),
//                                        void (*cb)(void *),
                                        void *arg);

void prefix_event_free(prefix_event_t *ev);

prefix_bufferevent_t *prefix_bufferevent_new(prefix_event_base_t *base, prefix_socket_t fd,
                                                short events, const struct timeval *tv,
                                                void (*cb)(prefix_socket_t, short, void *),
                                                void *arg, prefix_bufferevent_attr_t *attr);

int prefix_bufferevent_write(prefix_bufferevent_t *event, const char *buf, size_t len);

ssize_t prefix_bufferevent_read(prefix_bufferevent_t *event, void *buf, size_t len);

void prefix_bufferevent_free(prefix_bufferevent_t *event);

int prefix_bufferevent_attr_set_blocksize(prefix_bufferevent_attr_t *attr, int blockSize);

int prefix_bufferevent_attr_set_flushtype(prefix_bufferevent_attr_t *attr, int flushtype);

#endif

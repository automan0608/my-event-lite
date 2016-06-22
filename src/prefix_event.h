
#ifndef _PREFIX_EVENT_H_
#define _PREFIX_EVENT_H_

#include "prefix_event_core.h"

#define    PREFIX_EV_READ        0x01
#define    PREFIX_EV_WRITE       0x02
#define    PREFIX_EV_SIG           0x04
#define    PREFIX_EV_TIME         0x08
#define    PREFIX_EV_PERSIST    0x10

#define    EVENT_TYPE_IO          0x01
#define    EVENT_TYPE_SIG        0x02
#define    EVENT_TYPE_TIME      0x04

struct prefix_event_s
{
    struct prefix_event_s *prev;
    struct prefix_event_s *next;

    prefix_event_base_t *base;
    prefix_event_callback_t callback;
    void *arg;

    int eventType;              // EVENT_IO \   EVENT_SIG   \   EVENT_TIME

    union {
        struct {
            short   events;
            prefix_socket_t     fd;
            struct timeval      timeout;
        }io;

        struct {
            short   events;
            prefix_signal_t     signo;
        }sig;

        struct {
            short   events;
            struct timeval      timeout;
        }time;
    }ev;
};

prefix_event_t *prefix_event_new(prefix_event_base_t *base,
                                        prefix_socket_t fd, short events, const struct timeval *tv,
//                                        void (*cb)(prefix_socket_t, short, void *), void *arg);
                                        void (*cb)(void *), void *arg);

void prefix_event_free(prefix_event_t *ev);

#endif

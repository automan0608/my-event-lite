
#ifndef _PREFIX_EVENT_H_
#define _PREFIX_EVENT_H_

#include "prefix_core.h"

#if 0
#define    PREFIX_EV_READ         0x01
#define    PREFIX_EV_WRITE        0x02
#define    PREFIX_EV_SIG          0x04
#define    PREFIX_EV_TIME         0x08
#define    PREFIX_EV_PERSIST      0x10
#else
#define    EV_READ         0x01
#define    EV_WRITE        0x02
#define    EV_SIG          0x04
#define    EV_TIME         0x08
#define    EV_PERSIST      0x10
#endif

#define    EVENT_TYPE_IO          0x01
#define    EVENT_TYPE_SIG         0x02
#define    EVENT_TYPE_TIME        0x04

#define    EVENT_STATUS_AVAIL           0x01
#define    EVENT_STATUS_IN_MIN_HEAP     0x02
#define    EVENT_STATUS_SIG_INSTALLED   0x04
#define    EVENT_STATUS_ACTIVE          0x08
#define    EVENT_STATUS_INVOKED         0x10

struct prefix_event_s
{
    struct prefix_event_s *prev;
    struct prefix_event_s *next;

    struct prefix_event_s *activePrev;
    struct prefix_event_s *activeNext;

    prefix_event_base_t *base;
    prefix_event_callback_t callback;
    void *arg;

    int eventType;              // EVENT_TYPE_IO \   EVENT_TYPE_SIG   \   EVENT_TYPE_TIME

    int eventStatus;            // EVENT_STATUS_AVAIL \ EVENT_STATUS_IN_MIN_HEAP

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
                                        void (*cb)(prefix_socket_t, short, void *), void *arg);
//                                        void (*cb)(void *), void *arg);

int prefix_event_set_active(prefix_event_t *event);

int prefix_event_invoke(prefix_event_t *event);

int prefix_event_delete(prefix_event_t *event);

void prefix_event_free(prefix_event_t *event);

void prefix_event_dump(prefix_event_t *event);

#endif

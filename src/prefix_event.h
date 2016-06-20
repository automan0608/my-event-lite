
#ifndef _PREFIX_EVENT_H_
#define _PREFIX_EVENT_H_

typedef struct prefix_event_s prefix_event_t;
typedef void (*prefix_event_callback_t)(void *arg);
typedef prefix_socket_t int;

#define    EV_READ        0x01
#define    EV_WRITE      0x02
#define    EV_SIG           0x04
#define    EV_TIME        0x08
#define    EV_PERSIST    0x10

struct prefix_event_s
{
    struct prefix_event_s *prev;
    struct prefix_event_s *next;

    prefix_event_base_t *base;
    prefix_event_callback_t callback;

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

struct prefix_event *prefix_event_new(struct prefix_event_base *base,
                                        prefix_socket_t fd,     short events,   const struct timeval *tv,
                                        void (*cb)(prefix_socket_t,short,void *),   void *arg);

void prefix_event_free(struct prefix_event *ev);

#endif

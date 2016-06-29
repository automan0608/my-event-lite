
#ifndef _PREFIX_EVENT_CORE_H_
#define _PREFIX_EVENT_CORE_H_

typedef 	int        prefix_socket_t;
typedef 	int        prefix_signal_t;

#define CALLBACK_EVENT_GENERIC 		0x00
typedef void (*prefix_event_callback_t)(prefix_socket_t fd, short event, void *arg);
//typedef void (*prefix_event_callback_t)(void *arg);

typedef struct prefix_event_base_s  prefix_event_base_t;
typedef struct prefix_event_op_s    prefix_event_op_t;
typedef struct prefix_event_s 	    prefix_event_t;
typedef struct prefix_min_heap_s    prefix_min_heap_t;

#define    prefix_malloc        malloc
#define    prefix_free          free
#define    prefix_realloc 		realloc

#if 0
#define    PREFIX_EV_READ       0x01
#define    PREFIX_EV_WRITE      0x02
#define    PREFIX_EV_SIG        0x04
#define    PREFIX_EV_TIME       0x08
#define    PREFIX_EV_PERSIST    0x10
#else
#define    EV_READ         0x01
#define    EV_WRITE        0x02
#define    EV_SIG          0x04
#define    EV_TIME         0x08
#define    EV_PERSIST      0x10
#endif

#define    SUCCESS   0
#define    ERROR 	-1

#endif

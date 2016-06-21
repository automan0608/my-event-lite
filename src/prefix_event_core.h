
#ifndef _PREFIX_EVENT_CORE_H_
#define _PREFIX_EVENT_CORE_H_

typedef 	int        prefix_socket_t;
typedef 	int        prefix_signal_t;

typedef void (*prefix_event_callback_t)(void *arg);

typedef struct prefix_event_base_s 	prefix_event_base_t;
typedef struct prefix_event_op_s 	prefix_event_op_t;
typedef struct prefix_event_s 		prefix_event_t;
typedef struct prefix_min_heap_s	prefix_min_heap_t;

#define prefix_malloc  	malloc
#define prefix_free		free

#define    PREFIX_EV_READ        0x01
#define    PREFIX_EV_WRITE       0x02
#define    PREFIX_EV_SIG           0x04
#define    PREFIX_EV_TIME         0x08
#define    PREFIX_EV_PERSIST    0x10

#define SUCCESS 	0
#define ERROR 	-1

#endif

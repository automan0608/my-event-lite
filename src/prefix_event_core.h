
#ifndef _PREFIX_EVENT_CORE_H_
#define _PREFIX_EVENT_CORE_H_

#include <stdlib.h>

typedef 	int	prefix_socket_t;
typedef 	int 	prefix_signal_t;

typedef void (*prefix_event_callback_t)(void *arg);

typedef struct prefix_event_base_s 	prefix_event_base_t;
typedef struct prefix_event_op_s 	prefix_event_op_t;
typedef struct prefix_event_s 		prefix_event_t;
typedef struct prefix_min_heap_s	prefix_min_heap_t;

#define prefix_malloc 		malloc
#define prefix_free 		free

#endif

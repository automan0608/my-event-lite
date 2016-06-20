
#ifndef _PREFIX_EVENT_BASE_H_
#define _PREFIX_EVENT_BASE_H_

typedef struct prefix_event_base_s prefix_event_base_t;

struct prefix_event_base_s
{
    prefix_eventop_t 		*evOps;

    prefix_event_t 		*evIOHead;
    prefix_event_t 		*evSigHead;
    prefix_event_t 		*evTimeHead;

    prefix_min_heap_t  		*timeHeap;

    prefix_event_t 		*evActive;

    prefix_socket_t 		notifyFd[2]; 	//
};

#endif

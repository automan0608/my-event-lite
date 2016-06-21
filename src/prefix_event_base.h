
#ifndef _PREFIX_EVENT_BASE_H_
#define _PREFIX_EVENT_BASE_H_

#include "prefix_event_core.h"

struct prefix_event_base_s
{
    prefix_event_op_t 		*eventOps;

    prefix_event_t 		*eventIOHead;
    prefix_event_t 		*eventSigHead;
    prefix_event_t 		*eventTimeHead;

    prefix_min_heap_t  		*timeHeap;

    prefix_event_t 		*eventActive;

    prefix_socket_t 		notifyFd[2]; 	//
};

#endif

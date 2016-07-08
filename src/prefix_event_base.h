
#ifndef _PREFIX_EVENT_BASE_H_
#define _PREFIX_EVENT_BASE_H_

#include "prefix_core.h"

struct prefix_event_base_s
{
    const prefix_event_op_t 	*eventOps;

    prefix_event_t 		    	*eventIOHead;
    prefix_event_t 		    	*eventSigHead;
    prefix_event_t 		    	*eventTimeHead;
    prefix_bufferevent_t 		*buffereventHead;

    prefix_min_heap_t  			*timeHeap;

    prefix_event_t 		    	*eventActive;
    prefix_bufferevent_t 	   	*buffereventActive;

    prefix_socket_t 			 notifyFd[2]; 	//
};

prefix_event_base_t *prefix_event_base_new();

int prefix_event_base_add_event(prefix_event_t *event);
int prefix_event_base_add_bufferevent(prefix_bufferevent_t *event);

int prefix_event_base_set_event_active(prefix_event_base_t *base, prefix_event_t *event);
int prefix_event_base_set_bufferevent_active(prefix_event_base_t *base, prefix_bufferevent_t *event);

int prefix_event_base_dispatch(prefix_event_base_t *base);

int prefix_event_base_remove_event(prefix_event_base_t *base, prefix_event_t *event);

void prefix_event_base_free(prefix_event_base_t *base);

void prefix_event_base_dump(prefix_event_base_t *base);
#endif


#ifndef _PREFIX_EVENTOP_SELECT_H_
#define _PREFIX_EVENTOP_SELECT_H_

#include "prefix_event_core.h"

void *select_init(prefix_event_base_t *);
int select_add(prefix_event_base_t *, int, short old, short events, void*);
int select_del(prefix_event_base_t *, int, short old, short events, void*);
int select_dispatch(prefix_event_base_t *, struct timeval *);
//static void select_dealloc(prefix_event_base_t *);

extern prefix_event_op_t selectOps;

#endif


#ifndef _PREFIX_MIN_HEAP_H_
#define _PREFIX_MIN_HEAP_H_

#include "prefix_core.h"

typedef struct prefix_min_heap_node_s prefix_min_heap_t;

struct prefix_min_heap_node_s
{
	struct timeval tv;
	prefix_event_t *event;
};

struct prefix_min_heap_s
{
	struct prefix_min_heap_node_s **node;
	int n;
	int slots;
};

prefix_min_heap_t *prefix_min_heap_create();

void prefix_min_heap_free();

#endif

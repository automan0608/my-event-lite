
#ifndef _PREFIX_MIN_HEAP_H_
#define _PREFIX_MIN_HEAP_H_

#include "prefix_core.h"

typedef struct prefix_min_heap_node_s prefix_min_heap_node_t;

struct prefix_min_heap_node_s
{
	struct timeval tv;
	prefix_event_t *event;
};

struct prefix_min_heap_s
{
	int slots;
	int n;
	struct prefix_min_heap_node_s **nodes;
};

prefix_min_heap_t *prefix_min_heap_init();

int prefix_min_heap_push(prefix_min_heap_t *heap, struct timeval tv, prefix_event_t *event);

prefix_event_t *prefix_min_heap_pop(prefix_min_heap_t *heap);

struct timeval *prefix_min_heap_get_top(prefix_min_heap_t *heap);

void prefix_min_heap_free(prefix_min_heap_t *heap);

void prefix_min_heap_dump(prefix_min_heap_t *heap);

#endif


#include "prefix_min_heap.h"

prefix_min_heap_t *prefix_min_heap_create()
{
	prefix_min_heap_t *heap;

	heap = (prefix_min_heap_t *)prefix_malloc(sizeof(prefix_min_heap_t));
	if (NULL == heap)
	{
		prefix_log("error", "malloc min heap error");
		return NULL;
	}
	prefix_log("debug", "malloc min heap error");

	memset(heap, 0, sizeof(prefix_min_heap_t));
}

int prefix_min_heap_insert(prefix_min_heap_t *heap, struct timeval tv, prefix_event_t *event)
{
	prefix_min_heap_node_t *node;

	node = (prefix_min_heap_node_t *)prefix_malloc(sizeof(prefix_min_heap_node_t));
	if (NULL == node)
	{
		prefix_log("error", "malloc min heap node error");
		return NULL;
	}

	prefix_log("error", "malloc min heap node error");
	memset(node)


}

void prefix_min_heap_free()
{
	return;
}

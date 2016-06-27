#include <stdlib.h>
#include <string.h>

#include "prefix_log.h"
#include "prefix_base.h"
#include "prefix_min_heap.h"

prefix_min_heap_t *prefix_min_heap_init()
{
	prefix_min_heap_t *heap;

	heap = (prefix_min_heap_t *)prefix_malloc(sizeof(prefix_min_heap_t));
	if (NULL == heap)
	{
		prefix_log("error", "malloc min heap error");
		return NULL;
	}
	prefix_log("debug", "malloc min heap success");

	memset(heap, 0, sizeof(prefix_min_heap_t));

	return heap;
}

int prefix_min_heap_push(prefix_min_heap_t *heap, struct timeval tv, prefix_event_t *event)
{
	if (NULL == heap || (0 >= tv.tv_sec && 0 >= tv.tv_usec) || NULL == event)
	{
		prefix_log("error", "parameter error");
		return ERROR;
	}

	prefix_min_heap_node_t *node;

	// if slots not enough, add 8 slots.
	if (0 == heap->slots)
	{
		heap->nodes = (prefix_min_heap_node_t **)prefix_malloc(8 * sizeof(prefix_min_heap_node_t *));
		if (NULL == heap->nodes)
		{
			prefix_log("error", "malloc min heap nodes error");
			return ERROR;
		}
		heap->slots = 8;
	}
	else if (heap->n == heap->slots)
	{
		prefix_min_heap_node_t **ptrTmp = NULL;
		ptrTmp = (prefix_min_heap_node_t **)prefix_realloc(heap->nodes, (heap->slots + 8) * sizeof(prefix_min_heap_node_t *));
		if (NULL == ptrTmp)
		{
			prefix_log("error", "realloc min heap error");
			return ERROR;
		}

		memset(ptrTmp + heap->slots, 0, 8 * sizeof(prefix_min_heap_node_t *));

		heap->slots += 8;
		heap->nodes = ptrTmp;
	}

	node = (prefix_min_heap_node_t *)prefix_malloc(sizeof(prefix_min_heap_node_t));
	if (NULL == node)
	{
		prefix_log("error", "malloc min heap node error");
		return ERROR;
	}

	prefix_log("debug", "malloc min heap node success");
	memset(node, 0, sizeof(prefix_min_heap_t));

	node->tv = tv;
	node->event = event;

	(heap->n)++;
	heap->nodes[heap->n - 1] = node;

	int holeIdx = heap->n - 1;
	int parentIdx = (holeIdx + 1)/2 - 1;

	while (0 != holeIdx)
	{
	    parentIdx = (holeIdx + 1)/2 - 1;

	 	if (prefix_base_timeval_cmp(heap->nodes[parentIdx]->tv, heap->nodes[holeIdx]->tv) > 0)
	 	{
			prefix_min_heap_node_t *ptrTmp = heap->nodes[parentIdx];
			heap->nodes[parentIdx] = heap->nodes[holeIdx];
			heap->nodes[holeIdx] = ptrTmp;

			holeIdx = parentIdx;
			continue;
		}
		break;
	}

	return SUCCESS;
}

prefix_event_t *prefix_min_heap_pop(prefix_min_heap_t *heap)
{
	if (NULL == heap)
	{
		prefix_log("error", "parameter error");
		return NULL;
	}

	if (0 == heap->n)
	{
		prefix_log("error", "nothing to pop");
		return NULL;
	}

	prefix_event_t *event = heap->nodes[0]->event;
	prefix_free(heap->nodes[0]);
	heap->nodes[0] = NULL;

	int holeIdx = 0;
	int minChildIdx = 0;
	int leftChildIdx = (holeIdx + 1) * 2 - 1;

	while (leftChildIdx <= heap->n - 1)
	{
		if (leftChildIdx == heap->n - 1)
		{
			minChildIdx = leftChildIdx;
		}
		else if (prefix_base_timeval_cmp(heap->nodes[leftChildIdx]->tv, heap->nodes[leftChildIdx + 1]->tv) > 0)
		{
			minChildIdx = leftChildIdx + 1;
		}
		else
		{
			minChildIdx = leftChildIdx;
		}

		heap->nodes[holeIdx] = heap->nodes[minChildIdx];
		heap->nodes[minChildIdx] = NULL;

		holeIdx = minChildIdx;
		leftChildIdx = (holeIdx + 1) * 2 - 1;
	}
	if (holeIdx != heap->n - 1)
	{
		heap->nodes[holeIdx] = heap->nodes[heap->n - 1];
	}

	heap->nodes[heap->n - 1] = NULL;
	heap->n--;

	return event;
}

void prefix_min_heap_free(prefix_min_heap_t *heap)
{
	if (NULL == heap)
	{
		prefix_log("debug", "heap already freed");
		return;
	}

	prefix_free(heap->nodes);
	prefix_free(heap);

	return;
}

void prefix_min_heap_dump(prefix_min_heap_t *heap)
{
	if (NULL == heap)
	{
		prefix_log("error", "parameter error");
		return;
	}

	int i = 0;

	printf("***********************************************************\n");
	printf("******************** min heap dump ************************\n");
	printf("***********************************************************\n");
	printf("    heap->slots:        %d      \n", heap->slots);
	printf("    heap->n:            %d      \n", heap->n);
	printf("    heap->nodes:        %p      \n\n", heap->nodes);
	for (i = 0; i < heap->slots; ++i)
	{
		printf("            nodes[%d]:        %p        \n", i, heap->nodes[i]);
		if (NULL != heap->nodes[i])
		{
		printf("            nodes[%d]->tv.tv_sec:    %d      \n", i, (int)heap->nodes[i]->tv.tv_sec);
		printf("            nodes[%d]->tv.tv_usec:   %d      \n", i, (int)heap->nodes[i]->tv.tv_usec);
		printf("            nodes[%d]->event:        %p      \n", i, heap->nodes[i]->event);
		}
		printf("\n");
	}
	printf("***********************************************************\n");
}

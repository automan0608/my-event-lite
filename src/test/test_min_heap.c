#include <stdlib.h>
#include <stdio.h>

#include "prefix_core.h"
#include "prefix_event_base.h"
#include "prefix_event.h"
#include "prefix_min_heap.h"

void callback(void *arg)
{
	printf("In callback\n");
}

int main()
{
	prefix_event_base_t *base = prefix_event_base_new();

	prefix_event_t *event = prefix_event_new(base, 5, PREFIX_EV_READ | PREFIX_EV_PERSIST, NULL, callback, NULL);

	struct timeval tv = {0, 0};

	{
		tv.tv_sec = 900;
		prefix_min_heap_push(base->timeHeap, tv, event);
		prefix_min_heap_dump(base->timeHeap);

		tv.tv_sec = 200;
		prefix_min_heap_push(base->timeHeap, tv, event);
		prefix_min_heap_dump(base->timeHeap);

		tv.tv_sec = 300;
		prefix_min_heap_push(base->timeHeap, tv, event);
		prefix_min_heap_dump(base->timeHeap);

		tv.tv_sec = 100;
		prefix_min_heap_push(base->timeHeap, tv, event);
		prefix_min_heap_dump(base->timeHeap);

		tv.tv_sec = 500;
		prefix_min_heap_push(base->timeHeap, tv, event);
		prefix_min_heap_dump(base->timeHeap);

		tv.tv_sec = 800;
		prefix_min_heap_push(base->timeHeap, tv, event);
		prefix_min_heap_dump(base->timeHeap);

		tv.tv_sec = 400;
		prefix_min_heap_push(base->timeHeap, tv, event);
		prefix_min_heap_dump(base->timeHeap);

		tv.tv_sec = 600;
		prefix_min_heap_push(base->timeHeap, tv, event);
		prefix_min_heap_dump(base->timeHeap);

		tv.tv_sec = 700;
		prefix_min_heap_push(base->timeHeap, tv, event);
		prefix_min_heap_dump(base->timeHeap);
#if 0
#endif
	}

	prefix_min_heap_dump(base->timeHeap);

	{
		event = prefix_min_heap_pop(base->timeHeap);
		prefix_min_heap_dump(base->timeHeap);

		event = prefix_min_heap_pop(base->timeHeap);
		prefix_min_heap_dump(base->timeHeap);

		event = prefix_min_heap_pop(base->timeHeap);
		prefix_min_heap_dump(base->timeHeap);

		event = prefix_min_heap_pop(base->timeHeap);
		prefix_min_heap_dump(base->timeHeap);

		event = prefix_min_heap_pop(base->timeHeap);
		prefix_min_heap_dump(base->timeHeap);

		event = prefix_min_heap_pop(base->timeHeap);
		prefix_min_heap_dump(base->timeHeap);

		event = prefix_min_heap_pop(base->timeHeap);
		prefix_min_heap_dump(base->timeHeap);

		event = prefix_min_heap_pop(base->timeHeap);
		prefix_min_heap_dump(base->timeHeap);

		event = prefix_min_heap_pop(base->timeHeap);
		prefix_min_heap_dump(base->timeHeap);

	}
}

#include <stdlib.h>
#include <stdio.h>

#include "prefix_core.h"
#include "prefix_log.h"
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
	struct timeval *tvGet;

	{
		tv.tv_sec = 900;
		prefix_min_heap_push(base->timeHeap, tv, event);
		prefix_log("debug", "push min heap success");
		tvGet = prefix_min_heap_get_top(base->timeHeap);
		prefix_log("debug", "get min heap top:%p, tv_sec:%d, tv_usec:%d",
							tvGet, (int)tvGet->tv_sec, (int)tvGet->tv_usec);

		tv.tv_sec = 200;
		prefix_min_heap_push(base->timeHeap, tv, event);
		prefix_log("debug", "push min heap success");
		tvGet = prefix_min_heap_get_top(base->timeHeap);
		prefix_log("debug", "get min heap top:%p, tv_sec:%d, tv_usec:%d",
							tvGet, (int)tvGet->tv_sec, (int)tvGet->tv_usec);

		tv.tv_sec = 300;
		prefix_min_heap_push(base->timeHeap, tv, event);
		prefix_log("debug", "push min heap success");
		tvGet = prefix_min_heap_get_top(base->timeHeap);
		prefix_log("debug", "get min heap top:%p, tv_sec:%d, tv_usec:%d",
							tvGet, (int)tvGet->tv_sec, (int)tvGet->tv_usec);

		tv.tv_sec = 100;
		prefix_min_heap_push(base->timeHeap, tv, event);
		prefix_log("debug", "push min heap success");
		tvGet = prefix_min_heap_get_top(base->timeHeap);
		prefix_log("debug", "get min heap top:%p, tv_sec:%d, tv_usec:%d",
							tvGet, (int)tvGet->tv_sec, (int)tvGet->tv_usec);

		tv.tv_sec = 500;
		prefix_min_heap_push(base->timeHeap, tv, event);
		prefix_log("debug", "push min heap success");
		tvGet = prefix_min_heap_get_top(base->timeHeap);
		prefix_log("debug", "get min heap top:%p, tv_sec:%d, tv_usec:%d",
							tvGet, (int)tvGet->tv_sec, (int)tvGet->tv_usec);

		tv.tv_sec = 800;
		prefix_min_heap_push(base->timeHeap, tv, event);
		prefix_log("debug", "push min heap success");
		tvGet = prefix_min_heap_get_top(base->timeHeap);
		prefix_log("debug", "get min heap top:%p, tv_sec:%d, tv_usec:%d",
							tvGet, (int)tvGet->tv_sec, (int)tvGet->tv_usec);

		tv.tv_sec = 400;
		prefix_min_heap_push(base->timeHeap, tv, event);
		prefix_log("debug", "push min heap success");
		tvGet = prefix_min_heap_get_top(base->timeHeap);
		prefix_log("debug", "get min heap top:%p, tv_sec:%d, tv_usec:%d",
							tvGet, (int)tvGet->tv_sec, (int)tvGet->tv_usec);

		tv.tv_sec = 600;
		prefix_min_heap_push(base->timeHeap, tv, event);
		prefix_log("debug", "push min heap success");
		tvGet = prefix_min_heap_get_top(base->timeHeap);
		prefix_log("debug", "get min heap top:%p, tv_sec:%d, tv_usec:%d",
							tvGet, (int)tvGet->tv_sec, (int)tvGet->tv_usec);

		tv.tv_sec = 700;
		prefix_min_heap_push(base->timeHeap, tv, event);
		prefix_log("debug", "push min heap success");
		tvGet = prefix_min_heap_get_top(base->timeHeap);
		prefix_log("debug", "get min heap top:%p, tv_sec:%d, tv_usec:%d",
							tvGet, (int)tvGet->tv_sec, (int)tvGet->tv_usec);

	}

	prefix_min_heap_dump(base->timeHeap);

	{
		tvGet = prefix_min_heap_get_top(base->timeHeap);
		prefix_log("debug", "get min heap top:%p, tv_sec:%d, tv_usec:%d",
							tvGet, (int)tvGet->tv_sec, (int)tvGet->tv_usec);

		event = prefix_min_heap_pop(base->timeHeap);
		prefix_min_heap_dump(base->timeHeap);
		prefix_log("debug", "pop min heap success");
		tvGet = prefix_min_heap_get_top(base->timeHeap);
		prefix_log("debug", "get min heap top:%p, tv_sec:%d, tv_usec:%d",
							tvGet, (int)tvGet->tv_sec, (int)tvGet->tv_usec);


		event = prefix_min_heap_pop(base->timeHeap);
		prefix_min_heap_dump(base->timeHeap);
		prefix_log("debug", "pop min heap success");
		tvGet = prefix_min_heap_get_top(base->timeHeap);
		prefix_log("debug", "get min heap top:%p, tv_sec:%d, tv_usec:%d",
							tvGet, (int)tvGet->tv_sec, (int)tvGet->tv_usec);


		event = prefix_min_heap_pop(base->timeHeap);
		prefix_min_heap_dump(base->timeHeap);
		prefix_log("debug", "pop min heap success");
		tvGet = prefix_min_heap_get_top(base->timeHeap);
		prefix_log("debug", "get min heap top:%p, tv_sec:%d, tv_usec:%d",
							tvGet, (int)tvGet->tv_sec, (int)tvGet->tv_usec);


		event = prefix_min_heap_pop(base->timeHeap);
		prefix_min_heap_dump(base->timeHeap);
		prefix_log("debug", "pop min heap success");
		tvGet = prefix_min_heap_get_top(base->timeHeap);
		prefix_log("debug", "get min heap top:%p, tv_sec:%d, tv_usec:%d",
							tvGet, (int)tvGet->tv_sec, (int)tvGet->tv_usec);


		event = prefix_min_heap_pop(base->timeHeap);
		prefix_min_heap_dump(base->timeHeap);
		prefix_log("debug", "pop min heap success");
		tvGet = prefix_min_heap_get_top(base->timeHeap);
		prefix_log("debug", "get min heap top:%p, tv_sec:%d, tv_usec:%d",
							tvGet, (int)tvGet->tv_sec, (int)tvGet->tv_usec);


		event = prefix_min_heap_pop(base->timeHeap);
		prefix_min_heap_dump(base->timeHeap);
		prefix_log("debug", "pop min heap success");
		tvGet = prefix_min_heap_get_top(base->timeHeap);
		prefix_log("debug", "get min heap top:%p, tv_sec:%d, tv_usec:%d",
							tvGet, (int)tvGet->tv_sec, (int)tvGet->tv_usec);


		event = prefix_min_heap_pop(base->timeHeap);
		prefix_min_heap_dump(base->timeHeap);
		prefix_log("debug", "pop min heap success");
		tvGet = prefix_min_heap_get_top(base->timeHeap);
		prefix_log("debug", "get min heap top:%p, tv_sec:%d, tv_usec:%d",
							tvGet, (int)tvGet->tv_sec, (int)tvGet->tv_usec);


		event = prefix_min_heap_pop(base->timeHeap);
		prefix_min_heap_dump(base->timeHeap);
		prefix_log("debug", "pop min heap success");
		tvGet = prefix_min_heap_get_top(base->timeHeap);
		prefix_log("debug", "get min heap top:%p, tv_sec:%d, tv_usec:%d",
							tvGet, (int)tvGet->tv_sec, (int)tvGet->tv_usec);


		event = prefix_min_heap_pop(base->timeHeap);
		prefix_min_heap_dump(base->timeHeap);
		prefix_log("debug", "pop min heap success");
		tvGet = prefix_min_heap_get_top(base->timeHeap);
	}
}

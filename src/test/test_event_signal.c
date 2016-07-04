
#include <stdio.h>
#include <signal.h>

#include "event.h"

void cb(int fd, short events, void *arg)
{
	printf("in callback\n");
}

int main(int argc, char const *argv[])
{

	prefix_event_base_t *base = prefix_event_base_new();

	prefix_event_t *eventSig01 = prefix_event_new(base, SIGINT, EV_SIG|EV_PERSIST, NULL, cb, NULL);
	prefix_event_t *eventSig02 = prefix_event_new(base, SIGTERM, EV_SIG|EV_PERSIST, NULL, cb, NULL);

	struct timeval tv = {1, 10000};
	prefix_event_t *eventTime01 = prefix_event_new(base, -1, EV_TIME, &tv, cb, NULL);

	prefix_event_base_dispatch(base);

	return 0;
}

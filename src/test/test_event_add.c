
#include <stdio.h>
#include <sys/time.h>

#include "event.h"

void cb(int fd, short events, void *arg)
{
	printf("in callback\n");
}

int main()
{
	prefix_event_base_t *base = prefix_event_base_new();

	prefix_event_t *eventIO01 = prefix_event_new(base, 10, EV_READ, NULL, cb, NULL);
	prefix_event_t *eventIO02 = prefix_event_new(base, 11, EV_WRITE, NULL, cb, NULL);

	prefix_event_t *eventSig01 = prefix_event_new(base, -1, EV_SIG, NULL, cb, NULL);
	prefix_event_t *eventSig02 = prefix_event_new(base, -1, EV_SIG, NULL, cb, NULL);

	struct timeval tv = {10, 10000};
	prefix_event_t *eventTime01 = prefix_event_new(base, -1, EV_TIME, &tv, cb, NULL);
	prefix_event_t *eventTime02 = prefix_event_new(base, -1, EV_TIME, &tv, cb, NULL);

	prefix_event_base_dump(base);

	prefix_event_dump(eventIO01);
	prefix_event_dump(eventIO02);

	prefix_event_dump(eventSig01);
	prefix_event_dump(eventSig02);

	prefix_event_dump(eventTime01);
	prefix_event_dump(eventTime02);
}


#include <stdio.h>
#include <signal.h>

#include "event.h"

void cbtime(int fd, short events, void *arg)
{
	printf("in time callback\n");
}

void cbsig(int fd, short events, void *arg)
{
	printf("in signal callback, signo:%d\n", fd);
}

int main(int argc, char const *argv[])
{

	prefix_event_base_t *base = prefix_event_base_new();

	prefix_event_t *eventSig01 = prefix_event_new(base, SIGINT, EV_SIG|EV_PERSIST, NULL, cbsig, NULL);
//	prefix_event_t *eventSig01 = prefix_event_new(base, SIGINT, EV_SIG, NULL, cbsig, NULL);
	prefix_event_t *eventSig02 = prefix_event_new(base, SIGTERM, EV_SIG|EV_PERSIST, NULL, cbsig, NULL);

	struct timeval tv = {3, 10000};
	prefix_event_t *eventTime01 = prefix_event_new(base, -1, EV_TIME|EV_PERSIST, &tv, cbtime, NULL);

	prefix_event_base_dispatch(base);

	return 0;
}

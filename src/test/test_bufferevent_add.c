
#include <stdio.h>
#include <string.h>
#include <sys/time.h>

#include "event.h"

void cb(int fd, short events, void *arg)
{
	printf("in callback\n");
}

int main()
{
	prefix_event_base_t *base = prefix_event_base_new();

	prefix_bufferevent_attr_t attr01, attr02;
	prefix_bufferevent_attr_set_blocksize(&attr01, 20);
	prefix_bufferevent_attr_set_flushtype(&attr01, BUFFEREVENT_FLUSHTYPE_LINE);

	prefix_bufferevent_attr_set_blocksize(&attr02, BUFFEREVENT_BLOCKSIZE_DEFAULT);
	prefix_bufferevent_attr_set_flushtype(&attr02, BUFFEREVENT_FLUSHTYPE_DEFAULT);

	prefix_bufferevent_t *bufferevent01 = prefix_bufferevent_new(base, 10, EV_READ, NULL, cb, NULL, &attr01);
//	prefix_bufferevent_t *bufferevent02 = prefix_bufferevent_new(base, 11, EV_WRITE, NULL, cb, NULL, &attr02);

	char *buf01 = "how are you? ";
	char *buf02 = "fine. thank you. and you? ";
	char *buf03 = "i am fine too. ";

	prefix_bufferevent_write(bufferevent01, buf01, strlen(buf01));
	prefix_bufferevent_write(bufferevent01, buf02, strlen(buf02));
	prefix_bufferevent_write(bufferevent01, buf03, strlen(buf03));
	prefix_bufferevent_write(bufferevent01, buf01, strlen(buf01));
	prefix_bufferevent_write(bufferevent01, buf02, strlen(buf02));
	prefix_bufferevent_write(bufferevent01, buf03, strlen(buf03));

	prefix_event_base_dump(base);

//	prefix_bufferevent_dump(bufferevent01);
//	prefix_bufferevent_dump(bufferevent02);

}

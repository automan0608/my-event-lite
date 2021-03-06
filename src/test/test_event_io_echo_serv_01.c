#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "event.h"

#define PORT 6000
#define MAXLINE 1024

void cb(int fd, short events, void *arg)
{
	printf("in callback, fd:%d, events:%d, arg:%p\n", fd, events, arg);

    prefix_event_t **ev = (prefix_event_t **)arg;

	ssize_t n;
	char buf[MAXLINE];

cb_again:
	n = read(fd, buf, MAXLINE);
    printf("read n:%d\n", (int)n);
	if (0 < n)
	{
		write(fd, buf, n);
	}
	else if (0 == n)
	{
		close(fd);
		prefix_event_free(*ev);
	}
	else
	{
		if (errno == EINTR)
		{
			printf("error EINTR\n");
			goto cb_again;
		}
		printf("read error\n");
		return ;
	}
}

int main(int argc, char const *argv[])
{
	struct sockaddr_in servaddr, cliaddr;
	int listenfd, connfd;
	socklen_t clilen;
	int result;

    prefix_event_base_t *base;
    prefix_event_t *event;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (0 > listenfd)
    {
    	printf("socket syscall error\n");
    	return -1;
    }
   	printf("socket syscall success\n");

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    result = bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    if (0 > result)
    {
    	printf("bind syscall error\n");
    	return -1;
    }
   	printf("bind syscall success\n");

    result = listen(listenfd, 100);
    if (0 > result)
    {
    	printf("listen syscall error\n");
    	return -1;
    }
   	printf("listen syscall success\n");

//    prefix_event_base_t *base = NULL;
//    prefix_event_t *event = NULL;

    base= prefix_event_base_new();
    if (NULL == base)
    {
    	printf("event base new error\n");
    	return -1;
    }
   	printf("event base new success\n");

    clilen = sizeof(struct sockaddr);
again:
	printf("will accept for connection\n");
    connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);
	printf("accept connfd:%d\n", connfd);
    if (0 >= connfd)
    {
    	if (EINTR == errno)
    	{
    		printf("errno EINTR\n");
    		return -1;
    	}
    	printf("accept error\n");
    	return -1;
    }

    // be carefull, here pass the ptr of event(event is a point)
    event = prefix_event_new(base, connfd, EV_READ|EV_PERSIST, NULL, cb, &event);
//    event = prefix_event_new(base, connfd, EV_READ, NULL, cb, &event);
    printf("event new:%p\n", event);

    prefix_event_base_dispatch(base);

    goto again;

	return 0;
}

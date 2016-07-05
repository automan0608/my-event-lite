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

prefix_event_base_t *base;

void cbconn(int fd, short events, void *arg)
{
	printf("in connfd callback, fd:%d, events:%d, arg:%p\n", fd, events, arg);

    prefix_event_t **evconn = (prefix_event_t **)arg;

	ssize_t n;
	char buf[MAXLINE];

cbconn_again:
    printf("before read\n");
	n = read(fd, buf, MAXLINE);
    printf("read n:%d\n", (int)n);
	if (0 < n)
	{
		write(fd, buf, n);
	}
	else if (0 == n)
	{
		close(fd);
		prefix_event_free(*evconn);
        free(evconn);
	}
	else
	{
		if (errno == EINTR)
		{
			printf("error EINTR\n");
			goto cbconn_again;
		}
		printf("read error\n");
        prefix_event_free(*evconn);
        free(evconn);
	}

    return;
}

void cblisten(int fd, short events, void *arg)
{
    printf("in listenfd callback, fd:%d, events:%d, arg:%p\n", fd, events, arg);
    prefix_event_t **evlisten = (prefix_event_t **)arg;

    struct sockaddr_in cliaddr;
    socklen_t clilen;
    prefix_event_t **evconn;
    int connfd;

    clilen = sizeof(struct sockaddr);

    // for test
cblisten_again:
    printf("before accept\n");
    connfd = accept(fd, (struct sockaddr *)&cliaddr, &clilen);
    printf("accept connfd:%d\n", connfd);
    if (0 >= connfd)
    {
        if (EINTR == errno)
        {
            printf("errno EINTR\n");
            goto cblisten_again;
        }
        printf("accept error\n");
        // only here need to free the listen event
        prefix_event_free(*evlisten);
        return;
    }

    // malloc a ptr to ptr of event is necessary.
    evconn = (prefix_event_t **)malloc(sizeof(prefix_event_t *));
    if (NULL == evconn)
    {
        printf("malloc connect ev error\n");
        return;
    }
    memset(evconn, 0, sizeof(prefix_event_t *));

    // be carefull, here pass the ptr of event(event is a point)
    *evconn = prefix_event_new(base, connfd, EV_READ|EV_PERSIST, NULL, cbconn, evconn);
    printf("evconn new:%p\n", evconn);
}

int main(int argc, char const *argv[])
{
	struct sockaddr_in servaddr;
	int listenfd;
	socklen_t clilen;
	int result;

//    prefix_event_base_t *base;
    prefix_event_t *evlisten;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (0 > listenfd)
    {
    	printf("socket syscall error\n");
    	return -1;
    }
   	printf("socket syscall success,listenfd:%d\n", listenfd);

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

    base= prefix_event_base_new();
    if (NULL == base)
    {
    	printf("event base new error\n");
    	return -1;
    }
   	printf("event base new success\n");

    evlisten = prefix_event_new(base, listenfd, EV_READ|EV_PERSIST, NULL, cblisten, &evlisten);

    prefix_event_base_dispatch(base);

    return 0;
}

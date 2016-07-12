/*
* echo serv use thread
*
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <pthread.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "event.h"

#define PORT 6000
#define MAXLINE 1024

pthread_t tid;
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

void cbsig(int fd, short events, void *arg)
{
    printf("in signal callback, signo:%d\n", fd);
}

void *thread_fn(void *arg)
{
    prefix_event_base_t *base = (prefix_event_base_t *)arg;

    prefix_event_t *sig = prefix_event_new(base, SIGINT, EV_SIG|EV_PERSIST, NULL, cbsig, NULL);

    prefix_event_base_dispatch(base);
}

int main(int argc, char const *argv[])
{
	struct sockaddr_in servaddr;
	int listenfd;
	socklen_t clilen;
	int result;
    struct sockaddr_in cliaddr;
    prefix_event_t **evconn;
    int connfd;

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
    prefix_event_base_use_thread(base);

    result = pthread_create(&tid, NULL, thread_fn, (void *)base);
    if (0 != result)
    {
        printf("thread create error\n");
        return -1;
    }

    clilen = sizeof(struct sockaddr);

    while (1)
    {
cblisten_again:
        printf("before accept\n");
        connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);
        printf("accept connfd:%d\n", connfd);
        if (0 >= connfd)
        {
            if (EINTR == errno)
            {
                printf("errno EINTR\n");
                goto cblisten_again;
            }
            printf("accept error\n");
            // only here need to return
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
        *evconn = prefix_event_new(base, connfd, EV_READ|EV_PERSIST, NULL, cbconn, (void *)evconn);
        printf("evconn new:%p\n", evconn);
    }

    return 0;
}

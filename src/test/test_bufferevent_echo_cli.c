#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "event.h"

#define IP "192.168.0.11"
#define PORT 6000
#define MAXLINE 1024

struct test_arg_s {
    int fd;
    prefix_event_t **event_cml;
    prefix_bufferevent_t **event_io;

}arg_cbcml, arg_cbio;

void cbcml(int fd, short events, void *arg)
{
//    printf("in cml callback, fd:%d, events:%d, arg:%p\n", fd, events, arg);

#if 0
// not support io event timeout at present
    if (EVENT_ACTIVETYPE_TIMEOUT == events)
    {
        printf("active type is timeout\n");
        return;
    }
#endif

    int fdio = ((struct test_arg_s *)arg)->fd;
    prefix_event_t **event_cml = ((struct test_arg_s *)arg)->event_cml;
    prefix_bufferevent_t **event_io = ((struct test_arg_s *)arg)->event_io;

    ssize_t n;
    char buf[MAXLINE];

//    printf("before read\n");
    n = read(fd, buf, MAXLINE);
//    printf("read n:%d\n", (int)n);
    if (0 < n)
    {
        prefix_bufferevent_write(*event_io, buf, n);
    }
    else if (0 == n)
    {
        printf("EOF, will close with server\n");
        close(fdio);
        prefix_event_free(*event_cml);
        prefix_bufferevent_free(*event_io);
    }
    else
    {
        printf("read cml error\n");
        prefix_event_free(*event_cml);
        prefix_bufferevent_free(*event_io);
    }

    return;
}

void cbio(int fd, short events, void *arg)
{
//    printf("in io callback, fd:%d, events:%d, arg:%p\n", fd, events, arg);

    if (EVENT_ACTIVETYPE_BUFFERWRITE == events)
    {
        printf("bufferwrite got it\n");
        return;
    }

    int fdcml = ((struct test_arg_s *)arg)->fd;
    prefix_event_t **event_cml = ((struct test_arg_s *)arg)->event_cml;
    prefix_bufferevent_t **event_io = ((struct test_arg_s *)arg)->event_io;

    ssize_t n;
    char buf[MAXLINE];

//    printf("before read\n");
    n = prefix_bufferevent_read(*event_io, buf, MAXLINE);
//    printf("read n:%d\n", (int)n);
    if (0 < n)
    {
        write(fdcml, buf, n);
    }
    else if (0 == n)
    {
        printf("FIN, connection closed by foreign host\n");
        close(fd);
        prefix_event_free(*event_cml);
        prefix_bufferevent_free(*event_io);
    }
    else
    {
        printf("read cml error\n");
        prefix_event_free(*event_cml);
        prefix_bufferevent_free(*event_io);
    }

    return;
}

int main(int argc, char const *argv[])
{
    prefix_event_base_t *base;
    prefix_event_t *event_cml;
    prefix_bufferevent_t *event_io;

    struct sockaddr_in servaddr;
    int result = 0, sockfd = 0;
    unsigned char buf[sizeof(struct in_addr)];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (0 > sockfd)
    {
    	printf("socket syscall error\n");
    	return -1;
    }

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);

    result = inet_pton(AF_INET, IP, &servaddr.sin_addr.s_addr);
    if (0 > result)
    {
    	printf("inet_pton error\n");
    	return -1;
    }

    result = connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    if (0 > result)
    {
    	printf("connect syscall error\n");
    	return -1;
    }
    printf("connect %s:%d success, fd:%d\n", IP, PORT, sockfd);

    base = prefix_event_base_new();

    arg_cbcml.fd = sockfd;
    arg_cbcml.event_cml = &event_cml;
    arg_cbcml.event_io = &event_io;

    arg_cbio.fd = fileno(stdin);
    arg_cbio.event_cml = &event_cml;
    arg_cbio.event_io = &event_io;

    event_cml = prefix_event_new(base, fileno(stdin), EV_READ|EV_PERSIST, NULL, cbcml, (void *)&arg_cbcml);

#if 1
    prefix_bufferevent_attr_t attr;
    prefix_bufferevent_attr_set_blocksize(&attr, 200);
    prefix_bufferevent_attr_set_flushtype(&attr, BUFFEREVENT_FLUSHTYPE_BLOCK);
    event_io = prefix_bufferevent_new(base, sockfd, EV_READ|EV_WRITE|EV_PERSIST, NULL, cbio, (void *)&arg_cbio, &attr);
#else
    prefix_bufferevent_attr_t attr;
    prefix_bufferevent_attr_set_blocksize(&attr, 20);
    prefix_bufferevent_attr_set_flushtype(&attr, BUFFEREVENT_FLUSHTYPE_BLOCK);
    event_io = prefix_bufferevent_new(base, sockfd, EV_READ|EV_PERSIST, NULL, cbio, (void *)&arg_cbio, &attr);
#endif

    prefix_event_base_dispatch(base);

	return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "event.h"

#define IP "192.168.0.25"
#define PORT 6000

int main(int argc, char const *argv[])
{

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
    printf("connect %s:%d success\n", IP, PORT);

    prefix_event_base_t *base = prefix_event_base_new();
	return 0;
}

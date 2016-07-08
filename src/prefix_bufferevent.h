
#ifndef _PREFIX_BUFFEREVENT_H_
#define _PREFIX_BUFFEREVENT_H_

#include <sys/time.h>
#include "prefix_core.h"

#define BUFFEREVENT_STATUS_AVAIL           0x01
#define BUFFEREVENT_STATUS_IN_MIN_HEAP     0x02
#define BUFFEREVENT_STATUS_SIG_INSTALLED   0x04
#define BUFFEREVENT_STATUS_ACTIVE          0x08
#define BUFFEREVENT_STATUS_INVOKED         0x10
#define BUFFEREVENT_STATUS_FREED           0x20

typedef struct prefix_evbuffer_block_s 		prefix_evbuffer_block_t;
typedef struct prefix_evbuffer_s 			prefix_evbuffer_t;

// evbuffer manager struct
struct prefix_evbuffer_block_s
{
	struct prefix_evbuffer_block_s *prev;
	struct prefix_evbuffer_block_s *next;

	int length;
	int ptrHead;
	int ptrTail;

	char buf[0];
};

// evbuffer block manager struct
struct prefix_evbuffer_s
{
	int blockNum;
	int blockSize;

	struct prefix_evbuffer_block_s *blockHead;
	struct prefix_evbuffer_block_s *blockTail;
};

struct prefix_bufferevent_s
{
	struct prefix_bufferevent_s *prev;
	struct prefix_bufferevent_s *next;

	struct prefix_bufferevent_s *activePrev;
	struct prefix_bufferevent_s *activeNext;

    int eventStatus;
    int eventActiveType;

	struct prefix_event_base_s *base;

    prefix_event_callback_t callback;
    void *arg;

    int fd;

    struct prefix_bufferevent_attr_s attr;      // blocksize & flushtype

    struct prefix_evbuffer_s *input;
    struct prefix_evbuffer_s *output;

    struct timeval timeout_read;
    struct timeval timeout_write;
};

int prefix_bufferevent_attr_set_blocksize(prefix_bufferevent_attr_t *attr, int blockSize);

int prefix_bufferevent_attr_set_flushtype(prefix_bufferevent_attr_t *attr, int flushtype);

prefix_bufferevent_t *prefix_bufferevent_new(prefix_event_base_t *base, prefix_socket_t fd,
										short events, const struct timeval *tv,
										void (*cb)(prefix_socket_t, short, void *),
										void *arg, prefix_bufferevent_attr_t *attr);

int prefix_bufferevent_write(prefix_bufferevent_t *event, const char *buf, size_t len);

ssize_t prefix_bufferevent_read(prefix_bufferevent_t *event, void *buf, size_t len);

int prefix_bufferevent_writev_inner(prefix_bufferevent_t *event, int fd, int rmflag);

int prefix_bufferevent_readv_inner(prefix_bufferevent_t *event, int fd);

int prefix_bufferevent_set_active(prefix_bufferevent_t *event, int activeType);

int prefix_bufferevent_invoke(prefix_bufferevent_t *event);

int prefix_bufferevent_delete(prefix_bufferevent_t *event);

void prefix_bufferevent_free(prefix_bufferevent_t *event);

void prefix_bufferevent_free_inner(prefix_bufferevent_t *event);

void prefix_bufferevent_dump(prefix_bufferevent_t *event);

#endif

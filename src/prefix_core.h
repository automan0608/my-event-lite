
#ifndef _PREFIX_EVENT_CORE_H_
#define _PREFIX_EVENT_CORE_H_

typedef 	int        prefix_socket_t;
typedef 	int        prefix_signal_t;

//#define CALLBACK_EVENT_GENERIC 		0x00
//#define CALLBACK_EVENT_TIMEOUT 		0x01
#define    EVENT_ACTIVETYPE_GENERIC        0x00
#define    EVENT_ACTIVETYPE_TIMEOUT        0x01
#define    EVENT_ACTIVETYPE_BUFFERREAD     0x02
#define    EVENT_ACTIVETYPE_BUFFERWRITE    0x04

typedef void (*prefix_event_callback_t)(prefix_socket_t fd, short event, void *arg);
//typedef void (*prefix_event_callback_t)(void *arg);

typedef struct prefix_event_base_s  		prefix_event_base_t;
typedef struct prefix_event_op_s    		prefix_event_op_t;
typedef struct prefix_event_s 	    		prefix_event_t;
typedef struct prefix_min_heap_s    		prefix_min_heap_t;
typedef struct prefix_bufferevent_s 		prefix_bufferevent_t;
typedef struct prefix_bufferevent_attr_s 	prefix_bufferevent_attr_t;

#define 	prefix_malloc        malloc
#define 	prefix_free          free
#define 	prefix_realloc		 realloc

#define 	EV_READ         0x01
#define 	EV_WRITE        0x02
#define 	EV_SIG          0x04
#define 	EV_TIME         0x08
#define 	EV_PERSIST      0x10

/* bufferevent related staff */
#define 	BUFFEREVENT_FLUSHTYPE_CHAR 		0x01
#define 	BUFFEREVENT_FLUSHTYPE_LINE  	0x02
#define		BUFFEREVENT_FLUSHTYPE_BLOCK 	0x04

#define 	BUFFEREVENT_FLUSHTYPE_DEFAULT 	BUFFEREVENT_FLUSHTYPE_CHAR
#define 	BUFFEREVENT_BLOCKSIZE_DEFAULT 	256

#if 0
#define 	BUFFEREVENT_STATUS_AVAIL           0x01
#define 	BUFFEREVENT_STATUS_IN_MIN_HEAP     0x02
#define 	BUFFEREVENT_STATUS_SIG_INSTALLED   0x04
#define 	BUFFEREVENT_STATUS_ACTIVE          0x08
#define 	BUFFEREVENT_STATUS_INVOKED         0x10
#define 	BUFFEREVENT_STATUS_FREED           0x20
#endif

struct prefix_bufferevent_attr_s
{
	int blockSize;
	int flushType;
};
/*(end) bufferevent related staff */



#define 	SUCCESS  	 0
#define 	ERROR		-1

#endif

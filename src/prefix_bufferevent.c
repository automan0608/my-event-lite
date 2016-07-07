
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "prefix_event_base.h"
#include "prefix_log.h"
#include "prefix_bufferevent.h"

int prefix_bufferevent_attr_set_blocksize(prefix_bufferevent_attr_t *attr, int blockSize)
{
	if (NULL == attr)
	{
		prefix_log("error", "parameter error");
		return ERROR;
	}

	if (0 >= blockSize)
		attr->blockSize = BUFFEREVENT_BLOCKSIZE_DEFAULT;
	else
		attr->blockSize = blockSize;

	return SUCCESS;
}

int prefix_bufferevent_attr_set_flushtype(prefix_bufferevent_attr_t *attr, int flushtype)
{
	if (NULL == attr)
	{
		prefix_log("error", "parameter error");
		return ERROR;
	}

	if (0 >= flushtype)
		attr->flushType = BUFFEREVENT_FLUSHTYPE_DEFAULT;
	else
		attr->flushType = flushtype;

	return SUCCESS;
}

prefix_bufferevent_t *
prefix_bufferevent_new(prefix_event_base_t *base, prefix_socket_t fd,
						short events, const struct timeval *tv,
                        void (*cb)(prefix_socket_t, short, void *), void *arg,
                        prefix_bufferevent_attr_t *attr)
{
	prefix_log("debug", "in");

	if (NULL == base)
	{
		prefix_log("error", "parameter error");
		return NULL;
	}

	int result = 0;
	prefix_bufferevent_t *event = NULL;

	event = (prefix_bufferevent_t*)prefix_malloc(sizeof(prefix_bufferevent_t));
	if (NULL == event)
	{
		prefix_log("error", "malloc error");
		return NULL;
	}
	memset(event, 0, sizeof(prefix_bufferevent_t));

	event->base = base;
	event->callback = cb;
	event->arg = arg;
	event->fd = fd;
	if (NULL == attr)
	{
		event->attr.blockSize = BUFFEREVENT_BLOCKSIZE_DEFAULT;
		event->attr.flushType = BUFFEREVENT_FLUSHTYPE_DEFAULT;
	}
	else
	{
		event->attr.blockSize = attr->blockSize;
		event->attr.flushType = attr->flushType;
	}

	event->input = (prefix_evbuffer_t *)prefix_malloc(sizeof(prefix_evbuffer_t));
	if (NULL == event->input)
	{
		prefix_log("error", "malloc evbuffer input error");
		prefix_free(event);
		return NULL;
	}
	memset(event->input, 0, sizeof(prefix_evbuffer_t));

	prefix_log("debug", "malloc evbuffer input success");
	event->input->blockNum = 0;
	event->input->blockSize = event->attr.blockSize;

	event->output = (prefix_evbuffer_t *)prefix_malloc(sizeof(prefix_evbuffer_t));
	if (NULL == event->output)
	{
		prefix_log("error", "malloc evbuffer output error");
		prefix_free(event);
		return NULL;
	}
	memset(event->input, 0, sizeof(prefix_evbuffer_t));

	prefix_log("debug", "malloc evbuffer output success");
	event->output->blockNum = 0;
	event->output->blockSize = event->attr.blockSize;

	result = prefix_event_base_add_bufferevent(event);
	if (SUCCESS != result)
	{
		prefix_log("error", "add event to eventbase error");
		prefix_free(event);
		return NULL;
	}

	event->eventStatus = BUFFEREVENT_STATUS_AVAIL;

	prefix_log("debug", "out");
	return event;
}

int prefix_bufferevent_write(prefix_bufferevent_t *event, const char *buf, size_t len)
{
	if (NULL == event || NULL == buf || 0 >= len)
	{
		prefix_log("error", "parameter error");
		return ERROR;
	}

	int i;
	int nblock = 0; // n blocks to be malloced
	size_t sizeTotal = 0; // size to be malloc
	size_t sizeSingle = 0;
	char *ptr = 0;

	if (0 == event->output->blockNum)
	{
		nblock = (len -1)/event->output->blockSize + 1;
		sizeSingle = sizeof(prefix_evbuffer_block_t) + event->output->blockSize;
		sizeTotal = nblock * sizeSingle;

		ptr = (char *)prefix_malloc(sizeTotal);
		if (NULL == ptr)
		{
			prefix_log("error", "malloc blocks error");
			return ERROR;
		}
		memset(ptr, 0, sizeTotal);

		event->output->blockNum += nblock;
		for (i = 0; i < nblock; ++i)
		{
			((prefix_evbuffer_block_t *)(ptr + i*sizeSingle))->length = event->output->blockSize;

#if 0
			if (0 == i)
			{
				((prefix_evbuffer_block_t *)(ptr + i*sizeSingle))->prev = NULL;
				((prefix_evbuffer_block_t *)(ptr + i*sizeSingle))->next = (prefix_evbuffer_block_t *)(ptr + (i+1)*sizeSingle);
				memcpy(((prefix_evbuffer_block_t *)(ptr + i*sizeSingle))->buf, buf + i*event->output->blockSize, event->output->blockSize);
			}
			else if (nblock -1 != i)
			{
				((prefix_evbuffer_block_t *)(ptr + i*sizeSingle))->prev = (prefix_evbuffer_block_t *)(ptr + (i-1)*sizeSingle);
				((prefix_evbuffer_block_t *)(ptr + i*sizeSingle))->next = (prefix_evbuffer_block_t *)(ptr + (i+1)*sizeSingle);
				memcpy(((prefix_evbuffer_block_t *)(ptr + i*sizeSingle))->buf, buf + i*event->output->blockSize, event->output->blockSize);
			}
			else
			{
				((prefix_evbuffer_block_t *)(ptr + i*sizeSingle))->prev = (prefix_evbuffer_block_t *)(ptr + (i-1)*sizeSingle);
				((prefix_evbuffer_block_t *)(ptr + i*sizeSingle))->next = NULL;
				memcpy(((prefix_evbuffer_block_t *)(ptr + i*sizeSingle))->buf, buf + i*event->output->blockSize, len%event->output->blockSize);
			}
#else
			((prefix_evbuffer_block_t *)(ptr + i*sizeSingle))->prev = (prefix_evbuffer_block_t *)(ptr + (i-1)*sizeSingle);
			((prefix_evbuffer_block_t *)(ptr + i*sizeSingle))->next = (prefix_evbuffer_block_t *)(ptr + (i+1)*sizeSingle);
			if (i != nblock - 1)
			{
				memcpy(((prefix_evbuffer_block_t *)(ptr + i*sizeSingle))->buf, buf + i*event->output->blockSize, event->output->blockSize);
				((prefix_evbuffer_block_t *)(ptr + i*sizeSingle))->ptrTail = event->output->blockSize;
			}
			else
			{
				memcpy(((prefix_evbuffer_block_t *)(ptr + i*sizeSingle))->buf, buf + i*event->output->blockSize, (len - 1)%event->output->blockSize + 1);
				((prefix_evbuffer_block_t *)(ptr + i*sizeSingle))->ptrTail = (len-1)%event->output->blockSize+1;
			}

#endif
		}
		((prefix_evbuffer_block_t *)(ptr))->prev = NULL;
		((prefix_evbuffer_block_t *)(ptr + (nblock-1)*sizeSingle))->next = NULL;

		event->output->blockHead = (prefix_evbuffer_block_t *)(ptr);
		event->output->blockTail = (prefix_evbuffer_block_t *)(ptr + (nblock-1)*sizeSingle);
	}
	else
	{
		size_t nTailLeft = event->output->blockTail->length - event->output->blockTail->ptrTail;

		nblock = (len -nTailLeft - 1)/event->output->blockSize + 1;
		sizeSingle = sizeof(prefix_evbuffer_block_t) + event->output->blockSize;
		sizeTotal = nblock * sizeSingle;

		ptr = (char *)prefix_malloc(sizeTotal);
		if (NULL == ptr)
		{
			prefix_log("error", "malloc blocks error");
			return ERROR;
		}
		memset(ptr, 0, sizeTotal);

		// copy a bit fragment to the tail block which has some space left
		memcpy(event->output->blockTail->buf + event->output->blockTail->ptrTail, buf, nTailLeft);
		event->output->blockTail->ptrTail = event->output->blockTail->length;

		event->output->blockNum += nblock;
		for (i = 0; i < nblock; ++i)
		{
			((prefix_evbuffer_block_t *)(ptr + i*sizeSingle))->length = event->output->blockSize;

#if 0
			if (0 == i)
			{
				((prefix_evbuffer_block_t *)(ptr + i*sizeSingle))->prev = event->output->blockTail;
				((prefix_evbuffer_block_t *)(ptr + i*sizeSingle))->next = (prefix_evbuffer_block_t *)(ptr + (i+1)*sizeSingle);
				memcpy(((prefix_evbuffer_block_t *)(ptr + i*sizeSingle))->buf, buf + nTailLeft + i*event->output->blockSize, event->output->blockSize);
			}
			else if (nblock -1 != i)
			{
				((prefix_evbuffer_block_t *)(ptr + i*sizeSingle))->prev = (prefix_evbuffer_block_t *)(ptr + (i-1)*sizeSingle);
				((prefix_evbuffer_block_t *)(ptr + i*sizeSingle))->next = (prefix_evbuffer_block_t *)(ptr + (i+1)*sizeSingle);
				memcpy(((prefix_evbuffer_block_t *)(ptr + i*sizeSingle))->buf, buf + nTailLeft + i*event->output->blockSize, event->output->blockSize);
			}
			else
			{
				((prefix_evbuffer_block_t *)(ptr + i*sizeSingle))->prev = (prefix_evbuffer_block_t *)(ptr + (i-1)*sizeSingle);
				((prefix_evbuffer_block_t *)(ptr + i*sizeSingle))->next = NULL;
				memcpy(((prefix_evbuffer_block_t *)(ptr + i*sizeSingle))->buf, buf + i*event->output->blockSize, (len-nTailLeft)%event->output->blockSize);
			}
#else
			((prefix_evbuffer_block_t *)(ptr + i*sizeSingle))->prev = (prefix_evbuffer_block_t *)(ptr + (i-1)*sizeSingle);
			((prefix_evbuffer_block_t *)(ptr + i*sizeSingle))->next = (prefix_evbuffer_block_t *)(ptr + (i+1)*sizeSingle);
			if (i != nblock - 1)
			{
				memcpy(((prefix_evbuffer_block_t *)(ptr + i*sizeSingle))->buf, buf + nTailLeft + i*event->output->blockSize, event->output->blockSize);
				((prefix_evbuffer_block_t *)(ptr + i*sizeSingle))->ptrTail = event->output->blockSize;
			}
			else
			{
				memcpy(((prefix_evbuffer_block_t *)(ptr + i*sizeSingle))->buf, buf + nTailLeft + i*event->output->blockSize, (len - nTailLeft - 1)%event->output->blockSize + 1);
				((prefix_evbuffer_block_t *)(ptr + i*sizeSingle))->ptrTail = (len - nTailLeft - 1)%event->output->blockSize + 1;
			}

#endif
		}
		((prefix_evbuffer_block_t *)(ptr))->prev = event->output->blockTail;
		event->output->blockTail->next = (prefix_evbuffer_block_t *)ptr;
		((prefix_evbuffer_block_t *)(ptr + (nblock-1)*sizeSingle))->next = NULL;

		event->output->blockTail = (prefix_evbuffer_block_t *)(ptr + (i-1)*sizeSingle);
	}

	return SUCCESS;
}

void prefix_bufferevent_free(prefix_bufferevent_t *event)
{
	prefix_log("debug", "in");

	if (NULL == event)
	{
		prefix_log("debug", "already freed");
	}

	event->eventStatus |= BUFFEREVENT_STATUS_FREED;

	prefix_log("debug", "out");
}

void prefix_bufferevent_dump(prefix_bufferevent_t *event)
{
	if (NULL == event)
	{
		prefix_log("error", "parameter error");
		return;
	}

	prefix_evbuffer_block_t *ptr = NULL;
	int i;

	printf("--------------------------------------------\n");
	printf("------------ bufferevent dump --------------\n");
	printf("--------------------------------------------\n");
	printf("   event: %p                              \n", event);
	printf("         prev:              %p            \n", event->prev);
	printf("         next:              %p            \n", event->next);
	printf("         activePrev:        %p            \n", event->activePrev);
	printf("         activeNext:        %p            \n", event->activeNext);
	printf("                                          \n");
	printf("         base:              %p            \n", event->base);
	printf("         callback:          %p            \n", event->callback);
	printf("         arg:               %p            \n", event->arg);
	printf("         eventStatus:       %d            \n", event->eventStatus);
	printf("         eventActiveType:   %d            \n", event->eventActiveType);
	printf("                                          \n");
	printf("         attr.blockSize:    %d            \n", event->attr.blockSize);
	printf("         attr.flushType:    %d            \n", event->attr.flushType);
	printf("                                          \n");
	printf("         input:             %p            \n", event->input);
	printf("         input->blockNum:   %d            \n", event->input->blockNum);
	printf("         input->blockSize:  %d            \n", event->input->blockSize);
	printf("         input->blockHead:  %p            \n", event->input->blockHead);
	printf("         input->blockTail:  %p            \n", event->input->blockTail);
	for (i = 0,ptr = event->input->blockHead;ptr;ptr=ptr->next,i++)
	{
	printf("         	block %d:		%p    		  \n", i, ptr);
	printf("            block->prev:    %p            \n", ptr->prev);
	printf("            block->next:    %p            \n", ptr->next);
	printf("            block->length:  %d            \n", ptr->length);
	printf("            block->ptrHead: %d            \n", ptr->ptrHead);
	printf("            block->ptrTail: %d            \n", ptr->ptrTail);
	printf("            block->buf:		%p            \n", ptr->buf);
	}
	printf("                                          \n");
	printf("         output:            %p            \n", event->output);
	printf("         output->blockNum:  %d            \n", event->output->blockNum);
	printf("         output->blockSize: %d            \n", event->output->blockSize);
	printf("         output->blockHead: %p            \n", event->output->blockHead);
	printf("         output->blockTail: %p            \n", event->output->blockTail);
	for (i = 0,ptr = event->output->blockHead;ptr;ptr=ptr->next,i++)
	{
	printf("         	block %d:		%p    		  \n", i, ptr);
	printf("            block->prev:    %p            \n", ptr->prev);
	printf("            block->next:    %p            \n", ptr->next);
	printf("            block->length:  %d            \n", ptr->length);
	printf("            block->ptrHead: %d            \n", ptr->ptrHead);
	printf("            block->ptrTail: %d            \n", ptr->ptrTail);
	printf("            block->buf:		%p            \n", ptr->buf);
	}
	printf("                                          \n");
	printf("--------------------------------------------\n");
}

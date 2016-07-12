
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/uio.h>
#include <unistd.h>

#include "prefix_event.h"
#include "prefix_event_base.h"
#include "prefix_log.h"
#include "prefix_bufferevent.h"

static int prefix_bufferevent_evbuffer_block_free_head(prefix_evbuffer_t *evbuffer);
static int prefix_bufferevent_evbuffer_free(prefix_evbuffer_t *evbuffer);

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
	event->events = events;

	// if use not set, use default.
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

	// input blocksize set as default alwaysly
	prefix_log("debug", "malloc evbuffer input success");
	event->input->blockNum = 0;
	event->input->blockSize = BUFFEREVENT_BLOCKSIZE_DEFAULT;

	event->output = (prefix_evbuffer_t *)prefix_malloc(sizeof(prefix_evbuffer_t));
	if (NULL == event->output)
	{
		prefix_log("error", "malloc evbuffer output error");
		prefix_free(event);
		return NULL;
	}
	memset(event->output, 0, sizeof(prefix_evbuffer_t));

	// output blocksize set as use given or default
	prefix_log("debug", "malloc evbuffer output success");
	event->output->blockNum = 0;
	event->output->blockSize = event->attr.blockSize;

	if (base->useThread)
	{
		result = prefix_event_base_add_bufferevent_use_thread(event);
		if (SUCCESS != result)
		{
			prefix_log("error", "add event to eventbase error");
			prefix_free(event);
			return NULL;
		}
		prefix_log("debug", "add event to base use thread success");
	}
	else
	{
		result = prefix_event_base_add_bufferevent(event);
		if (SUCCESS != result)
		{
			prefix_log("error", "add event to eventbase error");
			prefix_free(event);
			return NULL;
		}
		prefix_log("debug", "add event to base success");
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
		}
		((prefix_evbuffer_block_t *)(ptr))->prev = NULL;
		((prefix_evbuffer_block_t *)(ptr + (nblock-1)*sizeSingle))->next = NULL;

		event->output->blockHead = (prefix_evbuffer_block_t *)(ptr);
		event->output->blockTail = (prefix_evbuffer_block_t *)(ptr + (nblock-1)*sizeSingle);
	}
	else
	{
		size_t nTailLeft = event->output->blockTail->length - event->output->blockTail->ptrTail;

		if (len <= nTailLeft)
		{
			memcpy(event->output->blockTail->buf + event->output->blockTail->ptrTail, buf, len);
			event->output->blockTail->ptrTail += len;
		}
		else
		{
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
			}
			((prefix_evbuffer_block_t *)(ptr))->prev = event->output->blockTail;
			event->output->blockTail->next = (prefix_evbuffer_block_t *)ptr;
			((prefix_evbuffer_block_t *)(ptr + (nblock-1)*sizeSingle))->next = NULL;

			event->output->blockTail = (prefix_evbuffer_block_t *)(ptr + (i-1)*sizeSingle);
		}
	}

	return SUCCESS;
}

// be careful of the situation which returns 0
ssize_t prefix_bufferevent_read(prefix_bufferevent_t *event, void *buf, size_t len)
{
	if (NULL == event || NULL == buf || 0 >= len)
	{
		prefix_log("error", "parameter error");
		return -1;
	}

	// be careful, here return 0 but not -1
	// cause !!!!
	if (0 == event->input->blockNum)
	{
		prefix_log("error", "nothing in buffer");
		return 0;
	}

	prefix_evbuffer_block_t *ptr = NULL;
	prefix_evbuffer_block_t *ptrNext = NULL;

	ptr = event->input->blockHead;
	size_t minsize = 0;
	size_t readsize = 0;

	while (len)
	{
		minsize = (len < ptr->ptrTail - ptr->ptrHead)?len:(ptr->ptrTail - ptr->ptrHead);

		memcpy(buf + readsize, ptr->buf + ptr->ptrHead, minsize);

		ptr->ptrHead += minsize;
		len -= minsize;
		readsize += minsize;

		ptrNext = ptr->next;

		if (ptr->ptrHead == ptr->ptrTail)
		{
			prefix_bufferevent_evbuffer_block_free_head(event->input);
		}

		ptr = ptrNext;
		if (NULL == ptr)
		{
			break;
		}
	}

	return readsize;
}

int prefix_bufferevent_writev_inner(prefix_bufferevent_t *event, int fd, int rmflag)
{
	if (NULL == event || 0 >= fd)
	{
		prefix_log("error", "parameter error");
		return ERROR;
	}

	int result = 0;
	int iovcnt = event->output->blockNum;
	struct iovec *iov = NULL;
	size_t size = 0, lenTotal = 0;
	int i = 0;
	prefix_evbuffer_block_t *ptrblock;

	size = iovcnt * sizeof(struct iovec);
	iov = (struct iovec *)prefix_malloc(size);
	if (NULL == iov)
	{
		prefix_log("error", "malloc iovec array error");
		return ERROR;
	}

	ptrblock = event->output->blockHead;
	for (i = 0; i < iovcnt; ++i)
	{
		iov[i].iov_base = &ptrblock->buf[ptrblock->ptrHead];
		iov[i].iov_len = ptrblock->ptrTail - ptrblock->ptrHead;

		lenTotal += iov[i].iov_len;
		ptrblock = ptrblock->next;
	}

	size = writev(fd, iov, iovcnt);
	if (size != lenTotal)
	{
		prefix_log("error", "writev error");
		return ERROR;
	}

	// rmflag decides whether to free the send buf
	if (0 == rmflag)
	{
		// do nothing
	}
	else
	{
		ptrblock = event->output->blockHead;
		int minsize = 0;
		int remainsize = 0;
		while (size)
		{
			remainsize = ptrblock->ptrTail - ptrblock->ptrHead;
			ptrblock = ptrblock->next;
			minsize = (size > remainsize)?remainsize:size;
			size -= minsize;
			// if all data sended, free the block.
			if (minsize == remainsize)
			{
				result = prefix_bufferevent_evbuffer_block_free_head(event->output);
				if (SUCCESS != result)
				{
					prefix_log("error", "evbuffer block head free error");
				}
			}
			// if not all data sended, only move the ptrTail.
			else
			{
				ptrblock->ptrTail += minsize;
			}

		}
	}

	return SUCCESS;
}

int prefix_bufferevent_readv_inner(prefix_bufferevent_t *event, int fd)
{
	if (NULL == event || 0 >= fd)
	{
		prefix_log("error", "parameter error");
		return ERROR;
	}

	prefix_evbuffer_block_t *ptr;
	size_t size = sizeof(struct prefix_evbuffer_block_s) + event->input->blockSize;

	if (0 == event->input->blockNum)		// blockNum == 0
	{
		ptr = (prefix_evbuffer_block_t *)prefix_malloc(size);
		if (NULL == ptr)
		{
			prefix_log("error", "malloc blocks error");
			return ERROR;
		}
		memset(ptr, 0, size);

		ptr->length = event->input->blockSize;
		event->input->blockHead = ptr;
		event->input->blockTail = ptr;

		event->input->blockNum++;

		size = read(fd, ptr->buf, event->input->blockSize);
		if (0 == size)
		{
			// TODO
		}
		else if (0 < size)
		{
			ptr->ptrTail = size;
		}
		else
		{
			// no need to handle EINTR
			prefix_log("error", "read error");
			exit(-1);
		}
	}
	else	// blockNum != 0
	{
		if (event->input->blockTail->length == event->input->blockTail->ptrTail)
		{

			ptr = (prefix_evbuffer_block_t *)prefix_malloc(size);
			if (NULL == ptr)
			{
				prefix_log("error", "malloc blocks error");
				return ERROR;
			}
			memset(ptr, 0, size);

			ptr->length = event->input->blockSize;
			event->input->blockTail->next = ptr;
			ptr->prev = event->input->blockTail;
			event->input->blockTail = ptr;

			event->input->blockNum++;

			size = read(fd, ptr->buf, event->input->blockSize);
			if (0 == size)
			{
				// TODO
			}
			else if (0 < size)
			{
				ptr->ptrTail = size;
			}
			else
			{
				// no need to handle EINTR
				prefix_log("error", "read error");
				exit(-1);
			}
		}
		else
		{
			ptr = event->input->blockTail;
			size = read(fd, (ptr->buf + ptr->ptrTail), ptr->length - ptr->ptrTail);
			if (0 == size)
			{
				// TODO
			}
			else if (0 < size)
			{
				ptr->ptrTail += size;
			}
			else
			{
				// no need to handle EINTR
				prefix_log("error", "read error");
				exit(-1);
			}
		}
	}

	return SUCCESS;
}

int prefix_bufferevent_set_active(prefix_bufferevent_t *event, int activeType)
{
	prefix_log("debug", "in");

	if (NULL == event)
	{
		prefix_log("error", "parameter error");
		return ERROR;
	}

	event->eventActiveType = activeType;

	int result = prefix_event_base_set_bufferevent_active(event->base, event);
	if (SUCCESS != result)
	{
		prefix_log("error", "set event active error");
		return ERROR;
	}

	prefix_log("debug", "out");
	return SUCCESS;
}

int prefix_bufferevent_invoke(prefix_bufferevent_t *event)
{
	prefix_log("debug", "in");

	if (NULL == event)
	{
		prefix_log("error", "parameter error");
		return ERROR;
	}

	// only invoke the events which use care about
	// void (*cb)(prefix_socket_t fd, short event, void *arg);
	if ((EV_WRITE & event->events) && (EVENT_ACTIVETYPE_BUFFERWRITE & event->eventActiveType))
	{
		event->callback(event->fd, event->eventActiveType & EVENT_ACTIVETYPE_BUFFERWRITE, event->arg);
	}

	if ((EV_READ & event->events) && (EVENT_ACTIVETYPE_BUFFERREAD & event->eventActiveType))
	{
		event->callback(event->fd, event->eventActiveType & EVENT_ACTIVETYPE_BUFFERREAD, event->arg);
	}

	event->eventStatus |= EVENT_STATUS_INVOKED;

	prefix_log("debug", "out");
	return SUCCESS;
}

int prefix_bufferevent_delete(prefix_bufferevent_t *event)
{
	prefix_log("debug", "in");

	if (NULL == event)
	{
		prefix_log("error", "parameter error");
		return ERROR;
	}

	int result = prefix_event_base_remove_bufferevent(event->base, event);
	if (SUCCESS != result)
	{
		prefix_log("error", "remove bufferevent from base error");
		return ERROR;
	}

	prefix_bufferevent_free_inner(event);

	prefix_log("debug", "out");
	return SUCCESS;
}

static int prefix_bufferevent_evbuffer_free(prefix_evbuffer_t *evbuffer)
{
	if (NULL == evbuffer)
	{
		prefix_log("error", "parameter error");
		return ERROR;
	}

	prefix_evbuffer_block_t *ptr;

	ptr = evbuffer->blockHead;

	while (ptr)
	{
		evbuffer->blockHead = ptr->next;

		prefix_free(ptr);
		ptr = evbuffer->blockHead;
	}

	prefix_free(evbuffer);

	return SUCCESS;
}

static int prefix_bufferevent_evbuffer_block_free_head(prefix_evbuffer_t *evbuffer)
{
	if (NULL == evbuffer)
	{
		prefix_log("error", "parameter error");
		return ERROR;
	}

	prefix_evbuffer_block_t *ptr;

	if (NULL == evbuffer->blockHead)
	{
		prefix_log("error", "evbuffer already has no block");
		return ERROR;
	}

	ptr = evbuffer->blockHead;
	evbuffer->blockHead = evbuffer->blockHead->next;
	if (NULL == evbuffer->blockHead)
	{
		evbuffer->blockTail = NULL;
	}

	evbuffer->blockNum --;

	prefix_free(ptr);

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

void prefix_bufferevent_free_inner(prefix_bufferevent_t *event)
{
	prefix_log("debug", "in");

	if (NULL == event)
	{
		prefix_log("debug", "already freed");
	}

	prefix_bufferevent_evbuffer_free(event->input);
	prefix_bufferevent_evbuffer_free(event->output);

	prefix_free(event);

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

	printf("------------------------------------------\n");
	printf("------------ bufferevent dump ------------\n");
	printf("------------------------------------------\n");
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
	printf("         fd:                %d            \n", event->fd);
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
	printf("            block %d:       %p            \n", i, ptr);
	printf("              block->prev:    %p          \n", ptr->prev);
	printf("              block->next:    %p          \n", ptr->next);
	printf("              block->length:  %d          \n", ptr->length);
	printf("              block->ptrHead: %d          \n", ptr->ptrHead);
	printf("              block->ptrTail: %d          \n", ptr->ptrTail);
	printf("              block->buf:     %p          \n", ptr->buf);
	}
	printf("                                          \n");
	printf("         output:            %p            \n", event->output);
	printf("         output->blockNum:  %d            \n", event->output->blockNum);
	printf("         output->blockSize: %d            \n", event->output->blockSize);
	printf("         output->blockHead: %p            \n", event->output->blockHead);
	printf("         output->blockTail: %p            \n", event->output->blockTail);
	for (i = 0,ptr = event->output->blockHead;ptr;ptr=ptr->next,i++)
	{
	printf("            block %d:       %p            \n", i, ptr);
	printf("              block->prev:    %p          \n", ptr->prev);
	printf("              block->next:    %p          \n", ptr->next);
	printf("              block->length:  %d          \n", ptr->length);
	printf("              block->ptrHead: %d          \n", ptr->ptrHead);
	printf("              block->ptrTail: %d          \n", ptr->ptrTail);
	printf("              block->buf:     %p          \n", ptr->buf);
	}
	printf("                                          \n");
	printf("------------------------------------------\n");
}

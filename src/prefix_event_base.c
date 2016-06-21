
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef _USE_SELECT
#include "prefix_event_op_select.h"
#endif

#include "prefix_event_op.h"
#include "prefix_log.h"
#include "prefix_event.h"
#include "prefix_event_base.h"


prefix_event_base_t *prefix_event_base_new()
{
	prefix_event_base_t *base;

	base = (prefix_event_base_t *)prefix_malloc(sizeof(prefix_event_base_t));
	if (NULL == base)
	{
		prefix_log("error", "malloc event base error!");
		return NULL;
	}
	memset(base, 0, sizeof(prefix_event_base_t));

#ifdef _USE_SELECT
	base->eventOps = &selectOps;
#endif

	return base;
}

int prefix_event_base_add_event(int type, prefix_event_t *event)
{
	if (!(type & (EVENT_TYPE_IO | EVENT_TYPE_SIG | EVENT_TYPE_TIME)))
	{
		prefix_log("error", "parameter error");
		return ERROR;
	}

	prefix_event_t **head = NULL;

	switch (type)
	{
	case EVENT_TYPE_IO:
		head = &event->base->eventIOHead;
		break;
	case EVENT_TYPE_SIG:
		head = &event->base->eventSigHead;
		break;
	case EVENT_TYPE_TIME:
		head = &event->base->eventTimeHead;
		break;
	default:
		prefix_log("error", "no such type");
		break;
	}

	if (NULL == *head)
	{
		*head = event;
	}
	else
	{
		while (NULL != (*head)->next)
		{
			head = &(*head)->next;
		}
		(*head)->next = event;
		event->prev = (*head)->next;
	}
	return SUCCESS;
}

void prefix_event_base_dump(prefix_event_base_t *base)
{
	if (NULL == base)
	{
		prefix_log("error", "parameter error!");
		return;
	}

	prefix_event_t *ptr;

	printf("********************************************\n");
	printf("************ event base dump **************\n");
	printf("********************************************\n");
	printf("   base:      %p                                        \n", base);
	printf("         eventOps:        %p                 \n", base->eventOps);
	printf("             op->name:             %s                 \n", base->eventOps->name);
	printf("         eventIOHead:   %p                 \n", base->eventIOHead);
	for (ptr = base->eventIOHead; ptr != NULL; ptr = ptr->next)
	{
	printf("              event:                 %p         \n", ptr);
	}
	printf("         eventSigHead:  %p                 \n", base->eventSigHead);
	for (ptr = base->eventSigHead; ptr != NULL; ptr = ptr->next)
	{
	printf("              event:                 %p         \n", ptr);
	}
	printf("         eventTimeHead: %p                 \n", base->eventTimeHead);
	for (ptr = base->eventTimeHead; ptr != NULL; ptr = ptr->next)
	{
	printf("              event:                 %p         \n", ptr);
	}
	printf("         timeHeap:          %p                 \n", base->timeHeap);
	printf("         eventActive:       %p                 \n", base->eventActive);
	printf("********************************************\n");
}

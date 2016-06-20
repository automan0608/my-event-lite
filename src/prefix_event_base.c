
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef _USE_SELECT
#include "prefix_event_op_select.h"
#endif

#include "prefix_log.h"
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

void prefix_event_base_dump(prefix_event_base_t *base)
{
	if (NULL == base)
	{
		prefix_log("error", "parameter error!");
		return;
	}

	printf("********************************************\n");
	printf("********** event base dump **************\n");
	printf("********************************************\n");
	printf("       base: %p                                         \n", base);
	printf("               eventOps:          %p                 \n", base->eventOps);
	printf("               eventIOHead:     %p                 \n", base->eventIOHead);
	printf("               eventSigHead:    %p                 \n", base->eventSigHead);
	printf("               eventTimeHead: %p                 \n", base->eventTimeHead);
	printf("               timeHeap:          %p                 \n", base->timeHeap);
	printf("               eventActive:       %p                 \n", base->eventActive);
	printf("********************************************\n");
}


#include <stdlib.h>
#include <sys/time.h>
#include <sys/select.h>
#include <errno.h>

#include "prefix_log.h"
#include "prefix_base.h"
#include "prefix_event.h"
#include "prefix_event_base.h"
#include "prefix_min_heap.h"
#include "prefix_event_op.h"
#include "prefix_event_op_select.h"

struct selectop {
        int maxfdp1;          /* Highest fd in fd set */
        int event_fdsz;
        int resize_out_sets;
        fd_set event_readset_in;
        fd_set event_writeset_in;
        fd_set event_readset_out;
        fd_set event_writeset_out;
};

static struct selectop object;

const prefix_event_op_t selectOps = {
        "select",
        select_init,
        select_add,
        select_del,
        select_dispatch,
};

void *select_init(prefix_event_base_t *base)
{
        FD_ZERO(&object.event_readset_in);
        FD_ZERO(&object.event_writeset_out);

        object.maxfdp1 = 0;

        return NULL;
}

int select_add(prefix_event_base_t *base, int fd, short old, short events, void *arg)
{
        int flag = 0;
        if (events & EV_READ)
        {
                FD_SET(fd, &object.event_readset_in);
                if (fd > object.maxfdp1 - 1)
                {
                        object.maxfdp1 = fd + 1;
                }
                flag = 1;
        }

        if (events & EV_WRITE)
        {
                FD_SET(fd, &object.event_writeset_out);
                if (fd > object.maxfdp1 - 1)
                {
                        object.maxfdp1 = fd + 1;
                }
                flag = 1;
        }

        // to tell the caller whether the event has been added to the reactor
        return flag;
}

int select_del(prefix_event_base_t *base, int fd, short old, short events, void *arg)
{
        return SUCCESS;
}

int select_dispatch(prefix_event_base_t *base, struct timeval *tv)
{
        if (NULL == base)
        {
                prefix_log("error", "parameter error");
                return ERROR;
        }

        int result = 0;
        struct timeval *tvMinHeapGet;
        struct timeval tvNow;

        struct timeval tvSelect;
        // cause select will
        tvSelect.tv_sec = tv->tv_sec;
        tvSelect.tv_usec = tv->tv_usec;

        // for test
        prefix_log("debug", "maxfdp1:%d", object.maxfdp1);
        result = select(object.maxfdp1, &object.event_readset_in,
                                &object.event_writeset_out, NULL, &tvSelect);
        if (0 > result)
        {
                prefix_log("error", "select error");
                return ERROR;
        }
        else if (0 == result)
        {
                prefix_log("debug", "select timeout");
                // time events
                while (NULL != (tvMinHeapGet = prefix_min_heap_get_top(base->timeHeap)))
                {
                        gettimeofday(&tvNow, NULL);
                        result = prefix_base_timeval_cmp(tvNow, *tvMinHeapGet);
                        if (0 <= result)
                        {
                                // will set eventStatus
                                prefix_event_set_active(prefix_min_heap_pop(base->timeHeap));
                        }
                        else
                        {
                                break;
                        }
                }
        }
        else
        {
                prefix_log("debug", "select event happens");
                // IO events
                prefix_event_t *ptr;

                for (ptr=base->eventIOHead;ptr;ptr=ptr->next)
                {
                        if(FD_ISSET(ptr->ev.io.fd, &object.event_readset_in))
                        {
                                prefix_log("debug", "readset io fd:%d ok",
                                                ptr->ev.io.fd);
                                prefix_event_set_active(ptr);
                        }
                }
        }

        return SUCCESS;
}

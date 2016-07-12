
#include <stdlib.h>
#include <sys/time.h>
#include <sys/select.h>
#include <errno.h>

#include "prefix_log.h"
#include "prefix_base.h"
#include "prefix_event.h"
#include "prefix_bufferevent.h"
#include "prefix_event_base.h"
#include "prefix_event_signal.h"
#include "prefix_min_heap.h"
#include "prefix_event_op.h"
#include "prefix_event_op_select.h"
#include "prefix_pipe.h"

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

        prefix_log("debug", "select result:%d", result);
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
                                prefix_event_set_active(prefix_min_heap_pop(base->timeHeap), EVENT_ACTIVETYPE_TIMEOUT);
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
                prefix_bufferevent_t *ptrbuf;

                for (ptr=base->eventIOHead;ptr;ptr=ptr->next)
                {
                        if(FD_ISSET(ptr->ev.io.fd, &object.event_readset_in))
                        {
                                prefix_log("debug", "readset io fd:%d ok",
                                                ptr->ev.io.fd);
                                prefix_event_set_active(ptr, EVENT_ACTIVETYPE_GENERIC);
                        }
                }

                if (FD_ISSET(base->notifyFd[0], &object.event_readset_in))
                {
                        prefix_log("debug", "base notifyFd:%d ok", base->notifyFd[0]);

                        int signo = 0;
                        char buf[1] = {0};
                        prefix_event_t          *ptrread = NULL;
                        prefix_bufferevent_t    *ptrbufread = NULL;

                        // read the type of the notify
                        result = prefix_pipe_read(base->notifyFd[0], buf, 1);
                        if (SUCCESS != result)
                        {
                                prefix_log("error", "read notify type error");
                                return ERROR;
                        }

                        switch(buf[0])
                        {
                        case NOTIFYTYPE_SIGNAL_COME:
                                if (0 >= (signo = prefix_event_signal_read(base)))
                                {
                                        prefix_log("error", "read signal error");
                                }
                                else
                                {
                                        for (ptr=base->eventSigHead;ptr;ptr=ptr->next)
                                        {
                                                if(signo == ptr->ev.sig.signo)
                                                {
                                                        prefix_event_set_active(ptr, EVENT_ACTIVETYPE_GENERIC);
                                                }
                                        }
                                }
                                break;
                        case NOTIFYTYPE_EVENT_NEW:
                                result = prefix_pipe_read(base->notifyFd[0], &ptrread, sizeof(prefix_event_t *));
                                if (SUCCESS != result)
                                {
                                        prefix_log("error", "read notify type error");
                                        return ERROR;
                                }

                                result = prefix_event_base_add_event(ptrread);
                                if (SUCCESS != result)
                                {
                                        prefix_log("error", "base add event error");
                                        return ERROR;
                                }

                                break;
                        case NOTIFYTYPE_BUFFEREVENT_NEW:
                                result = prefix_pipe_read(base->notifyFd[0], &ptrbufread, sizeof(prefix_bufferevent_t *));
                                if (SUCCESS != result)
                                {
                                        prefix_log("error", "read notify type error");
                                        return ERROR;
                                }

                                result = prefix_event_base_add_bufferevent(ptrbufread);
                                if (SUCCESS != result)
                                {
                                        prefix_log("error", "base add event error");
                                        return ERROR;
                                }

                                break;
                        default:
                                prefix_log("error", "no such type of notify");
                                break;
                        }

                }

                for (ptrbuf=base->buffereventHead; ptrbuf; ptrbuf=ptrbuf->next)
                {
                        if (FD_ISSET(ptrbuf->fd, &object.event_readset_in))
                        {
                                prefix_log("debug", "buffer event fd:%d readset ok", ptrbuf->fd);

                                prefix_bufferevent_readv_inner(ptrbuf, ptrbuf->fd);

                                // TODO need to handle the return value

                                prefix_bufferevent_set_active(ptrbuf, EVENT_ACTIVETYPE_BUFFERREAD);
                        }
                        if (FD_ISSET(ptrbuf->fd, &object.event_writeset_out))
                        {
                                prefix_log("debug", "buffer event fd:%d writeset ok", ptrbuf->fd);

                                // TODO set flag
                                int rmflag = 1;
                                prefix_bufferevent_writev_inner(ptrbuf, ptrbuf->fd, rmflag);

                                // TODO need to handle the return value

                                prefix_bufferevent_set_active(ptrbuf, EVENT_ACTIVETYPE_BUFFERWRITE);
                        }
                }
        }
        return SUCCESS;
}

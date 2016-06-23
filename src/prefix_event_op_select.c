
#include <stdlib.h>
#include <sys/time.h>
#include <sys/select.h>
#include <errno.h>

#include "prefix_event_op.h"
#include "prefix_event_op_select.h"

struct selectop {
        int event_fds;          /* Highest fd in fd set */
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

        return NULL;
}

int select_add(prefix_event_base_t *base, int fd, short old, short events, void *arg)
{
        int flag = 0;
        if (events & PREFIX_EV_READ)
        {
                FD_SET(fd, object.event_readset_in);
                flag = 1;
        }

        if (events & PREFIX_EV_WRITE)
        {
                FD_SET(fd, object.event_writeset_in);
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
        return SUCCESS;
}

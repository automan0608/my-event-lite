
#include "prefix_event_op_select.h"

struct selectop {
        int event_fds;          /* Highest fd in fd set */
        int event_fdsz;
        int resize_out_sets;
        fd_set *event_readset_in;
        fd_set *event_writeset_in;
        fd_set *event_readset_out;
        fd_set *event_writeset_out;
};

const prefix_event_op_t selectOps = {
        "select",
        select_init,
        select_add,
        select_del,
        select_dispatch,
};


void *select_init(prefix_event_base_t *base)
{
        return;
}

int select_add(prefix_event_base_t *base, int fd, short old, short events, void *arg)
{
        return 0;
}

int select_del(prefix_event_base_t *base, int fd, short old, short events, void *arg)
{
        return 0;
}

int select_dispatch(prefix_event_base_t *, struct timeval *)
{
        return 0;
}

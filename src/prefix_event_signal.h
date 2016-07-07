
#ifndef _PREFIX_EVENT_SIGNAL_H_
#define _PREFIX_EVENT_SIGNAL_H_

#include "prefix_core.h"

void prefix_event_signal_write(int signo);

int prefix_event_signal_read(prefix_event_base_t *base);

int prefix_event_signal_register(prefix_event_base_t *base, int signo);

int prefix_event_signal_unregister(int signo);

#endif

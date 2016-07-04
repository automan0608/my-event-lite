#include <stdio.h>

#include "prefix_event_signal.h"

void signal_handler(int signo)
{
	printf("In signal handler\n");
}

#include <stdio.h>
#include <signal.h>

#include "prefix_log.h"
#include "prefix_event_base.h"
#include "prefix_pipe.h"

#include "prefix_event_signal.h"

static int notifyFd[2] = {0};

void prefix_event_signal_write(int signo)
{
	prefix_log("debug", "write signo:%d", signo);

	if (0 >= signo)
	{
		prefix_log("error", "parameter error");
		return;
	}

	int result = 0;
	char buf[1] = {0};
	buf[0] = (char)signo;

	result = prefix_pipe_write(notifyFd[1], buf, 1);
	if (SUCCESS != result)
	{
		prefix_log("error", "signal write error");
		return;
	}

	prefix_log("debug", "signal write success");
}

// return the signum
int prefix_event_signal_read(prefix_event_base_t *base)
{
	prefix_log("debug", "read");

	if (NULL == base)
	{
		prefix_log("error", "parameter error");
		return ERROR;
	}

	int result = 0;
	char buf[1] = {0};

	result = prefix_pipe_read(notifyFd[0], buf, 1);
	if (SUCCESS != result)
	{
		prefix_log("error", "signal read error");
		return ERROR;
	}

	prefix_log("debug", "signal read success");

	return (int)buf[0];
}

int prefix_event_signal_register(prefix_event_base_t *base, int signo)
{
	prefix_log("debug", "in, signo:%d", signo);

	if (SIG_ERR == signal(signo, prefix_event_signal_write))
	{
		prefix_log("error", "signal register error");
		return ERROR;
	}

	notifyFd[0] = base->notifyFd[0];
	notifyFd[1] = base->notifyFd[1];

	prefix_log("debug", "out");
	return SUCCESS;
}

int prefix_event_signal_unregister(int signo)
{
	prefix_log("debug", "in, signo:%d", signo);

	if (SIG_ERR == signal(signo, SIG_DFL))
	{
		prefix_log("error", "signal unregister error");
		return ERROR;
	}

	prefix_log("debug", "out");
	return SUCCESS;
}

